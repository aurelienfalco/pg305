#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#define MO 1000000

int interval(int x, int n){

  
}

void comm(MPI_Comm inter)
{
  int buf = 158185;
  MPI_Send(&buf,1,MPI_INT,0,99,inter);
}

// main function calling mpi functions
int main(int argc, char **argv)
{

  int myrank, size, p = 0, c, provided, flag, claimed;
  int t, r, n;

  MPI_Comm inter;

  MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Comm_get_parent(&inter);
  MPI_Bcast(&t,1,MPI_INT,0,inter);
  int size_a;
  MPI_Bcast(&size_a,1,MPI_INT,0,inter);
  char* a = malloc(size_a * sizeof(char));
  MPI_Bcast(a,size_a,MPI_CHAR,0,inter);
  MPI_Bcast(&r,1,MPI_INT,0,inter);
  int size_m;
  MPI_Bcast(&size_m,1,MPI_INT,0,inter);
  char* m = malloc(size_m * sizeof(char));
  MPI_Bcast(m,size_m,MPI_CHAR,0,inter);
  
  printf("Read args: %d %s %d %s\n",t,a,r,m );
  omp_set_num_threads(t+1);
  printf("%d\n",flag );
  #pragma omp parallel
  {
    n = omp_get_thread_num();
    printf("%d\n", n);
    #pragma omp master
    {
      comm(inter);
    }
  }

  free(a);
  free(m);
  MPI_Finalize();
  return 0;
}
