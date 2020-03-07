#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv) {

    int rank, size, process,source, tag, flag;
    int tasks;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

//    MPI_Send(task,1,MPI_INT,process,0,MCW);
//    MPI_Recv(task,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);

    srand(rank);
    vector<int> queue;
    int taskNum = 0;
    //todo: change to random amount of work
    int maxTask = 100;
//    int maxTask = (rand() % 1024) + 1024
    bool processWhite = true;\
    bool sendToken = false;
    int token =0;
    //token 0 = no token, 1 = white, 2 = black
    int numIter;



//    taskNum = maxTask;
    ///kickoff
    if(!rank){
        //this is to initiate process only
        tasks = taskNum;
//        tasks = -2;
        cout << "Initializing work at process 0 starting with task " << taskNum << endl;
        MPI_Send(&tasks, 1, MPI_INT, 0, 0, MCW);
//        MPI_Send(&tasks, 1, MPI_INT, 1, 1, MCW);
        token = 0;
    }


    while(1){
//            MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&flag,&mystatus);
        if(flag) {
            //MPI_Get_count(&mystatus, MPI_INT, &count);
            source = mystatus.MPI_SOURCE;
            tag = mystatus.MPI_TAG;
        }

        if(tag == 1 && flag){
            MPI_Recv(&token,1,MPI_INT,MPI_ANY_SOURCE,1,MCW,&mystatus);
            cout << rank << " received token " << token << " from " << source << endl;
            ///termination logic
            if (token == -2 && rank == 0) {
                //send out termination message
                token = -1;
                for (int i = 1; i < size; i++) {
                    cout << "Terminating process " << i << endl;
                    MPI_Send(&token, 1, MPI_INT, i, 1, MCW);
                }
                break;
            }
            if(token == -3 && rank == 0){
                token = -2;
            }
            if (token == -1 && rank != 0) {
                cout << rank << " terminated" << endl;
                break;
            }
        }

        ///work logic

        //recieve message if message waiting
        if(tag == 0 && flag) {
            MPI_Recv(&tasks, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);
//            cout << rank << " received " << tasks << " task from " << source << endl;
            queue.push_back(tasks);
        }


        //send off excess work
        if(queue.size() >16){

            for(int i = 0; i < 2; i++){
                process = rand() %size;
                tasks = queue.back();
                queue.pop_back();

                if(process < rank){
                    processWhite = false;
                }
//                cout << rank << " has " << queue.size() << " elements in queue. Sending to process " <<process << endl;
                MPI_Send(&tasks, 1, MPI_INT, process, 0, MCW);
            }
        }

        //do work
        if(queue.size() > 0){
            int task = queue.back();
            queue.pop_back();
            numIter = task * task;
            for(int i = 0;  i < numIter; i++){
                task++;
            }
        }

        //add new tasks
        int numNew = (rand()%2) +1;
        int counter = 0;
        while (taskNum < maxTask && counter < numNew){
            taskNum++;
//            cout << rank << " added " << taskNum << " to the queue " << endl;
            queue.push_back(taskNum);
            counter++;
            if(taskNum>= maxTask){
                cout << rank << " is done generating tasks " << endl;
            }
            if(taskNum>= maxTask && rank == 0){
                cout << rank << " sending token around" << endl;
                token = -2;
            }
        }

        ///send token
        if(queue.size() == 0 && taskNum >= maxTask && token!=0){
            process = (rank +1) %size;

            if(!processWhite){
                token = -3;
                processWhite = true;
            }
            cout << rank << " sending token " << token << " to " << process << endl;
            MPI_Send(&token, 1, MPI_INT, process, 1, MCW);
            token = 0;
        }
    }




    MPI_Finalize();
    return 0;
}