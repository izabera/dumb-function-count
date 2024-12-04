compile your code with `-pg -mfentry`, then run
`LD_PRELOAD=./count.so ./your-program-here` to get a function call tally

the profiling function is 4 instructions, so it's a *lot* lighter than gprof



`fib.cpp` is a function-call-heavy example that displays what's likely to be
the worst-case performance impact for all profilers of this kind

it also spawns a lot of threads, so there's some variability in the results



compiled with gcc 14.2, running on a decent laptop:

### baseline - avg 4.647s
```
$ for i in {1..10}; do time ./fibnoprof &>/dev/null; done
real    4.923   user    6.778   sys     0.010
real    4.892   user    6.802   sys     0.005
real    4.831   user    6.809   sys     0.005
real    3.526   user    5.389   sys     0.005
real    5.363   user    7.435   sys     0.003
real    5.122   user    6.989   sys     0.005
real    3.988   user    5.643   sys     0.005
real    5.441   user    7.441   sys     0.006
real    4.318   user    6.301   sys     0.009
real    4.074   user    6.383   sys     0.003
```

### glibc/gprof profile - avg 17.053s - 3.67x slowdown
```
$ for i in {1..10}; do time ./fib &>/dev/null; done
real    18.836  user    51.325  sys     0.012
real    16.507  user    53.748  sys     0.014
real    14.256  user    44.927  sys     0.020
real    16.818  user    46.404  sys     0.012
real    15.911  user    50.439  sys     0.009
real    16.339  user    48.921  sys     0.013
real    15.025  user    46.956  sys     0.013
real    17.909  user    52.738  sys     0.018
real    18.294  user    50.377  sys     0.009
real    20.634  user    57.284  sys     0.013
```

### LD_PRELOAD=./count.so - avg 5.417s - 1.16x slowdown
```
$ for i in {1..10}; do time LD_PRELOAD=./count.so ./fib &>/dev/null; done
real    6.609   user    8.808   sys     0.006
real    6.203   user    8.734   sys     0.008
real    5.938   user    8.327   sys     0.008
real    4.322   user    6.787   sys     0.004
real    4.431   user    6.501   sys     0.003
real    5.856   user    8.329   sys     0.006
real    6.121   user    8.701   sys     0.011
real    4.160   user    6.644   sys     0.012
real    5.003   user    7.447   sys     0.005
real    5.532   user    7.675   sys     0.007
```
