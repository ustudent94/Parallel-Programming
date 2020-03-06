
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
    srand(rank);
    int order;

//    MPI_Send(&rank,1,MPI_INT,(rank+1)%size,0,MCW);
//    MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

    if(rank!=0){
    /* i am a chef */
        while(1){
            /* meet the diners */
            sleep(rand()%5);
            /* give an order to the cook */
            MPI_Send(&order,1,MPI_INT,0,0,MCW);
            cout<<"chef "<<rank<<": puts in an order for the cook.\n";
        }
    }else{
    /*i am the cook */
        int orderCount=0;
        int flag;
        while(1){
            /* grab all the new orders */
            MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&flag,MPI_STATUS_IGNORE);
            while(flag){
                MPI_Recv(&order,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
                orderCount++;
                MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&flag,MPI_STATUS_IGNORE);
            }
            cout<<"Cook: received "<<orderCount<<" orders.\n";
            if(orderCount>30){
                cout<<"Cook: screw this, I am out of here.\n";
                break;
            }
            if(orderCount==0){
                cout<<"Cook: nothing to do, going for a smoke.\n";
                sleep(1);
            } else {
                while(orderCount>0){
                    cout<<"Cook: cooking an order...\n";
                    sleep(1);
                    orderCount--;
                }
            }
        }
    }

    cout<<"(the cook has left the building\n";
    


    MPI_Finalize();

    return 0;
}

