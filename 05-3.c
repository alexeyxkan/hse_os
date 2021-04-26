#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

/*int fun1(int semid, struct sembuf *buf) {
	buf->sem_op = -1;
    buf->sem_flg = 0;
    buf->sem_num = 0;
    return semop(semid, buf, 1);
}

int fun2(int semid, struct sembuf *buf) {
	buf->sem_op = 1;
    buf->sem_flg = 0;
    buf->sem_num = 0;
    return semop(semid, buf, 1);
}

void re(char* string, size_t size) {
	for (int i = 0; i < size / 2; i++) {
        char t = string[i];
        string[i] = string[size - i - 1];
        string[size - i - 1] = t;
    }
}*/

int main() {
  struct sembuf mybuf;
  int semid;
  int     fd[2], result;
  int count;

  size_t size;
  char  resstring[14];
  
  char pathname[] = "05-3.c";
  key_t key; // IPC key

  scanf("%d", &count);
  
  if (pipe(fd) < 0) {
    printf("Can\'t open pipe\n");
    exit(-1);
  }
  
  if ((key = ftok(pathname, 0)) < 0) {
	  printf("Cant generate key\n");
	  exit(-1);
  }
  
  if ((semid = semget(key, 1, 0666)) < 0) {
	  printf("Sem not found. Trying create\n");
	  
	  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		  printf("Cant get sem id\n");
		  exit(-1);
	  }
	  
	  printf("Create successful\n");
  }
  
  mybuf.sem_num = 0;
  mybuf.sem_op = 1;
  mybuf.sem_flg = 0;
  
  if (semop(semid, &mybuf, 1) < 0) {
      printf("Cant set sem value = 1\n");
      exit(-1);
  }
  
  result = fork();

  if (result < 0) {
    printf("Can\'t fork child\n");
    exit(-1);
  } 
  else if (result > 0) {
	//parent
	mybuf.sem_num = 0;
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    if (semop(semid, &mybuf, 1) < 0) {
      printf("Cant drop sem value = 1\n");
      exit(-1);
    }
	
	for (int i = 0; i < count; i++) {
	  size = write(fd[1], "Hello, world!", 14);
	  if (size != 14) {
        printf("Can\'t write all string to pipe\n");
        exit(-1);
      }
	  else printf("parent wrote msg\n");
	  
	  mybuf.sem_num = 0;
      mybuf.sem_op = 2;
      mybuf.sem_flg = 0;
	  
	  if (semop(semid, &mybuf, 1) < 0) {
        printf("Critical section error!\n");
        exit(-1);
      }
	  
	  mybuf.sem_num = 0;
      mybuf.sem_op = 0;
      mybuf.sem_flg = 0;
	  
	  if (semop(semid, &mybuf, 1) < 0) {
        printf("Critical section error!\n");
        exit(-1);
      }
	  
	  mybuf.sem_num = 0;
      mybuf.sem_op = -1;
      mybuf.sem_flg = 0;
      if (semop(semid, &mybuf, 1) < 0) {
        printf("Can\'t enter the critical section properly in program A\n");
        exit(-1);
      }
	  
	  size = read(fd[0], resstring, 14);
	  
	  if (size < 0) {
        printf("Cant read string\n");
        exit(-1);
	  }
	
	  printf("parent read: %s\n, resstring");
	}

    printf("Parent exit\n");
  } 
  else {
	// child
	
	for (int i = 0; i < count; i++) {
	  mybuf.sem_num = 0;
      mybuf.sem_op = -2;
      mybuf.sem_flg = 0;
      if (semop(semid, &mybuf, 1) < 0) {
        printf("Critical section error!\n");
        exit(-1);
      }
	  
	  size = read(fd[0], resstring, 14);
	  
	  if (size < 0) {
        printf("Can\'t read string from pipe\n");
        exit(-1);
      }
	  
	  printf("Child exit, resstring:%s\n", resstring);
	  
	  size = write(fd[1], "Hello, world!", 14);
	  
	  if (size != 14) {
        printf("Cant write all string\n");
        exit(-1);
	  } 
	  else printf("child wrote msg\n");
	  
	  mybuf.sem_num = 0;
      mybuf.sem_op = 1;
      mybuf.sem_flg = 0;
	  
	  if (semop(semid, &mybuf, 1) < 0) {
        printf("Critical section error!\n");
        exit(-1);
      }
	}
	
	close(fd[1]);
    close(fd[0]);
    printf("Child exit\n");
  }

  return 0;
}
