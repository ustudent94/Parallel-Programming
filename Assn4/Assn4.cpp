#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

int cube(int f, int process){
    int dest;
    int mask=1;
    mask <<= f;
    dest = process ^ mask;
    return dest;
}

void allPrint(int data){
    int rank, size;

    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    const int printSize = size;
    int print[printSize];
    MPI_Gather(&data,1,MPI_INT,print,1,MPI_INT,0,MCW);
    if(!rank) {
        for (int i = 0; i < size; i++) {
            cout << print[i] << " ";
        }
        cout << endl;
    }
    return;
}

int main(int argc, char **argv){

    int rank,recvRank, size;
    int data, myNum, prevNum, process;
    int mask;
    bool descending, larger;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    MPI_Request numRequest;

    //Send out random numbers
    if(!rank){
        for(int i = 0; i < size; i++){
            data = rand()%100;
            MPI_Send(&data, 1, MPI_INT, i, 0, MCW);

        }
    }

    //recieve numbers
    MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
    myNum = data;
    allPrint(myNum);

    //loop through bitonic sort
    for(int i= 0; i < sqrt(size); i++){
        mask = 1;
        mask <<= i + 1;
        descending = rank & mask;
        for(int j = i; j >= 0; j--){
            mask = 1;
            mask <<= j;
            larger = rank & mask;
            process = cube(j,rank);


            data = myNum;
            MPI_Send(&data,1,MPI_INT,process,0,MCW);
            MPI_Recv(&data,1,MPI_INT,process,0,MCW,&mystatus);

            //swap if any of the four cases are true
            if((data < myNum && !larger && !descending) || (data > myNum && larger && !descending) || (data > myNum && !larger && descending) ||(data < myNum && larger && descending)){
                myNum = data;
            }

            MPI_Barrier(MCW);
            allPrint(myNum);
        }
    }
    MPI_Finalize();
    return 0;
}

