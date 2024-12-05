#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstdio>
#include <dlfcn.h>
#include <mutex>
#include <pthread.h>
#include <unistd.h>

/* extremely lightweight thread-safe function counter
 *
 * each thread writes to a thread_local counter, and registers it into a global
 * list when it's destroyed.  the main thread traverses it at the end and prints
 * out the tally
 *
 * it works with asan and tsan
 */
namespace {

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
__attribute__((tls_model("initial-exec"))) thread_local size_t count;

struct entry {
    int tid;
    size_t count;
    entry *next;
} *list;

/* there is no proper api to register a callback to run at thread creation, so
 * we wrap pthread_create instead, as this is meant to be LD_PRELOADed anyway
 */
using orig_type = int (*)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
orig_type orig;

struct thread {
    std::mutex m;
    ~thread() {
        std::unique_lock lock{m};
        list = new entry{gettid(), count, list};
    }
};

extern thread_local thread this_thread;

struct main_thread {
    main_thread() {
        orig = reinterpret_cast<orig_type>(dlsym(RTLD_NEXT, "pthread_create"));
        (void) this_thread;
    }
    ~main_thread() {
        size_t total = 0;
        for (entry *e = list, *next; e; e = next) {
            fprintf(stderr, "tid=%d count=%zu\n", e->tid, e->count);
            total += e->count;
            next = e->next;
            delete e;
        }
        fprintf(stderr, "total=%zu\n", total);
    }
} main_thread;
thread_local thread this_thread;

struct targs {
    void *(*func)(void *);
    void *arg;
};

void *wrapper(void *arg) {
    auto *args = static_cast<targs *>(arg);
    auto orig_func = args->func;
    auto orig_arg = args->arg;
    delete args;

    /* the whole point of wrapping pthread_create is to touch these
     * or the constructors/destructors will never run
     */
    (void) this_thread;
    (void) main_thread;

    return orig_func(orig_arg);
};

} // namespace

extern "C" {
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*func)(void *), void *arg) {
    return orig(thread, attr, wrapper, new targs{func, arg});
}

#if ASM_VERSION
__attribute__((naked))
void __fentry__(){
    asm("mov count@gottpoff(%rip), %rax\n"
        "incq %fs:(%rax)\n"
        "ret\n");
}
#else
/* initial versions of this used the no_caller_saved_registers and
 * target("general-regs-only") attributes, which are supported by both gcc and
 * clang on x86.  with those, this function is guaranteed to be safe (i think)
 *
 * with -O3, in both gcc and clang, this only saves/restores rax
 *
 * however, it seems fine to dirty rax in the special case of __fentry__
 * (it might break with lto...?)
 *
 * pick 2 of the following 3:
 * - write this in c or c++
 * - not save useless registers
 * - not touch registers other than rax without depending on -O3
 *
 * this file will be compiled with optimisations anyway
 *
 * neither the attributes nor the inline asm will work on arm
 * i don't know how to do this on arm :(
 */
void __fentry__() { count++; }
#endif
}
