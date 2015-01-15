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
unsigned long long int * powe = NULL;
char * pwd_found = NULL;

#define CHK_ERR(boolean, ...) do{ \
if ((boolean)) {  \
	fprintf(stderr,__VA_ARGS__); \
	MPI_Finalize(); \
	return 1; \
}}while(0)

unsigned long long int power(int a, int pow){
	unsigned long long int tmp = pow, res = 1;
	while (tmp-- > 0){
		res *= a;
	}
	return res;
}

void next_word(char *word, unsigned long long int offset){
	unsigned long long int remain = offset, tmp; 
	int j, rem, k, size = 0;
	while (word[size] != 0)
		++size;
	j = size - 1;
	while (remain != 0 && j < r){
		while (j >= 0){
			k = 0;
			tmp = 0;
			while (k < (nb_letters - word[j] - 1)){
				if (remain >= (tmp + powe[j])){
					tmp += powe[j];
				} else
				break;
				++k;
			}
			word[j] += k;
			remain -= tmp;	
			--j;
			if (remain == 0)
				return;
		}
    /* int k; */
    /* printf("->  "); */
    /* for (k = 0; k <= r; k++){ */
    /*   printf("%d ", (int) word[k]); */
    /* } */
    /* printf(" sous init ...\n"); */
		rem = 0;
		while (j < (r - 1)){
			++j;
			if (word[j] == (nb_letters - 1)){
				word[j] = 1;
				rem = 1;
			} else if (rem == 1) {
				++word[j];
				break;
			} else {
				break;
			}
		}
		--remain;
	}
	return;
}

void thread_comm(MPI_Comm inter){
	int finishing = 0;
	while(1){
		struct task * task_to_deal_with;
		int flag;
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
	  // printf("finishing++ pwd\n");
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
	// printf("Master received ASK from slave %d\n", status_p.MPI_SOURCE);
				break;
				case INTER :
	// printf("finishing++ inter\n");
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
	// printf("Master received INTER from slave %d\n", status_p.MPI_SOURCE);
				break;
				case NOTHING :
				++finishing;
	// printf("finishing++ not\n");
				MPI_Recv(&buffer, 1, MPI_CHAR, status_p.MPI_SOURCE, NOTHING, inter, MPI_STATUS_IGNORE);
	// printf("Master received NOTHING from slave %d\n", status_p.MPI_SOURCE);
				break;
			}
		} else if (finishing == p){
      // printf("Finishing : %d, p : %d\n", finishing, p);
			return;
		}
	}
}

// main function calling mpi functions
int main(int argc, char **argv){

	MPI_Comm inter;
	double time;
	int size, i;
	char c;
	char *a = NULL, *m = NULL;
	char *path = NULL;

	printf("Welcome to the World of the Mighty Password\n");

	while ((c = getopt (argc, argv, "p:t:a:r:m:c:")) != -1){
		switch (c) {
			case 'p':
			p = atoi(optarg) - 1;
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
			case 'c':
			path = optarg;
			break;
			default:
			return 1;
		}
	}

	MPI_Init(&argc, &argv);

  // Just in case
	CHK_ERR(a == NULL || m == NULL || p < 1 || t < 1 || r < 1,"Error in args, exiting now.");

  // determine if alphabet contain pwd's letters (and only once)
	int alphabet_length = strlen(a);
	char occ[256];
	memset(occ, 0, 256);
	for (i = 0; i < alphabet_length; ++i)
		++occ[(int)a[i]];

	for (i = 0; i < 256; ++i){
		CHK_ERR(occ[i] > 1,"Error: alphabet contains some letters twice or more (For example, '%c' is %d times in alphabet).\n",i,occ[i]);
		if (occ[i] == 1)
			++nb_letters;
	}

	int pwd_length = strlen(m);
	for (i = 0; i < pwd_length; ++i) {
		CHK_ERR(occ[(int)m[i]] == 0,"Error: alphabet doesn't contain pwd's letters (For example: '%c').\n",m[i]);
	}

	CHK_ERR((nb_letters >= 256) || (r >= MAX_CHARS) || (pwd_length >= MAX_CHARS) ,
		"Error: out of range (> max_chars : %d).\n", MAX_CHARS );


	list_head_init(&todo_list.children);
	todo_list.num_children = 0;

	time = MPI_Wtime();
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	CHK_ERR(size > 1,"You created several (%d) *master* (not allowed), exiting now.", size);

	if (path == NULL)
		MPI_Comm_spawn("slave", argv+1, p, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter, MPI_ERRCODES_IGNORE);
	else
		MPI_Comm_spawn(path, argv+1, p, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter, MPI_ERRCODES_IGNORE);


	printf("Created %d slaves\n", p);


	int a_of_b[2] = {1,(1+r)};
	MPI_Aint a_of_d[2];
	MPI_Datatype a_of_t[2] = {MPI_UNSIGNED_LONG_LONG,MPI_CHAR};
	MPI_Aint i1, i2;
	struct task useless_task;
	MPI_Get_address(&useless_task, &i1);
	MPI_Get_address(&useless_task.nb_test, &i2); a_of_d[0] = i2-i1 ;
	MPI_Get_address(&useless_task.start_word, &i2); a_of_d[1] = i2-i1 ;
	MPI_Type_create_struct(2, a_of_b, a_of_d, a_of_t,&task_type);
  MPI_Type_commit(&task_type); // this type can represent an incoming task (besides start word) or the pwd found

  // create all task
  powe = malloc(sizeof(unsigned long long int)*(r+1));
  for (i = 0; i <= r; ++i){
  	powe[i] = power(nb_letters - 1, i);
    //printf("pow(%d) : %d\n", i, powe[i]);
  }
  unsigned long long int nb_possibilites = 0;
  for (i = 1; i<=r; ++i){
  	nb_possibilites += powe[i];
  }
  unsigned long long int index = 0;
  char * start_word = malloc(sizeof(char)*(r+1));
  memset(start_word, 0, sizeof(char)*(r+1));
  start_word[0] = 1;
  unsigned long long int nb_task = (nb_possibilites + MAX_INTER - 1) / MAX_INTER;
  printf("%lld intervals of size %d to be computed.\n", nb_task, MAX_INTER);
  unsigned long long j;
  for (j = 0; j < nb_task; ++j){
  	struct task * task_to_add = malloc(sizeof(struct task));
  	memcpy(task_to_add->start_word, start_word,sizeof(char)*(r+1));
  	task_to_add->nb_test = (MAX_INTER < (nb_possibilites - index)) ? MAX_INTER : (nb_possibilites - index);
  	index += MAX_INTER;
  	list_add_tail(&todo_list.children, &task_to_add->list);
  	++todo_list.num_children;
    /* int k; */
    /* for (k = 0; k <= r; k++){ */
    /*   printf("%d ", (int) start_word[k]); */
    /* } */
    /* printf("init ...\n"); */
  	next_word(start_word, MAX_INTER);
  }
  free(start_word);
  free(powe);

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
