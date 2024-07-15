#include <stdio.h>
#include <pthread.h>
#define NUM_THREADS 4
void *ProcessFunc(void *pArg){
    int *p = (int *)pArg;
    int myNum = *p;
    printf("Thread number %d\n", myNum);
}

int main(){
    pthread_t tid[NUM_THREADS];
    int threadNum[NUM_THREADS];
    int i =0;
    printf("Main process\n");
    for(i =0; i < NUM_THREADS;i++){
        pthread_create(&tid[i], 0, ProcessFunc, &i);
    }
    
    for (i =0; i <NUM_THREADS;i++){
        pthread_join(tid[i], NULL);
    }

    return 0;
}