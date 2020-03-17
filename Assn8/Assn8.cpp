#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

const int numPoints = 100;


struct Point {

    int x;
    int y;
    Point(int a, int b){
        x = a;
        y = b;
    }
};

//Point points[numPoints] = {
//    {0,0},
//    {0,10},
//    {10,10},
//    {10,0},
//    {2,2},
//    {2,5},
//    {5,5},
//    {5,2},
//};

Point points[numPoints] = {
        {179140, 750703},
        {78270, 737081},
        {577860, 689926},
        {628150, 597095},
        {954030, 510314},
        {837880, 811285},
        {410640, 846947},
        {287850, 600161},
        {270030, 494359},
        {559020, 199445},
        {353930, 542989},
        {515920, 497472},
        {648080, 470280},
        {594550, 968799},
        {386690, 907669},
        {93070, 395385},
        {93620, 313966},
        {426870, 39662},
        {437000, 139949},
        {789810, 488001},
        {749130, 575522},
        {481030, 286118},
        {670720, 392925},
        {273890, 892877},
        {138430, 562658},
        {85480, 465869},
        {775340, 220065},
        {862980, 312238},
        {155180, 263662},
        {274070, 74689},
        {333340, 456245},
        {822150, 399803},
        {158880, 612518},
        {815560, 707417},
        {678240, 709341},
        {394470, 679221},
        {631300, 846813},
        {528320, 824193},
        {666940, 845130},
        {298650, 816352},
        {243750, 745443},
        {220500, 654221},
        {338920, 381007},
        {313110, 201386},
        {856380, 564703},
        {549250, 565255},
        {537400, 604425},
        {502110, 435463},
        {498840, 590729},
        {482310, 571034},
        {416930, 765126},
        {418400, 638700},
        {374170, 695851},
        {412370, 570904},
        {301090, 737412},
        {235690, 782470},
        {475940, 439645},
        {268540, 609753},
        {130500, 712663},
        {81660, 732470},
        {64520, 711936},
        {264690, 529248},
        {90230, 612484},
        {38370, 610277},
        {15430, 579032},
        {138890, 482432},
        {264580, 421188},
        {86690, 394738},
        {209190, 347661},
        {425890, 376154},
        {312480, 177450},
        {373360, 142350},
        {442850, 106198},
        {505100, 189757},
        {542610, 224170},
        {566730, 262940},
        {615970, 237922},
        {612120, 303181},
        {634410, 320152},
        {879480, 239867},
        {868760, 286928},
        {807670, 334613},
        {943060, 368070},
        {827280, 387076},
        {896040, 413699},
        {920900, 454842},
        {746380, 440559},
        {734300, 452247},
        {730780, 471211},
        {870570, 549620},
        {607060, 453077},
        {926580, 669624},
        {812660, 614479},
        {701420, 559132},
        {688600, 580646},
        {743800, 669521},
        {819700, 857004},
        {683690, 682649},
        {732680, 857362},
        {685760, 866857},
};

void printPath(float dist,int path[numPoints]){
    cout << dist <<": ";
    for(int i = 0; i < numPoints; i++){
        cout << path[i] << " ";
    }
    cout <<endl;
}


float DBP(Point a, Point b){
    float x= b.x - a.x;
    float y = b.y - a.y;
    float dist = sqrt(x*x + y*y);
//    cout << x << " " << y << " " << x*x << " " << y*y << " " << dist << endl;
    return dist;
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

float fitness(int path[numPoints]){
    float distance = 0;
    for(int i = 0; i < numPoints-1; i++){
        Point first = points[path[i]];
        Point second = points[path[i+1]];
        float addDist = DBP(first,second);
//        cout << "a.x: " << first.x << " a.y: " << first.y << " b.x: " << second.x << " b.y: " << second.y << " dist: " << addDist << endl;
        distance += addDist;
//        cout << addDist << endl;

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
    srand(rank);

    int chance;
    int bestPath[numPoints];
    float bestDist;
    float bestTime;
    int curPath[numPoints];
    float curDist;
    int nextPath[numPoints];
    float nextDist;

    randPath(curPath);
    copyPath(bestPath,curPath);
    bestDist = fitness(curPath);

    curDist = bestDist;
//    printPath(bestDist,bestPath);
//    printPath(curDist,curPath);


    float maxTime = 1;
    float startTime = MPI_Wtime();
    float curTime = MPI_Wtime() - startTime;
    bestTime = curTime;
    float nextTime = 0;
    while(nextTime < maxTime){
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&flag,&mystatus);
        //if there is a message to recieve recieve in as next path otherwise generate new nextPath
        if(flag) {
            MPI_Recv(nextPath,numPoints,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
        }else {
            mutation = (rand() % (numPoints / 5)) == 0;

            PMX(curPath, nextPath, mutation);
            if (mutation) {
                //send if mutation occurs
                process = rand() % size;
                MPI_Send(nextPath, numPoints, MPI_INT, process, 0, MCW);

            }
        }

//        cout << curTime << " " << numPoints<< endl;
        chance = 100-((curTime/maxTime)*100)+1;
//        if(!rank) {
//            cout << rank << " " << chance << endl;
//        }
        nextDist = fitness(nextPath);

        if(mutation){
            cout << "A mutation has occurred. Sending to process "<< process << " from " << rank << endl;
            printPath(curDist,curPath);
            printPath(nextDist,nextPath);
        }

        if(curDist < bestDist){

            copyPath(bestPath, curPath);
            bestDist = curDist;
            bestTime = curTime;
        }
        chooseWorse = ((rand()%chance) ==0);
        if((!chooseWorse && nextDist < curDist) || (chooseWorse && curDist < nextDist)){
            copyPath(curPath,nextPath);
            curDist = nextDist;
            curTime = MPI_Wtime() - startTime;
        }
        //display chosen distance and path
//        printPath(curDist,curPath);
        nextTime = MPI_Wtime() - startTime;
    }

    if(rank != 0) {
        MPI_Send(bestPath, numPoints, MPI_INT, 0, 0, MCW);
        MPI_Send(&bestTime, 1, MPI_INT, 0, 0, MCW);
    }else {

        for (int i = 1; i < size; i++) {
            MPI_Recv(nextPath, numPoints, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &mystatus);
            source = mystatus.MPI_SOURCE;
            MPI_Recv(&nextTime, 1, MPI_INT, source, MPI_ANY_TAG, MCW, &mystatus);
            nextDist = fitness(nextPath);
            if (nextDist < bestDist) {
                copyPath(bestPath,nextPath);
                bestDist = nextDist;
                bestTime = nextTime;
            }
        }
        sleep(5);
        cout << endl << "The best path was found at " << bestTime << " seconds and is the following" << endl;
        printPath(bestDist, bestPath);
    }









    MPI_Finalize();
    return 0;
}
