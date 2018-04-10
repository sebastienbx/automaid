CC=gcc
CFLAGS=-Wall -Wextra -W -pedantic -Wbad-function-cast -Wcast-align \
		 -Wcast-qual -Wchar-subscripts -Winline -std=c99 -D_GNU_SOURCE \
		 -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wnonnull \
		 -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

# To use mudflap, then you could type following in the console you run the app
# export MUDFLAP_OPT='-trace-calls -verbose-trace -collect-stats \
#                     -internal-checking -print-leaks -check-initialization \
#                     -verbose-violations' to have many debug outputs.
#CFLAGS+= -fmudflap
#LDFLAGS+= -lmudflap

# Verbose Detection
#CFLAGS+= -DDBG_DETECTION
#CFLAGS+= -DDBG_EVENT
#CFLAGS+= -DDBG_CDF24
#CFLAGS+= -DDBG_MOVAV

# Use dynamic allocations
#CFLAGS+= -DRECURSIVEFILTER_DYNAMIC_ALLOCATION
#CFLAGS+= -DMOVINGAVERAGE_DYNAMIC_ALLOCATION
#CFLAGS+= -DTRIGGEREDBUFFER_DYNAMIC_ALLOCATION
#CFLAGS+= -DMERMAIDPROCESSING_DYNAMIC_ALLOCATION

# Sampling rate = 20 Hz, for 40 Hz, keep the following line commented
#CFLAGS+= -DSAMPLE_RATE_20Hz

# Always do the STA / LTA
CFLAGS+= -DCONTINUOUS_STALTA

# Long integers operations implementation 
#CFLAGS+= -DLONG_OPS_PARANOID
#CFLAGS+= -DLONG_OPS_LAZY

# Debug or release
CFLAGS+= -g
#CFLAGS+= -O3 -fomit-frame-pointer -funroll-all-loops 


#EXEC=MovingAverage_test cdf24_test RecursiveFilter_test long_ops_test \
#	wt_scales_power_test stalta Detection_test double2frac Event_test

EXEC=icdf24_v103_test

# two different configurations of the make file allow to compiile program cdf24_test
# one first, whose lines are indicated with #1 first creates object files and then 
# compiles them with cdf24_test.c
# the second version, #2, takes all c files at once and compiles them in one single
# run of gcc; to see this uncomment either lines with numbers #1 or #2;
#1
#SRC=long_ops.o cdf24.o 
#2
SRC=long_ops.c icdf24.c
#3
#SRC=long_ops.c cdf24_2.c 

.PHONY: clean doc

all: tests

tests: $(EXEC)

doc:
	@doxygen doxygen.cfg

##1
#%_test: $(SRC) %.o %_test.c      
#	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 
##1
#%.o: %.h %.c
#	$(CC) $(CFLAGS) -fPIC -c $*.c -o $@

#2
%_v103_test: $(SRC) %.c  %_test.c        	            
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)  

#%_v101_test: $(SRC) %_1.c  %_test_1.c        	            
#	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)  

clean:
	@rm -vf *.o $(EXEC) 

