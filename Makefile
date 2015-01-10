CC = mpicc
EX = mpiexec
EXEC = master slave
# SRC = master.c slave.c
# OBJ=$(SRC:.c=.o)
# effet Bonaldi ?
CFLAGS = -fopenmp -O3 -Iincludes -Wall -std=c99 #c99 for unsigned long long ... and inlining ! (gain = ~9 for r=7 m=zzzzzzz)
stat=diff
n=1
p=4
t=5
a=abcdefghijklmnopqrstuvwxyz
r=6
m=passwd

all: $(EXEC)

exec: $(EXEC)
	$(EX) -np $(n) master -p $(p) -t $(t) -a $(a) -r $(r) -m $(m)

qsub: $(EXEC)
	rm -rf res.*
	@qsub batch;

%: src/%.o
	$(CC) $(CFLAGS) $^ -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf src/*.o $(EXEC)

