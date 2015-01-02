#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
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

  MPI_Comm inter;
  int myrank, size, provided;
  char c;
  char *a = NULL, *m = NULL;
  int p, t, r, n;

  while ((c = getopt (argc, argv, "p:t:a:r:m:")) != -1){
    switch (c) {
      case 'p':
      p = atoi(optarg);
      break;
      case 't':
      t = atoi(optarg);
      break;
      case 'a':
      a = optarg;
      break;
      case 'r':
      r = atoi(optarg);
      break;
      case 'm':
      m = optarg;
      break;
      default:
      abort();

    }
  }
  
  printf("Read args: %d %s %d %s\n",t,a,r,m );

  MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Comm_get_parent(&inter);
  
  omp_set_num_threads(t+1);
  #pragma omp parallel
  {
    n = omp_get_thread_num();
    printf("##%d #%d\n", myrank, n);
    #pragma omp master
    {
      comm(inter);
    }
  }

  MPI_Finalize();
  return 0;
}
