project('dumb_count', 'cpp')

count = files('count.cpp')
count_args = ['-fcf-protection=none', '-O3']

count_a  = static_library('count', count, cpp_args: count_args)
count_so = shared_library('count', count, cpp_args: count_args)

fib = files('fib.cpp')
fib_prof = ['-pg', '-mfentry']

fib_base  = executable('baseline',     fib)
fib_gprof = executable('gprof',        fib, cpp_args: fib_prof)
fib_stat  = executable('with-static',  fib, cpp_args: fib_prof, link_with: count_a)
fib_dyn   = executable('with-dynamic', fib, cpp_args: fib_prof, link_with: count_so)
