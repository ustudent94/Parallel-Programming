#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

const int numPoints = 8;


struct Point {

    int x;
    int y;
    Point(int a, int b){
        x = a;
        y = b;
    }
};

Point points[numPoints] = {
    {0,0},
    {0,10},
    {10,10},
    {10,0},
    {2,2},
    {2,5},
    {5,5},
    {5,2},
};

void printPath(double dist,int path[numPoints]){
    cout << dist <<": ";
    for(int i = 0; i < numPoints; i++){
        cout << path[i] << " ";
    }
    cout <<endl;
}


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
//    printPath(0,path);
}

double fitness(int path[numPoints]){
    double distance = 0;
    for(int i = 0; i < numPoints-1; i++){
        Point first = points[path[i]];
        Point second = points[path[i+1]];
        double addDist = DBP(first,second);
//        cout << "a.x: " << first.x << " a.y: " << first.y << " b.x: " << second.x << " b.y: " << second.y << " dist: " << addDist << endl;
        distance += addDist;

    }
    return  distance;
}

void SR(int path[numPoints]){
    int start = rand() % numPoints;
    int end = rand() % numPoints;
    int numSub = end -start;
    if(numSub <0){
        numSub = numPoints-1+numSub;
    }
    const int numReverse = numSub;
    cout << numReverse << endl;
    int reversal[numReverse];
    int curIndex = start;
    //get subsection
    for(int i = 0; i < numReverse; i++) {
        reversal[i] = path[curIndex];
        curIndex = (curIndex +1)%numPoints;
    }

    int frontInd = 0;
    int backInd = numReverse;
    //reverse subsection
    while(frontInd < backInd){
        int temp = reversal[frontInd];
        reversal[frontInd] = reversal[backInd];
        reversal[backInd] = temp;
        frontInd++;
        backInd--;
    }

    //reinsert subsection
    curIndex = start;
    for(int i = 0; i < numReverse; i++) {
        path[curIndex] = reversal[curIndex];
        curIndex = (curIndex +1)%numPoints;
    }

}

void PMX(int path[numPoints], int newPath[numPoints], bool mutation){
    randPath(newPath);

    int start = rand() % numPoints;
    int end = rand() % numPoints;
//    cout << start << " " << end << endl;
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

void copyPath(int paste[numPoints], int copy[numPoints]){
    for(int i = 0; i < numPoints; i++){
        paste[i] = copy[i];
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

    bool mutation = false;
    bool chooseWorse = false;

    int chance;
    int bestPath[numPoints];
    double bestDist;
    int curPath[numPoints];
    double curDist;
    int nextPath[numPoints];
    double nextDist;

    randPath(curPath);
//    bestPath = curPath;
//    printPath(0, curPath);
//    std::copy(std::begin(bestPath),std::end(bestPath),std::begin(curPath));
    copyPath(bestPath,curPath);
    bestDist = fitness(curPath);

    curDist = bestDist;
    printPath(bestDist,bestPath);
    printPath(curDist,curPath);

    int lessDist[numPoints];
    int moreDist[numPoints];

    double maxTime = 10;
    double startTime = MPI_Wtime();
    double curTime = MPI_Wtime() - startTime;
    while(curTime < maxTime){

        mutation = (rand() % (numPoints/5)) == 0;
        PMX(curPath,nextPath,mutation);

        chance = (curTime/numPoints) + 1;
        nextDist = fitness(nextPath);



        if(curDist < bestDist){
//            bestPath = curPath;
//            std::copy(begin(bestPath),end(bestPath),begin(curPath));
            copyPath(bestPath, curPath);
            bestDist = curDist;
        }
//        cout << chance << endl;
        chooseWorse = (rand()%chance) ==0;
        if((!chooseWorse && nextDist < curDist) || (chooseWorse && curDist < nextDist)){
//            curPath = nextPath;
//            std::copy(begin(curPath),end(curPath),begin(nextPath));
            copyPath(curPath,nextPath);
            curDist = nextDist;
        }
        //display chosen distance and path
        printPath(curDist,curPath);

        curTime = MPI_Wtime() - startTime;
    }

    cout << "The best path is the following" << endl;
    printPath(bestDist,bestPath);









    MPI_Finalize();
    return 0;
}
