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

#include "common_types.h"

MPI_Datatype task_type;
struct task_list todo_list;
char nb_letters = 1;
int p = -1, t = -1, r = -1;


inline int power(int a, int pow){
  int res = 1;
  while (pow-- != 0){
    res *= a;
  }
}

inline char *next_word(char *word, int offset){
  int remain = offset, i = r-1, tmp, pow;
  char *res = malloc(sizeof(char)*(r+1));
  memset(res, 0, r+1);
  while (remain != 0){
    pow = power(nb_letters-1, i);
    tmp  = remain / pow;
    if (tmp > 0){
      res[r] = word[r] + tmp;
    }
    --r;
    remain -= tmp*pow;
  }
}


// main function calling mpi functions
int main(int argc, char **argv){

  MPI_Comm inter;
  MPI_Status status;
  double time;
  int size, i;
  char c;
  char *a = NULL, *m = NULL;

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
      return 1;

    }
  }
  // Just in case
  if (a == NULL || m == NULL || p < 1 || t < 1 || r < 1){
    fprintf(stderr, "Error in args, exiting now.\n");
    return 1;
  }

  // determine if alphabet contain pwd's letters (and only once)
  int alphabet_length = strlen(a);
  char occ[256];
  memset(occ, 0, 256);
  for (i = 0; i < alphabet_length; ++i)
    ++occ[a[i]];
  
  for (i = 0; i < 256; ++i){
    if (occ[i] > 1){
      occ[i] = 1;
      fprintf(stderr, "Warning: alphabet contains some letters twice or more.\n");
    }
    if (occ[i] == 1)
      ++nb_letters;
  }
    
  if (alphabet_length > MAX_CHARS){
      fprintf(stderr, "Error: alphabet is out of range (> max #chars).\n");
      return 1;    
  }

  int pwd_length = strlen(m);
  for (i = 0; i < pwd_length; ++i) {
    if (occ[m[i]] == 0){
      fprintf(stderr, "Error: alphabet doesn't contain pwd's letters.\n");
      return 1;
    }
  }


  MPI_Init(&argc, &argv);
  time = MPI_Wtime();
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size > 1){
    printf("You created several *master* (not allowed), exiting now.\n");
    MPI_Finalize();
    return 1;
  }
  MPI_Comm_spawn("slave", argv+1, p, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter, MPI_ERRCODES_IGNORE);

  printf("Created %d slaves\n", p);

  int a_of_b[2] = {1,(1+r)};
  MPI_Aint a_of_d[2];
  MPI_Datatype a_of_t[2] = {MPI_INT,MPI_CHAR};
  MPI_Aint i1, i2;
  struct task useless_task;
  MPI_Get_address(&useless_task, &i1);
  MPI_Get_address(&useless_task.nb_test, &i2); a_of_d[0] = i2-i1 ;
  MPI_Get_address(&useless_task.start_word, &i2); a_of_d[1] = i2-i1 ;
  MPI_Type_create_struct(2, a_of_b, a_of_d, a_of_t,&task_type);
  MPI_Type_commit(&task_type); // this type can represent an incoming task (besides start word) or the pwd found


  // create all task
  // call to the comm thread
  
  time = MPI_Wtime() - time;
  printf("Time : %lf (s)\n", time);
  MPI_Finalize();
  return 0;
}
