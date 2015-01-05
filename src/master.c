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
char * pwd_found = NULL;


int power(int a, int pow){
  int tmp = pow, res = 1;
  while (tmp-- > 0){
    res *= a;
  }
  return res;
}

char * next_word(char *word, int offset){
  int remain = offset, i = r-1, tmp, pow;
  char *res = malloc(sizeof(char)*(r+1));
  memset(res, 0, sizeof(char)*(r+1));
  while (remain != 0){
    pow = power(nb_letters-1, i);
    tmp  = remain / pow;
    if (tmp > 0){
      res[i] = word[i] + tmp;
    }
    --i;
    remain -= tmp*pow;
  }
  free(word);
  return res;
}

void thread_comm(MPI_Comm inter){
  int finishing = 0;
  while(1){
    struct task * task_to_deal_with;
    int flag;
    int k;
    char buffer;
    MPI_Status status_p;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, inter, &flag, &status_p);
    if (flag) {
      switch(status_p.MPI_TAG) {
	//Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
      case ASK :
	MPI_Recv(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, ASK, inter, MPI_STATUS_IGNORE);
	if (pwd_found != NULL){
	  // The process send END message, because we have the password
	  MPI_Send(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, END, inter);
	  ++finishing;
	} else if (todo_list.num_children == 0){     
	  // The process send FINISH message, because we have any task left
	  MPI_Send(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, FINISH, inter);
	} else {
	  task_to_deal_with = list_pop(&todo_list.children, struct task, list);
	  MPI_Send(task_to_deal_with, 1, task_type, status_p.MPI_SOURCE, INTER, inter);
	  free(task_to_deal_with);
	  --todo_list.num_children;
	}
	printf("Master received ASK from slave %d\n", status_p.MPI_SOURCE);
	break;
      case INTER :
	++finishing;
	task_to_deal_with = malloc(sizeof(struct task));
	MPI_Recv(task_to_deal_with, 1, task_type, status_p.MPI_SOURCE, INTER, inter, MPI_STATUS_IGNORE);
	MPI_Send(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, END, inter);
	pwd_found = malloc(sizeof(char)*(r+1));
	memcpy(pwd_found, task_to_deal_with->start_word, sizeof(char)*(r+1));
	free(task_to_deal_with);
	while(!list_empty(&(todo_list.children))){
	  task_to_deal_with = list_pop(&todo_list.children, struct task, list);
	  free(task_to_deal_with);
	  --todo_list.num_children;    
	}
	printf("Master received INTER from slave %d\n", status_p.MPI_SOURCE);
	break;
      case NOTHING :
	++finishing;
	MPI_Recv(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, NOTHING, inter, MPI_STATUS_IGNORE);
	printf("Master received NOTHING from slave %d\n", status_p.MPI_SOURCE);
	break;
      }
    } else if (finishing == p){
      return;
    }
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
    
  int pwd_length = strlen(m);
  for (i = 0; i < pwd_length; ++i) {
    if (occ[m[i]] == 0){
      fprintf(stderr, "Error: alphabet doesn't contain pwd's letters.\n");
      return 1;
    }
  }

  if ((nb_letters >= 256) || (r >= MAX_CHARS) || (pwd_length >= MAX_CHARS)){
      fprintf(stderr, "Error: out of range (> max #chars).\n");
      return 1;    
  }


  list_head_init(&todo_list.children);
  todo_list.num_children = 0;

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
  int nb_possibilites = power(nb_letters, r);
  int index = 0;
  char * start_word = malloc(sizeof(char)*(r+1));
  memset(start_word, 0, (r+1));
  start_word[0] = 1;
  int nb_task = (nb_possibilites + MAX_INTER - 1) / MAX_INTER;
  for (i = 0; i < nb_task; ++i){
    int k;
    printf("start word : ");
    for (k = 0; k < r ; k++)
      printf("%d ", (unsigned int) start_word[k]);
    printf("\n");
    struct task * task_to_add = malloc(sizeof(struct task));
    memcpy(task_to_add->start_word,start_word,sizeof(char)*(r+1));
    task_to_add->nb_test = (MAX_INTER < (nb_possibilites - index)) ? MAX_INTER : (nb_possibilites - index);
    index += MAX_INTER;
    list_add(&todo_list.children, &task_to_add->list);
    ++todo_list.num_children;
    start_word = next_word(start_word, MAX_INTER);
  }
  free(start_word);

  // comm thread
  printf("Interval generation ended, computation begin now.\n");
  thread_comm(inter);

  if (pwd_found != NULL) {
    printf("Pwd : %s\n", pwd_found);
    free(pwd_found);
  } else {
    printf("Pwd not found\n");
  }

  time = MPI_Wtime() - time;
  printf("Time : %lf (s)\n", time);
  MPI_Finalize();
  return 0;
}
