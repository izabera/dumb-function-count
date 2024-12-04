all: fib fibnoprof count.so
.PHONY: all clean
clean:
	$(RM) fib fibnoprof count.so gmon.out

count.so: override CXXFLAGS += -fPIC -shared -fcf-protection=none -O3
count.so: count.cpp
	$(LINK.cc) $^ $(LDLIBS) -o $@

CXXFLAGS = -O3

fib: override CXXFLAGS += -pg -mfentry
fib: fib.cpp

fibnoprof: fib.cpp
	$(LINK.cc) $^ $(LDLIBS) -o $@
