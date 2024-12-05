#include <cstdio>
#include <string>
#include <thread>
#include <vector>

__attribute__((noinline))
int fib(int n) { return n > 1 ? fib(n - 1) + fib(n - 2) : 1; }

int main(int argc, char **argv) {
    for (auto i = 0; i < 45; i++)
        printf("fib(%d) = %d\n", i, fib(i));

    if (argc <= 1 || std::string{argv[1]} != "--no-threads") {
        std::vector<std::thread> threads;
        for (auto i = 0; i < 45; i++)
            threads.emplace_back([](int i) { printf("fib(%d) = %d\n", i, fib(i)); }, i);
        for (auto &t : threads)
            t.join();
    }
}
