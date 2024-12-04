compile your code with `-pg -mfentry`, then run
`LD_PRELOAD=./count.so ./your-program-here` to get a function call tally

(it also works if you just link against it, without the preload)

the profiling function is 3 instructions, so it's a *lot* lighter than gprof



`fib.cpp` is a function-call-heavy example that displays what's likely to be
the worst-case performance impact for all profilers of this kind

it also spawns a lot of threads, so there's some variability in the results



(todo: update timings)

running on a decent laptop:


#### clang - baseline: 6.108s avg
```
real    6.104   user    8.945   sys     0.003
real    6.076   user    8.972   sys     0.003
real    6.152   user    8.963   sys     0.001
real    6.103   user    8.939   sys     0.004
real    6.110   user    8.956   sys     0.004
real    6.076   user    8.932   sys     0.004
real    6.101   user    8.959   sys     0.003
real    6.102   user    8.943   sys     0.006
real    6.109   user    8.933   sys     0.003
real    6.149   user    8.939   sys     0.003
```
#### clang - gprof: 87.360s avg (14.3x slowdown)
```
real    89.609  user    297.723 sys     0.053
real    90.673  user    315.313 sys     0.071
real    89.120  user    303.381 sys     0.050
real    87.885  user    308.180 sys     0.057
real    80.900  user    265.258 sys     0.076
real    84.617  user    290.682 sys     0.047
real    87.815  user    310.882 sys     0.087
real    90.332  user    299.866 sys     0.030
real    84.567  user    306.601 sys     0.097
real    88.079  user    321.815 sys     0.105
```
#### clang - preload: 13.326s avg (2.2x slowdown)
```
real    13.297  user    19.412  sys     0.007
real    13.337  user    19.363  sys     0.005
real    13.395  user    19.430  sys     0.007
real    13.320  user    19.421  sys     0.004
real    13.318  user    19.406  sys     0.008
real    13.219  user    19.359  sys     0.006
real    13.354  user    19.454  sys     0.010
real    13.342  user    19.356  sys     0.004
real    13.335  user    19.400  sys     0.006
real    13.344  user    19.445  sys     0.007
```

#### gcc - baseline: 5.962s avg
```
real    5.905   user    8.631   sys     0.007
real    5.912   user    8.619   sys     0.003
real    5.931   user    8.654   sys     0.003
real    5.979   user    8.715   sys     0.005
real    5.951   user    8.680   sys     0.005
real    6.002   user    8.733   sys     0.004
real    5.988   user    8.724   sys     0.003
real    6.033   user    8.710   sys     0.010
real    5.917   user    8.692   sys     0.002
real    5.998   user    8.713   sys     0.004
```
#### gcc - gprof: 86.880s avg (14.6x slowdown)
```
real    90.701  user    311.642 sys     0.042
real    87.811  user    312.863 sys     0.075
real    91.727  user    299.173 sys     0.044
real    76.513  user    264.368 sys     0.061
real    86.957  user    306.667 sys     0.072
real    87.873  user    295.090 sys     0.068
real    86.061  user    297.014 sys     0.077
real    79.614  user    266.994 sys     0.055
real    90.797  user    313.830 sys     0.115
real    90.745  user    305.190 sys     0.124
```
#### gcc - preload: 13.055s avg (2.2x slowdown)
```
real    13.008  user    19.015  sys     0.005
real    13.070  user    19.070  sys     0.006
real    13.088  user    18.926  sys     0.003
real    13.092  user    18.972  sys     0.004
real    13.051  user    19.046  sys     0.004
real    13.118  user    19.040  sys     0.004
real    13.051  user    19.030  sys     0.006
real    13.000  user    18.999  sys     0.006
real    13.035  user    19.100  sys     0.005
real    13.042  user    18.905  sys     0.002
```
