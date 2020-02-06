

#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

void bubbleSort(int *arr, int size){
    bool sorted = false;
    int temp;
    while(!sorted){
        sorted = true;
        for(int i = 0; i< size- 1; i++){
            if(arr[i+1] < arr[i]){
                sorted = false;
                //cout << "swap " << arr[i] << " and " << arr[i+1] << endl;
                temp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = temp;
            }
            //arr[i] = arr[0];
        }
    }
    //return arr;
}

int main(int argc, char **argv){

    int rank, size;
    int data;

    MPI_Init(&argc, &argv);
    MPI_Status mystatus;
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    //number of elements for each process to take on
    const int numSplit = 4;
    int split[numSplit];

    const int numData = size*numSplit - numSplit;
    int bigData[numData];

    //int sortOpt[size];
    if(!rank){
        cout << "The unsorted list: " << endl;
        for(int i=0;i<numData/numSplit;++i){
            for(int j = 0; j < numSplit; j++) {
                split[j] = rand()%100;
                cout << split[j] << " ";
            }
            MPI_Send(split, numSplit, MPI_INT, i + 1, 0, MCW);
        }
        cout << endl;
        //request data
        sleep(1);
        //MPI_Barrier(MCW);

        //get initial list to merge
        int tempArr[size-1];
        for(int i = 0; i < size-1; i++) {
            data = -1;

            MPI_Send(&data, 1, MPI_INT, i+1, 0, MCW);
            MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
            tempArr[i]= data;

        }
        int min = tempArr[0];
        int minProcess;
        int minIndex;
        bool assigned = false;
        for(int i = 0; i < numData; i++) {

            minProcess = 1;
            assigned = false;
            for (int j = 0; j < size - 1; j++) {
                if(tempArr[j] != -1 && (tempArr[j] < min || !assigned)){
                    min = tempArr[j];
                    minProcess = j+1;
                    minIndex = j;
                    if(!assigned){
                        assigned = true;
                    }
                }
            }
            bigData[i] = tempArr[minIndex];
            cout << "The " << i << " element of " << bigData[i] <<" was added to the sorted list" << endl;

            data = -1;
            MPI_Send(&data, 1, MPI_INT, minProcess, 0, MCW);
            MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
            tempArr[minIndex] = data;
            //sleep(1);
        }

        sleep(1);
        cout << "The sorted list:" << endl;
        for(int i = 0; i < numData; i++){
            cout << bigData[i] << " ";
        }
        cout << endl;


    }
    else {
        MPI_Recv(split,numSplit,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
        sleep(1);
        cout << "rank " << rank;
        for(int i = 0; i < numSplit; i++){
           cout << " " << split[i];
        }
        cout << endl;


        //sort group and send back
        bubbleSort(split,numSplit);
        //qsort(split,split+numSplit);
        //MPI_Barrier(MCW);

        for(int i = 0; i < numSplit; i++){
            //anything can be recieved this is just meant to wait for process zero to ask for the next sorted number
            MPI_Recv(&data,numSplit,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
            data = split[i];
            MPI_Send(&data, 1, MPI_INT, 0, 0, MCW);
        }
        MPI_Recv(&data,numSplit,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
        data = -1;
        MPI_Send(&data, 1, MPI_INT, 0, 0, MCW);
    }




    MPI_Finalize();

    return 0;
}



