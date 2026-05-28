#include <iostream>
#include <vector>
#include "findCoords.h"



int main() {
    // Создаём массив 2x2x2
    int l = 2, m = 2, n = 2;
    int ***mtrx = new int**[l];
    for (int i = 0; i < l; i++) {
        mtrx[i] = new int*[m];
        for (int j = 0; j < m; j++) {
            mtrx[i][j] = new int[n];
        }
    }

    // Заполняем
    mtrx[0][0][0] = 1;  mtrx[0][0][1] = 5;
    mtrx[0][1][0] = 3;  mtrx[0][1][1] = 5;
    mtrx[1][0][0] = 5;  mtrx[1][0][1] = 2;
    mtrx[1][1][0] = 4;  mtrx[1][1][1] = 7;

    // Ищем все элементы равные 5
    auto result = findCoords(mtrx, l, m, n, 5);

    for (auto& coords : result)
        std::cout << "(" << coords[0] << ", " << coords[1] << ", " << coords[2] << ")\n";

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < m; j++)
            delete[] mtrx[i][j];
        delete[] mtrx[i];
    }
    delete[] mtrx;

    return 0;
}