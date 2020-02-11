#include <iostream>
#include <fstream>

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

    Complex c1,c2,c3;
    Complex c;
    ofstream fout;
    fout.open("image.ppm");
    //params(file, c1.r,c1.i,c2.r)
//    c1.r=0.5;
//    c1.i=0.39;
//    c2.r=-2;
//    c2.i=-2;
    c1.r=atof(argv[1]);
    c1.i=atof(argv[2]);
    c2.r=atof(argv[3]);
    c2.i=c1.i + (c2.r-c1.r);

    c3=c1+c2;
    cout << c3.r << " + " << c3.i <<"i"<<endl;

    c3=c1*c2;
    cout << c3.r << " + " << c3.i <<"i"<<endl;

    int DIM = 500;
    fout<<"P3"<<endl;
    fout<<DIM<<" "<<DIM<<endl;
    fout<<255<<endl;

    for(int j=0;j<DIM;++j){
        for(int i=0;i<DIM;++i){
            c.r = (i*(c1.r-c2.r)/DIM)+c2.r;
            c.i = (j*(c1.i-c2.i)/DIM)+c2.i;

            int iters = mbrotIters(c,255);
            fout << rcolor(iters)<<" ";
            fout << gcolor(iters)<<" ";
            fout << bcolor(iters)<<" ";

        }
        fout << endl;
    }
    

    fout.close();

    return 0;
}


