CC = mpicc
EX = mpiexec
EXEC = master slave
# SRC = master.c slave.c
# OBJ=$(SRC:.c=.o)
CFLAGS = -fopenmp -std=c99 -O2
stat=diff
n=1
p=4
t=5
a=abcdefghijklmnopqrstuvwxyz
r=3
m=lol

all: $(EXEC)

exec: $(EXEC)
	$(EX) -np $(n) master -p $(p) -t $(t) -a $(a) -r $(r) -m $(m)

qsub: $(EXEC)
	rm -rf res.*
	@qsub batch; 
	@sleep 3; 
	@cat res.*

%: %.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -lm

clean:
	rm -rf *.o $(EXEC) *~

