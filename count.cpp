#include "count.hpp"
#include <cstdio>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

static entry *list;
__attribute__((visibility("hidden"))) thread_local entry *mine;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

__attribute__((constructor, no_instrument_function)) //
static void entry_allocator() {
    pthread_mutex_lock(&lock);
    mine = list = new entry{gettid(), 0, list};
    pthread_mutex_unlock(&lock);
}

__attribute__((destructor, no_instrument_function)) //
static void entry_dumper() {
    unsigned long total = 0;
    for (entry *e = list, *next; e; e = next) {
        next = e->next;
        printf("tid=%d count=%lu\n", e->tid, e->count);
        total += e->count;
        delete e;
    }
    printf("total=%lu\n", total);
}

static int (*orig)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) = nullptr;

struct targs {
    void *(*func)(void *);
    void *arg;
};

__attribute__((no_instrument_function)) //
static auto wrapper(void *arg) {
    targs *args    = (targs *)arg;
    auto orig_func = args->func;
    auto orig_arg  = args->arg;

    entry_allocator();
    delete args;
    return orig_func(orig_arg);
};

extern "C" {
__attribute__((no_instrument_function)) //
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, //
                   void *(*func)(void *), void *arg) {
    if (!orig)
        orig = (decltype(orig))dlsym(RTLD_NEXT, "pthread_create");
    if (!orig)
        return -1;

    return orig(thread, attr, wrapper, new targs{func, arg});
}
}
