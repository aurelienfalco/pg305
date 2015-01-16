CC = mpicc
EX = mpiexec
EXEC = master slave
CFLAGS = -fopenmp -O3 -Iincludes -Wall -std=c99 #c99 for unsigned long long ... and inlining ! (gain = ~9 for r=7 m=zzzzzzz)
stat=diff
n=1
p=4
t=5
a=abcdefghijklmnopqrstuvwxyz
r=6
m=passwd
c=./slave


all: $(EXEC)

exec: $(EXEC)
	$(EX) -np $(n) master -p $(p) -t $(t) -a $(a) -r $(r) -m $(m) -c $(c)

qsub: $(EXEC)
	rm -rf pg305-fh.*
	module load compiler/gcc mpi/openmpi/current
	echo "#PBS -l nodes=$(n):ppn=$(p)" > tmp.pbs
	echo "module load compiler/gcc mpi/openmpi/current" >> tmp.pbs
	echo "mpiexec -np $(n) ./pg305-fh/master -p $(p) -t $(t) -a $(a) -r $(r) -m $(m) -c $(c)" >> run.pbs
	@qsub -N pg305-hf run.pbs;

$(EXEC):%: src/%.o
	$(CC) $(CFLAGS) $^ -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

plot: graph_procs.dat
	gnuplot dataGraph.gp
	eog graph_procs.png&

clean:
	rm -rf src/*.o $(EXEC)

clean-all: clean
	cd rapport;	make clean


