#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

const int numPoints = 10;

struct Point {
    double x;
    double y;
};

double DBP(Point a, Point b){
    return sqrt((b.x - a.x)*(b.x - a.x)+(b.y - a.y)*(b.y - a.y));
}

void randPath(int path[numPoints]){
    int swapPlace[numPoints];
    for(int i = 0; i < numPoints; i++){
        path[i] = i;
        swapPlace[i] = rand() % numPoints;
    }
    for(int i = 0; i < numPoints; i++) {
        int swapIndex = swapPlace[i];
        int temp = path[swapIndex];
        path[swapIndex] = path[i];
        path[i] = temp;
    }

}

void PMX(int path[numPoints], int newPath[numPoints]){
    randPath(newPath);
    int start = rand() % numPoints;
    int end = rand() % numPoints;
    cout << start << " " << end << endl;
    int curIndex = start;
    int curNum;
    int swapIndex;
    bool swapFound;

    //find swap
    while(curIndex != end){
        swapIndex = 0;
        swapFound = false;
        curNum = path[curIndex];
        //find swap
        while (!swapFound){
            if(newPath[swapIndex] == curNum ){
                swapFound = true;
            }else{
                swapIndex++;
            }
        }

        //swap numbers
        int temp = newPath[curIndex];
        newPath[curIndex] = newPath[swapIndex];
        newPath[swapIndex]= temp;

        //increment current index
        curIndex = (curIndex +1)%numPoints;
    }
}


int main(int argc, char **argv) {

    int rank, size, process, source, tag, flag;
    int task;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

//    MPI_Send(task,1,MPI_INT,process,0,MCW);
//    MPI_Recv(task,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);

    int path[numPoints];
    randPath(path);
    for(int i = 0; i < numPoints;i++){
        cout << path[i] << " ";
    }
    cout << endl;

    int newPath[numPoints];
    PMX(path,newPath);
    for(int i = 0; i < numPoints;i++){
        cout << newPath[i] << " ";
    }
    cout << endl;










    MPI_Finalize();
    return 0;
}
