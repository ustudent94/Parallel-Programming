#include <iostream>
#include <fstream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>

#define MCW MPI_COMM_WORLD
using namespace std;

struct Complex {
    double r;
    double i;
};

Complex operator + (Complex s, Complex t){
    Complex v;
    v.r = s.r + t.r;
    v.i = s.i + t.i;
    return v;
}

Complex operator * (Complex s, Complex t){
    Complex v;
    v.r = s.r*t.r - s.i*t.i;
    v.i = s.r*t.i + s.i*t.r;
    return v;
}

int rcolor(int iters){
    if(iters == 255) return 0;
    return 32*(iters%8);
}

int gcolor(int iters){
    if(iters == 255) return 0;
    return 32*(iters%8);
}

int bcolor(int iters){
    if(iters == 255) return 0;
    return 32*(iters%8);
}

int mbrotIters(Complex c, int maxIters){
// calculate the number of iters for a pixel

    int i=0;
    Complex z;
    z=c;
    while(i<maxIters && z.r*z.r+z.i*z.i<4){
        z=z*z+c;
        i++; 
    }
    return i;
}


int main(int argc, char **argv){

    int rank, size, tag, count, source;
    int data;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);


    Complex c;
    const int DIM = 512;
    Complex row[DIM];
    int iters[DIM];
    int finalIters[DIM][DIM];
    Complex c1, c2;
    c1.r=0.5;
    c1.i=0.39;
    c2.r=-2;
    c2.i=-2;
//        c1.r = atof(argv[1]);
//        c1.i = atof(argv[2]);
//        c2.r = atof(argv[3]);
//        c2.i = c1.i + (c2.r - c1.r);
    if(!rank) {


        ofstream fout;
        fout.open("image.ppm");

        //send a row to each process initially
        fout << "P3" << endl;
        fout << DIM << " " << DIM << endl;
        fout << 255 << endl;

        auto start = std::chrono::system_clock::now();
        for (int j = 0; j < size-1; ++j) {
            MPI_Send(&c,1,MPI_INT,j+1,j,MCW);
        }

        //Send to whatever is available
        for (int j = size; j < DIM; ++j) {
            //recieve row of iters into finalIters[tag]
            MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
            tag = mystatus.MPI_TAG;
            source = mystatus.MPI_SOURCE;
            MPI_Recv(iters,DIM,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
            cout << tag <<":";
            for(int k = 0; k < DIM; ++k) {
                finalIters[tag][k] = iters[k];
                //cout << finalIters[tag][i] << " ";
                cout << iters[k] << " ";
            }
            cout << endl;
            //send row of complex numbers
            MPI_Send(&c,1,MPI_INT,source,j,MCW);
        }
        for(int i = 1; i < size; ++i){
            //recieve row of iters into finalIters[tag]
            MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
            tag = mystatus.MPI_TAG;
            source = mystatus.MPI_SOURCE;
            MPI_Recv(iters,DIM,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
            cout << tag <<":";
            for(int k = 0; k < DIM; ++k) {
                finalIters[tag][k] = iters[k];
                //cout << finalIters[tag][i] << " ";
                cout << iters[k] << " ";
            }
            cout << endl;
        }
        //kill processes
        sleep(1);
        data = -1;

        for(int i = 0; i < size; ++i){
            MPI_Send(&data,1,MPI_INT,i,DIM +1,MCW);
        }


        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end-start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);


        //output file
        for(int j = 0; j <DIM; ++j) {
            //cout << j << ": ";
            for (int i = 0; i < DIM; ++i) {
                //cout << finalIters[j][i] << " ";
                fout << rcolor(finalIters[j][i]) << " ";
                fout << gcolor(finalIters[j][i]) << " ";
                fout << bcolor(finalIters[j][i]) << " ";
            }
            //cout << endl;
            fout << endl;
        }
        fout.close();
        std::cout << "finished computation at " << std::ctime(&end_time)
                  << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }

    //other processes
    else{

        while(1) {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &mystatus);
            tag = mystatus.MPI_TAG;
            MPI_Get_count(&mystatus,MPI_INT,&count);

            if(tag == DIM+1){
                break;
                //MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);
            }else {
                MPI_Recv(&c, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &mystatus);
                //cout << tag << ": ";
                for (int i = 0; i < DIM; ++i) {
                    c.r = (i * (c1.r - c2.r) / DIM) + c2.r;
                    c.i = (tag * (c1.i - c2.i) / DIM) + c2.i;
                    iters[i] = mbrotIters(c, 255);
                    //cout << tag < " " << i << " " << iters[i] << endl;
                    //cout << iters[i] << " ";
                    //cout << c.r << ","<<c.i << " ";
                }
                //cout << endl;
                MPI_Send(iters, DIM, MPI_INT, 0, tag, MCW);
            }
        }
    }

    MPI_Finalize();
    return 0;
}


