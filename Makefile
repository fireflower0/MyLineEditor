CC      = gcc
OBJS    = main.o
PROGRAM = MyLineEditor

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)

clean:
	rm -f *.o *~ $(PROGRAM)
