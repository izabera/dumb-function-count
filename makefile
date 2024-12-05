all: libs bins
.PHONY: all clean libs bins profiled
clean:
	$(RM) baseline lib* with* *.o gprof gmon.out



libs: override CXXFLAGS += -fcf-protection=none -O3
libs: libcount.a libcount.so

libcount.so: override CXXFLAGS += -fPIC
libcount.so: override LDFLAGS += -shared
libcount.so: count.cpp
	$(LINK.cc) $^ $(LDLIBS) -o $@

libcount.a: count.o
	$(AR) rcs $@ $^




CXXFLAGS = -O3

bins: baseline profiled

fib.o: override CXXFLAGS += -pg -mfentry

baseline: fib.o
	$(LINK.cc) $^ $(LDLIBS) -o $@

profiled: override LDLIBS += -L.
profiled: gprof with-static with-dynamic libs

with-static: override LDLIBS += -l:libcount.a
with-static: fib.o libcount.a
	$(LINK.cc) $< $(LDLIBS) -o $@

with-dynamic: fib.o libcount.so
	$(LINK.cc) $< $(LDLIBS) -o $@

gprof: fib.o
	$(LINK.cc) $< $(LDLIBS) -o $@
