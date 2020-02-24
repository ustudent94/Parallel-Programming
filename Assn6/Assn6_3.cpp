
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#define numRows 20
#define numCols 20




using namespace std;

int main(int argc, char **argv){

    int world[DIM][DIM];
    int next[DIM][DIM];

    for(int i=0;i<DIM;++i){
        for(int j=0;j<DIM;++j){
            int value= rand()%5;
            if(value>0)value=0;
            else value=1;
            world[i][j]=value;
        }
    }

    while(1){
        cout<<endl;
        cout<<endl;
        cout<<endl;
        cout<<endl;
        cout<<endl;
        cout<<endl;
        for(int i=0;i<DIM;++i){
            for(int j=0;j<DIM;++j){
                if(world[i][j])cout<<" *";
                else cout<<" .";
            }
            cout<< endl;
        }
        
        
        for(int i=1;i<DIM-1;++i){
            for(int j=1;j<DIM-1;++j){
                int sum;
                sum = world[i-1][j-1]+ world[i  ][j-1]+ world[i+1][j-1]+ world[i-1][j+1]+ world[i  ][j+1]+ world[i+1][j+1]+ world[i-1][j]+ world[i+1][j]; 

                if(world[i][j]==0&&sum==3){next[i][j]=1;continue;}
                if(world[i][j]==0&&sum!=3){next[i][j]=0;continue;}
                if(sum<2){next[i][j]=0;continue;}
                if(sum>3){next[i][j]=0;continue;}
                next[i][j]=1;
            }
        }

        for(int i=0;i<DIM;++i){
            for(int j=0;j<DIM;++j){
                world[i][j]=next[i][j];
            }
        }

        sleep(1);

    }



    

    return 0;
}

