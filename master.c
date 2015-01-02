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

  MPI_Comm inter;
  MPI_Status status;
  double initialTime = 0.0;
  int myrank, size, p = -1, t = -1, r = -1;
  char c;
  char *a = NULL, *m = NULL;
  struct timeval t1, t2;
  unsigned long time_difference = 0;
  /* extern char * optarg;  */
  /* extern int optind, opterr; */

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
  if (a == NULL || m == NULL || p < 1 || t < 1 || r < 1){
    fprintf(stderr, "Error in args, exiting now.\n");
    return 1;
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Comm_spawn("slave", argv+1, p, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter, MPI_ERRCODES_IGNORE);

  printf("Created %d slaves\n", p);

  // not usefull if (size == p) isn't tested
  MPI_Comm_size(inter, &size);

  //test
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
