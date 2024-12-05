### super lightweight profiler that counts the number of function calls in each thread

compile your code with `-pg -mfentry`, then run
`LD_PRELOAD=./libcount.so ./your-program-here` to get a function call tally

or just link against it, without the preload

really you want to link against the static library, as this is so lightweight
that the overhead of going through the plt is actually measurable

the profiling function is **3 instructions**, a *lot* lighter than gprof

note: it needs to override `pthread_create`.  statically linked binaries that
spawn threads are not suppported


### results

`fib.cpp` is a function-call-heavy example that displays what's likely to be
the worst-case performance impact for all profilers of this kind


built with gcc 14, running on a decent laptop, all in single-threaded mode to
reduce the variability due to the threads:

<details><summary>
baseline - avg 5.204s
</summary><pre><code>
real    5.215   user    5.213   sys     0.002
real    5.203   user    5.201   sys     0.001
real    5.202   user    5.201   sys     0.001
real    5.202   user    5.201   sys     0.001
real    5.203   user    5.203   sys     0.000
real    5.202   user    5.201   sys     0.001
real    5.202   user    5.201   sys     0.000
real    5.203   user    5.202   sys     0.000
real    5.203   user    5.202   sys     0.000
real    5.202   user    5.200   sys     0.001
</code></pre></details>

<details><summary>
with-static - avg 6.520s - slowdown 1.25x
</summary><pre><code>
real    6.574   user    6.573   sys     0.001
real    6.572   user    6.571   sys     0.000
real    6.518   user    6.517   sys     0.001
real    6.541   user    6.541   sys     0.000
real    6.507   user    6.505   sys     0.001
real    6.482   user    6.481   sys     0.000
real    6.475   user    6.472   sys     0.002
real    6.488   user    6.486   sys     0.001
real    6.539   user    6.538   sys     0.000
real    6.508   user    6.507   sys     0.001
</code></pre></details>

<details><summary>
with-dynamic - avg 9.360s - slowdown 1.80x
</summary><pre><code>
real    9.349   user    9.348   sys     0.001
real    9.385   user    9.383   sys     0.001
real    9.369   user    9.367   sys     0.002
real    9.355   user    9.354   sys     0.000
real    9.338   user    9.338   sys     0.000
real    9.411   user    9.408   sys     0.002
real    9.348   user    9.346   sys     0.001
real    9.331   user    9.331   sys     0.000
real    9.343   user    9.340   sys     0.002
real    9.375   user    9.373   sys     0.001
</code></pre></details>

<details><summary>
gprof - avg 23.435s - slowdown 4.50x
</summary><pre><code>
real    24.822  user    24.819  sys     0.001
real    22.951  user    22.948  sys     0.000
real    22.998  user    22.996  sys     0.000
real    22.944  user    22.942  sys     0.001
real    23.389  user    23.386  sys     0.002
real    23.389  user    23.387  sys     0.000
real    22.969  user    22.963  sys     0.003
real    24.552  user    24.550  sys     0.000
real    23.388  user    23.384  sys     0.002
real    22.952  user    22.949  sys     0.001
</code></pre></details>
