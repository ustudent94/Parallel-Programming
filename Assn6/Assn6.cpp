#include <iostream>
#include <vector>
#include <numeric>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>


#define MCW MPI_COMM_WORLD
const int numRows = 40;
const int numCols = 80;
//const int numRows = 10;
//const int numCols = 10;

using namespace std;
// ----------------------------------------------------------------------------

//using Row   = std::vector<int>;
//using Cells = std::vector<Row>;


int getNeighbor(int row, int col, int board[numRows][numCols]) {
    // use modulus to get wrapping effect at board edges
    if(row < 0 || row >= numRows || col < 0 || col >= numCols) {
        return 0;
    }else{
        return board[row][col];
    }
}

int getCount(int row, int col, int board[numRows][numCols]) {
    int count = 0;
    vector<int> deltas {-1, 0, 1};
    for (int dc : deltas) {
        for (int dr : deltas) {
            if (dr || dc) {
                count += getNeighbor(row + dr, col + dc, board);
            }
        }
    }
    return count;
}

void showGeneration(int board[numRows][numCols]) {
    for(int i =0; i <numRows; i++) {
        cout << "|";
        for (int j = 0; j < numCols; j++) {
            cout << (board[i][j] ? "*" : " ");
        }
        cout << "|\n";
    }
}

int tick(int board[numRows][numCols], int row, int col) {
    int count = getCount(row, col, board);
    bool birth = !board[row][col] && count == 3;
    bool survive = board[row][col] && (count == 2 || count == 3);
    return birth || survive;
}

void updateCells(int board[numRows][numCols]) {
    int original[numRows][numCols];
    //copyArray(board,original);
    copy(&board[0][0], &board[0][0]+numRows*numCols, &original[0][0]);

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            board[row][col] = tick(original, row, col);
        }
    }
}

void updateSmall(int **board,int smallRows) {
    const int numSmallRows = smallRows;
    int original[numSmallRows][numCols];
    //copyArray(board,original);
    copy(&board[0][0], &board[0][0]+numRows*numCols, &original[0][0]);

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < numCols; col++) {
            board[row][col] = tick(original, row, col);
        }
    }
}

void copyArray(int copy[numRows][numCols], int **paste){
    for(int i = 0; i < numRows; i++ ){
        for(int j = 0; j < numCols; j++){
            paste[i][j] = copy[i][j];
        }
    }
}

void clearRow(int *row){
    for(int i = 0; i < numCols; i++){
        row[i] = 0;
    }
}

void clearTempBoard(int **board){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < numCols; j++) {
            board[i][j] = 0;
        }
    }
}

int main(int argc, char **argv) {
    ///glider gun test
    int board[numRows][numCols] = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    int rank, size, tag, source, process;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    int tempRow[numCols];
    int numIter = 20;


    ///master
    //master is last process for easy mod operation
    if (rank == size -1) {


        ///Initial state
        //int board[numRows][numCols];
        int seed;
        for(int i = 0; i < numRows; i++){
            clearRow(tempRow);
            for(int j = 0; j < numCols; j++){
                //todo: uncomment when finished debugging
//                seed = i*j;
//                if(rand()%5 == 0){
//                    board[i][j] = 1;
//                }else{
//                    board[i][j] = 0;
//                }
                tempRow[j] = board[i][j];
                //cout << tempRow[j] << " ";
            }
            //cout << endl;


//            process = i/(size-2);
            process = i/(numRows/(size-1));
            //cout << rank << " " << process << " " << i << endl;
            MPI_Send(tempRow,numCols,MPI_INT,process,i,MCW);

        }
        //finalize reception by changing tag
        //MPI_Send(tempRow,numCols,MPI_INT,process,size +1,MCW);

        MPI_Barrier(MCW);
        //recieve each piece into ta
        for (int gen = 0; gen < numIter; gen++) {
            //automatically outputs board
            std::cout << "\ngeneration " << gen << ":\n";
            //showGeneration(board);

            //recieve all rows from processes
            for(int i = 0; i < size -1; i++) {
                //todo: probe and receive count
                MPI_Probe(MPI_ANY_SOURCE,0,MCW,&mystatus);
                int count;
                MPI_Get_count(&mystatus,MPI_INT,&count);
                source = mystatus.MPI_SOURCE;
                const int numRecv = count;
                int tempSection[numRecv][numCols];
                //cout << rank << " recieving " << count << " items from " << source << endl;
                MPI_Recv(tempSection,count,MPI_INT,source,MPI_ANY_TAG,MCW,&mystatus);
                tag = mystatus.MPI_TAG;
                //cout <<rank << " " << tag << endl;
                //put into the appropriate row
                int row = source *(numRows/(size-1));
                //cout << "Section starting at row " << row << endl;
                for(int k = 0; k < numRecv/numCols; k++) {
                    for (int j = 0; j < numCols; j++) {
                        board[row +k][j] = tempSection[k][j];
                        //cout << board[row +k][j] << " ";
                    }
                    //cout << endl;
                }

            }
            MPI_Barrier(MCW);
        }
        std::cout << "\ngeneration " << numIter << ":\n";
        showGeneration(board);
    }




    ///slave
    else {

//        cout << rank << " 1 " << numRows/(size-1) << endl;
//        cout << rank << " 2 " <<(rank/(numRows%(size-1)))*-1 + 1 <<endl;
        //const int numParts = numRows/(size-1) + (rank/(numRows%(size-1)))*-1 + 1;
        int sectionSize = numRows / (size - 1);
        int numExtra = 2;

        //account for smaller last section
        if(rank == size -2) {
            sectionSize = numRows - rank *sectionSize;
            numExtra = 1;
        }else if(!rank){
            numExtra = 1;
        }

        const int numSectionRows = sectionSize;
        const int numUpdateRows = sectionSize + numExtra;
        //cout << rank << " " << numParts << endl;
        int section[numSectionRows][numCols];
        int updateBoard[numUpdateRows][numCols];
        int rowList[numSectionRows];
        //todo: something weird is happening inside for loop where rank and size are getting changed
        //recieve in rows to be in charge of
        for(int i = 0; i < numSectionRows; i++){
            //cout <<rank << " recieving " << i << " " << endl;

            MPI_Recv(tempRow,numCols,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&mystatus);
            tag = mystatus.MPI_TAG;
            //cout <<rank << " " << tag << " " << endl;
            rowList[i] = tag;
            for(int j= 0; j < numCols; j++){
                section[tag][j] = tempRow[j];
            }
        }

        MPI_Comm_rank(MCW, &rank);
        MPI_Comm_size(MCW, &size);
        cout << rank << " Size: " << size<<endl;
        MPI_Barrier(MCW);

        //loop iterations
        for (int gen = 0; gen < numIter; gen++) {
//            for(int i = 0; i < numSectionRows; i++) {
            for(int j = 0; j < numCols; j++){
                tempRow[j] = updateBoard[0][j];
            }
            //send below

            if(rank >0){
                process = rank -1;
                //cout << rank << " " << process << " S1" << endl;
                MPI_Send(tempRow, numCols, MPI_INT, process, 0, MCW);
            }

            if(rank < size-2){
                source = rank +1;
                //cout << rank << " " << source << " R1" << endl;
                MPI_Recv(tempRow,numCols,MPI_INT,source,0,MCW,&mystatus);
            }

            for(int j = 0; j < numCols; j++){
                tempRow[j] = updateBoard[numSectionRows-1][j];
            }
            if(rank < size-2){
                process = rank+1;
                //cout << rank << " " << process << " S2" << endl;
                MPI_Send(tempRow, numCols, MPI_INT, process, 1, MCW);
            }

            if(rank >0){
                source = rank - 1;
                //cout << rank << " " << source << " R2" << endl;
                MPI_Recv(tempRow,numCols,MPI_INT,source,1,MCW,&mystatus);
            }

            //updateSection(updateBoard, numUpdateRows);
            int original[numUpdateRows][numCols];
            copy(&updateBoard[0][0], &updateBoard[0][0]+numRows*numCols, &original[0][0]);

            for (int row = 0; row < numUpdateRows; row++) {
                for (int col = 0; col < numCols; col++) {
                    int count = getCount(row, col, board);
                    bool birth = !board[row][col] && count == 3;
                    bool survive = board[row][col] && (count == 2 || count == 3);
                    updateBoard[row][col] = birth || survive;
                }
            }

            int startIndex = 0;
            int endIndex = numUpdateRows;
            if(!rank){
                startIndex = 1;
            }
            if(rank == size -2){
                endIndex = numUpdateRows-1;
            }
            for(int i = startIndex; i < endIndex; i++){
                for(int j = 0; j < numCols ; j++) {
                    if(!rank){
                        section[i][j] = updateBoard[i][j];
                    }else {
                        section[i][j] = updateBoard[i+1][j];
                    }
                }
            }
            //cout << rank << " " << size-1 << endl;

            cout <<rank << " Sending to  "<< (size - 1)  <<  endl;
            MPI_Send(section, numSectionRows * numCols, MPI_INT, (size-1), 0, MCW);
//            }
            //cout <<rank << "made it here" <<endl;
            MPI_Barrier(MCW);
        }
    }
}