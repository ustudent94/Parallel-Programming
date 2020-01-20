#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv){

    int rank, size;
    int data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size); 

    if(rank==0){
        data = rand()%100;
        cout<< "process 0 generated a "<<data<<endl;
        MPI_Send(&data,1,MPI_INT,(rank+1)%size,0,MCW);
        MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
        cout<< "process 0 received a "<<data<<endl;
    } else {
        MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
        cout<<"process "<<rank<<"received a "<<data<<"and sent it on.\n";
        MPI_Send(&data,1,MPI_INT,(rank+1)%size,0,MCW);
    }


    MPI_Finalize();

    return 0;
}

