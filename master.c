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

#define MO 1000000

// main function calling mpi functions
int main(int argc, char **argv)
{
  int myrank, size, p = -1, c, t = -1, r = -1;
  MPI_Comm inter;
  MPI_Status status;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double initialTime = 0.0;
  char *a = NULL, *m = NULL;
  struct timeval t1, t2;
  unsigned long time_difference = 0;
  extern char * optarg; 
  extern int optind, opterr;

  printf("Welcome to the World of the Mighty Password\n");

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
  // Just in case
  if (size != 1 || p == -1 || t == -1 || r == -1){
    fprintf(stderr, "Err: negative arguments\n");
    MPI_Finalize();
    return 1;
  }
  MPI_Comm_spawn("slave", MPI_ARGV_NULL, p-1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter, MPI_ERRCODES_IGNORE);

  printf("Created %d slaves\n", p-1);

  // Broadcast spawn parameters
  MPI_Bcast(&t,1,MPI_INT,MPI_ROOT,inter);
  int size_a = strlen(a) + 1;
  MPI_Bcast(&size_a,1,MPI_INT,MPI_ROOT,inter);
  MPI_Bcast(a,size_a,MPI_CHAR,MPI_ROOT,inter);
  MPI_Bcast(&r,1,MPI_INT,MPI_ROOT,inter);
  int size_m = strlen(m) + 1;
  MPI_Bcast(&size_m,1,MPI_INT,MPI_ROOT,inter);
  MPI_Bcast(m,size_m,MPI_CHAR,MPI_ROOT,inter);

  MPI_Comm_size(inter, &size);
  int buf;
  MPI_Recv(&buf,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,inter,&status);
  printf("%d\n",buf );

  // if (myrank == 0)
  //   initialTime = MPI_Wtime();
  // gettimeofday(&t1, NULL);
  // gettimeofday(&t2, NULL);
  // time_difference = (t2.tv_sec-t1.tv_sec)*MO +(t2.tv_usec-t1.tv_usec);
  // printf("%ld\n",time_difference );

  // if (myrank == 0)
  //   fprintf(stdout, "%lf\n", MO*(MPI_Wtime() - initialTime));

  MPI_Finalize();
  return 0;
}
