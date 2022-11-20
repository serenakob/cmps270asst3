#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>                                       

#define threadnumb 1
#define length 1000000000
int array[length];
int count;
int lower = 0, upper = 5;

pthread_mutex_t m;
pthread_t thread[threadnumb];
void fillArray();
void* count1s_mutex(void * threadid);
int count1s_thread_mutex(int id);

int main(){
    fillArray(array);
    double averageTimeMutex;
    
    for(int i =0; i < 100; i++){
        struct timespec begin, end;
        double time_spent;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        int nbOnes = count1s_thread_mutex(i);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_spent = (end.tv_sec - begin.tv_sec);
        time_spent += (end.tv_nsec - begin.tv_nsec)/ 1000000000.0;
        averageTimeMutex += time_spent;
        printf("mutex version: Number of one = %d\nThread Version 1: Time Spent = %f\n", nbOnes, time_spent);
    }

    averageTimeMutex = averageTimeMutex/100;
    printf("AVG time it took for mutex version with array size %d and %d threads: %lf\n", length, threadnumb, averageTimeMutex);

   return 0;
}

/*
    requires: empty array
    effects: fills randomly the array with integers between 0 and 5 
*/
void fillArray(int array[]){
    srand ( time(NULL) );
    for(int i = 0; i<length; i++){
        array[i]= (rand() % (upper - lower + 1)) + lower;
    }
}
/*
    requires: nothing
    effects: counts the number of ones in the threads using mutex locks
*/
void* count1s_mutex(void * id){
    int i = (intptr_t) id;
    long tid = (long) id;
    int length_per_thread = length / threadnumb;   // deviding the array into threads. 
    long start = tid * (long) length_per_thread;
    for(i = start; i < threadnumb; i ++){
        if(array[i] == 1){
            pthread_mutex_lock(&m);
            count++;
            pthread_mutex_unlock(&m);
        }
    }
    pthread_exit(NULL);
}
/*
    requires: number of threads as parameters and returns the count (number of ones in the array).  
    effects : creates the thread 
*/ 
int count1s_thread_mutex(int id){
    
    for(int i = 0; i < threadnumb; i++){
        pthread_create(&thread[i], NULL, count1s_mutex, (void*)i);
    }
    for(int i = 0; i < threadnumb; i++){
        pthread_join(thread[i], NULL);
    }

    return count;
 }
