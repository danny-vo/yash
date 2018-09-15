GCC = gcc

SYMFLAGS = -g -lreadline

OPTFLAGS =#-O 
CFLAGS = $(OPTFLAGS) $(SYMFLAGS)

PROGRAM = yash

CSRCS = $(shell ls *.c)

SRCS = $(CSRCS)

OBJS = $(CSRCS:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(GCC) -o $@ $(OBJS) $(CFLAGS)

test: $(PROGRAM)
	./$(PROGRAM)

gdb: $(PROGRAM)
	gdb ./$(PROGRAM)

valgrind: $(PROGRAM)
	valgrind --leak-check=yes ./$(PROGRAM)

clean:
	-rm -f $(OBJS) $(PROGRAM)
