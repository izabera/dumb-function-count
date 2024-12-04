all: fib count.so
.PHONY: all clean
clean:
	$(RM) fib count.so gmon.out
count.so: override CFLAGS += -fPIC -shared -fcf-protection=none -O3
count.so: count.c
	$(LINK.c) $^ $(LDLIBS) -o $@
fib: override CXXFLAGS += -pg -mfentry
fib: fib.cpp
