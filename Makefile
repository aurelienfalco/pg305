CC = mpicc
EX = mpiexec
EXEC = master slave
# SRC = master.c slave.c
# OBJ=$(SRC:.c=.o)
# effet Bonaldi ?
CFLAGS = -fopenmp -O3 -Iincludes -Wall -std=c99 #c99 for unsigned long long ....
stat=diff
n=1
p=4
t=5
a=abcdefghijklmnopqrstuvwxyz
r=5
m=ajoai

all: $(EXEC)

exec: $(EXEC)
	$(EX) -np $(n) master -p $(p) -t $(t) -a $(a) -r $(r) -m $(m)

qsub: $(EXEC)
	rm -rf res.*
	@qsub batch; 
	@sleep 3; 
	@cat res.*

%: src/%.o
	$(CC) $(CFLAGS) $^ -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf src/*.o $(EXEC)

