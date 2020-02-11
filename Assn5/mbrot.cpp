#include <iostream>
#include <fstream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

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

    int rank, size, tag;
    //int data[64];
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);


    Complex c;
    const int DIM = 512;
    Complex row[DIM];
    int iters[DIM];
    int finalIters[DIM][DIM];

    if(!rank) {
        Complex c1, c2, c3;

        ofstream fout;
        fout.open("image.ppm");
        //params(file, c1.r,c1.i,c2.r)
//    c1.r=0.5;
//    c1.i=0.39;
//    c2.r=-2;
//    c2.i=-2;
        c1.r = atof(argv[1]);
        c1.i = atof(argv[2]);
        c2.r = atof(argv[3]);
        c2.i = c1.i + (c2.r - c1.r);

        c3 = c1 + c2;
        cout << c3.r << " + " << c3.i << "i" << endl;

        c3 = c1 * c2;
        cout << c3.r << " + " << c3.i << "i" << endl;

        //send a row to each process initially
        fout << "P3" << endl;
        fout << DIM << " " << DIM << endl;
        fout << 255 << endl;
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < DIM; ++i) {
                c.r = (i * (c1.r - c2.r) / DIM) + c2.r;
                c.i = (j * (c1.i - c2.i) / DIM) + c2.i;
                row[i] = c;
            }
            MPI_Send(row,DIM,MPI_INT,0,j,MCW);
        }

        for (int j = size +1; j < DIM; ++j) {
            for (int i = 0; i < DIM; ++i) {
                c.r = (i * (c1.r - c2.r) / DIM) + c2.r;
                c.i = (j * (c1.i - c2.i) / DIM) + c2.i;
                row[i] = c;
            }
            //recieve row of iters into finalItes[tag]
            MPI_Probe(MPI_ANY_SOURCE,0,MCW,&mystatus);
            tag = mystatus.MPI_TAG;
            MPI_Recv(iters,DIM,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
            
            for(int k = 0; k < DIM; ++k) {
                finalIters[tag][k] = iters[k];
            }
            //send row of complex numbers
            MPI_Send(row,DIM,MPI_INT,0,j,MCW);
        }




        //output file
        for(int j = 0; j <DIM; ++j) {
            for (int i = 0; i < DIM; ++i) {
                fout << rcolor(finalIters[j][i]) << " ";
                fout << gcolor(finalIters[j][i]) << " ";
                fout << bcolor(finalIters[j][i]) << " ";
            }
            fout << endl;
        }
        fout.close();
    }

    //other processes
    else{
        //int iters;
        MPI_Probe(MPI_ANY_SOURCE,0,MCW,&mystatus);
        tag = mystatus.MPI_TAG;
        MPI_Recv(row,DIM,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
        for(int i = 0; i < DIM; ++i){
            iters[i] = mbrotIters(row[i],255);
        }

        //MPI_Send(iters,DIM,MPI_INT,0,tag,MCW);
    }

    MPI_Finalize();
    return 0;
}


