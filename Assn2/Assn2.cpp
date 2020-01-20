#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv){
    int rank, size;
    int countdown = rand()%50;
    int initCount = countdown;
    int process;//= rand()%size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size);
    srand(countdown*rank);
    bool start = true;


    if(rank == 0 && start) {
        cout << "Process " << rank << " started the bomb with " << countdown << " time left" << endl;
        start = false;
        countdown--;
        sleep(1);
        process = rand() % size;
        while (process == rank) {
            process = rand() % size;
        }
        MPI_Send(&countdown,1,MPI_INT,process,0,MCW);
    }
    while (1) {
        MPI_Recv(&countdown, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        srand(countdown*rank);

        if(countdown >0){
            cout << "Process " << rank << " recieved the bomb with " << countdown << " time left" << endl;
            countdown--;
            sleep(1);
            process = rand() % size;
            while (process == rank) {
                process = rand() % size;
            }
            //cout << "Process " << rank << " is sending the bomb to process " << process << endl;
            MPI_Send(&countdown,1,MPI_INT,process,0,MCW);
        }else if(countdown == 0){
            cout << "Process " << rank << " recieved the bomb with " << countdown << " time left" << endl;
            cout << "The bomb has exploded. Process " << rank << " has lost" << endl;
            countdown = -1;
            //MPI_Bcast(&countdown,1,MPI_INT,0,MCW);
            for(int i = 0; i < size; i++){
                MPI_Send(&countdown,1,MPI_INT,i,0,MCW);
            }
            break;
        } else if(countdown <0) {
            break;
        }else{
            process = rand()%size;
            MPI_Send(&countdown,1,MPI_INT,process,0,MCW);
        }
    }


    MPI_Finalize();

    return 0;
}

