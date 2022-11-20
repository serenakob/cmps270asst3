#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define threadnumb 8
#define length 100000000

int array[length];
int lower = 0, upper = 5;
pthread_t thread[threadnumb];
int counts[threadnumb]; //array that stores private countsp


/* 
    requires: empty array of integers
    effects: fills randomly the array with integers between 0 and 5 
*/
void arrayfill(int array[]){
    srand ( time(NULL) );
    for(int i = 0; i<length; i++){
        array[i]= (rand() % (upper - lower + 1)) + lower;
    }
}

/* 
    SEQUENTIAL CODE GIVEN
    requires: nothing
    effects: returns the total number of 1s in the array
*/
int count1s() { 
    int i;
    int count = 0;
    for (i=0; i<length; i++){
       if (array[i] == 1){
          count++;
       }
    }
    return count;
}

/* 
    PARALLEL CODE 
    requires: nothing
    effects: every thread works on a part of the array and counts the number of
             1s in the array. 
*/
void * count1(void * id){
    int count=0;
    int s= (intptr_t)id;
    long myID = (long)id;
    int partitionthreads = length/threadnumb;
    long start = myID * (long)partitionthreads;

    for(int i=start; i<start+partitionthreads; i++){
        if(array[i]==1){
            count++;
        }
    }
    pthread_exit(NULL);

    counts[myID] = count;

}

int main(){

    arrayfill(array);
    double AVGtimesequentially, AVGtimerace;
    int AVGcountrace, total;

    //sequential code and time taken
        int count = 0;
        struct timespec start, finish;
        double elapsed;
        clock_gettime(CLOCK_MONOTONIC, &start);
        int seq = count1s();
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
        AVGtimesequentially += elapsed;
        printf("Sequential version: Number of ones = %d            Time Spent = %f\n", seq, elapsed); 
    

    //parallel code and time taken
    for (int j = 0; j < 100; j++){
        count = 0;
        struct timespec start1, finish1;
        double elapsed1;
        clock_gettime(CLOCK_MONOTONIC, &start1);
        for (int i = 0; i < threadnumb; i++){
            pthread_create(&thread[i], NULL, count1, (void*)i);
        }
        for (int i = 0; i < threadnumb; i++){
            pthread_join(thread[i], NULL);
        } 

        for (int i= 0; i < threadnumb; i++) {//add up the private counts)
            total += counts [i];
        }
        clock_gettime(CLOCK_MONOTONIC, &finish1);
        int numOf1s = count;
        elapsed1 = (finish1.tv_sec - start1.tv_sec);
        elapsed1 += (finish1.tv_nsec - start1.tv_nsec) / 1000000000.0;    
        AVGcountrace += numOf1s;
        AVGtimerace += elapsed1;
        printf("Parallel version: Number of ones = %d         Time Spent = %f\n", numOf1s, elapsed1);
    }

    AVGtimesequentially = AVGtimesequentially/100;
    AVGtimerace = AVGtimerace/100;
    AVGcountrace = AVGcountrace/100;

    printf("average time it took for sequential version with array size %d and %d threads: %lf\n", length, threadnumb, AVGtimesequentially);
    printf("average time it took for race cond version with array size %d and %d threads: %lf\n", length, threadnumb, AVGtimerace);
    printf("average count for race cond version with array size %d and %d threads: %d\n", length, threadnumb, AVGcountrace);

}