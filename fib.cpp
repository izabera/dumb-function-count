#include <cstdio>
#include <thread>
#include <vector>

int fib(int n) { return n > 1 ? fib(n - 1) + fib(n - 2) : 1; }

int main() {
    for (auto i = 0; i < 45; i++)
        printf("fib(%d) = %d\n", i, fib(i));
    std::vector<std::thread> threads;
    for (auto i = 0; i < 45; i++)
        threads.emplace_back([](int i) { printf("fib(%d) = %d\n", i, fib(i)); }, i);
    for (auto &t : threads)
        t.join();
}
