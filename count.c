#define _GNU_SOURCE
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>

/* extremely lightweight thread-safe function counter
 *
 * each thread allocates a private entry and registers it into a global list,
 * which is traversed at the end and the tally is printed out
 *
 * it works with asan
 * it doesn't work with tsan but i don't get why
 */
struct entry {
    int tid;
    size_t count;
    struct entry *next;
};
static struct entry *list;

/* a call to __fentry__ is added at the beginning of (potentially) *every*
 * function call in the program, and the program itself won't expect changes in
 * registers, so it needs to save all the registers it touches.  this *really*
 * needs to be compiled with optimisations
 *
 * it's especially important that the thread-local entry uses the initial-exec
 * tls model.   absolutely avoid the global-dynamic or local-dynamic models, as
 * those need to call __tls_get_addr to get the address, and we'd have to save
 * and restore like 37 registers on every single call, and the overhead would
 * be exponentially more
 */
__attribute__((tls_model("initial-exec")))
static thread_local struct entry *mine;

#if PLAY_IT_SAFE
__attribute__((used,no_caller_saved_registers,target("general-regs-only")))
void __fentry__() { mine->count++; }
#else
/* actually it seems fine to touch rax in __fentry__
 *
 * pick 2 of the following 3:
 * - write this in c
 * - not save useless registers
 * - not touch registers other than rax without depending on -O3
 *
 * this other version didn't save the registers properly on arm anyway :(
 *
 * it might break with lto...?
 */
__attribute__((used,naked))
void __fentry__(){
    asm("mov mine@gottpoff(%rip), %rax\n"
        "mov %fs:(%rax), %rax\n"
        "incq 8(%rax)\n"
        "ret\n");
}
#endif

/* there is no proper api to register a callback to run at thread creation, so
 * we wrap pthread_create instead, as this is meant to be LD_PRELOADed anyway
 */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void entry_allocator() {
    mine = (struct entry *)malloc(sizeof *mine);
    mine->tid = gettid();
    mine->count = 0;

    pthread_mutex_lock(&lock);
    mine->next = list;
    list = mine;
    pthread_mutex_unlock(&lock);
}

__attribute__((destructor)) static void entry_dumper() {
    unsigned long total = 0;
    for (struct entry *e = list, *next; e; e = next) {
        next = e->next;
        total += e->count;
        fprintf(stderr, "tid=%d count=%lu\n", e->tid, e->count);
        free(e);
    }
    fprintf(stderr, "total=%lu\n", total);
}

static int (*orig)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) = NULL;

// library constructor registers the main thread into the linked list
__attribute__((constructor)) static void constructor() {
    entry_allocator();
    orig = dlsym(RTLD_NEXT, "pthread_create");
}

struct targs {
    void *(*func)(void *);
    void *arg;
};

static void *wrapper(void *arg) {
    struct targs *args = arg;
    void *(*orig_func)(void *) = args->func;
    void *orig_arg = args->arg;

    entry_allocator();
    free(arg);
    return orig_func(orig_arg);
};

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*func)(void *), void *arg) {
    struct targs *args = malloc(sizeof *args);

    args->func = func;
    args->arg = arg;

    return orig(thread, attr, wrapper, args);
}
