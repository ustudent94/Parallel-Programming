

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
    //cout << "Cube " << f <<  " of " << process << " is " << dest <<endl;
    return dest;
}

void allPrint(int data){
    int array[64];
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
    bool desc, zero;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    MPI_Request numRequest;

//    const int printSize = size;
//    int print[printSize];

    //Send out random numbers
    if(!rank){
        for(int i = 0; i < size; i++){
            data = rand()%100;
            cout << data << " ";
            MPI_Send(&data, 1, MPI_INT, i, 0, MCW);
        }
        cout << endl;
    }
    //recieve numbers
    MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
    myNum = data;

    //loop through bitonic sort
    for(int i= 0; i < sqrt(size); i++){
        mask = 1;
        mask <<= i + 1;
        desc = rank & mask;
        //cout << "Mask 1 " << mask << " " << rank << " " << i << " " << desc << endl;
        for(int j = i; j >= 0; j--){
            mask = 1;
            mask <<= j;
            //cout << "Mask 2 " << mask << " " << rank << " " << j << " " << zero << endl;
            zero = rank & mask;
            //cout << i << " " <<  j << " " <<zero << " " << desc << endl;

            MPI_Irecv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,&numRequest);
            recvRank = mystatus.MPI_TAG;

            //if the rank is smaller and the number recieved is smaller swap
            prevNum = myNum;
            process = cube(j,rank);
            //debug
            //if((rank == 4 || rank == 5) && i == 0 && j == 0){
                cout << i << " " <<  j << " " <<  rank << " " << process << " " << zero << " " << desc <<  endl;
            //}
            if(zero && !desc){
                myNum = data;
                //cout << (zero && !desc) << endl;
                //cout << zero << " " << desc << endl;
                cout << "Swap " << rank << " and " << process << " for i = " << i << " and cube = " << j << endl;
            }
//            if(!desc && ((rank < recvRank && data < myNum) || desc && (rank > recvRank && data > myNum))){
//                myNum = data;
//                cout << "Swap" << rank << " and " << process << " for i = " << i << " and cube = " << j << endl;
//            }


            data = myNum;
            //cout << "Before send " << i << " " << j << " " << data << " " << rank << endl;
            MPI_Send(&data, 1, MPI_INT, process, rank, MCW);
            MPI_Barrier(MCW);
            sleep(1);
            //cout << "Sort " << rank << " Cube " << j << endl;
            allPrint(myNum);
        }
    }
    data = myNum;

    MPI_Barrier(MCW);
    if(!rank) {
        cout << "Final Sort: " << endl;
    }
    allPrint(myNum);
//    MPI_Gather(&myNum,1,MPI_INT,print,1,MPI_INT,0,MCW);
//    if(!rank) {
//        for (int i = 0; i < size; i++) {
//            cout << print[i] << " ";
//        }
//        cout << endl;
//    }
    MPI_Finalize();
    return 0;
}

