#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include <errno.h> 
#include <string.h> 

char *target = "thread";

void *modify(void *vargp) { 
  *(char**)vargp = target; /* add cast and dereference to modify what is pointed */
  printf("Thread vargp = %s\n", *(char**)vargp); /* add cast and dereference */
  return NULL; 
} 
   
int main() {
  char *pt = "main";
  pthread_t thread_id; 
  printf("Before Thread, pt = %s\n", pt); 
  pthread_create(&thread_id, NULL, modify, &pt); /* pass pointer to pt */
  pthread_join(thread_id, NULL); 
  printf("After Thread, pt = %s\n", pt); 
  
  exit(0); 
}