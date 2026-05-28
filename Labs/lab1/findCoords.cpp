#include <vector>
#include "findCoords.h"

std::vector<std::vector<int>> findCoords(int ***mtrx, int l, int m, int n, int z) {
    std::vector<std::vector<int>> result;

    for (int i = 0; i < l; i++)
        for (int j = 0; j < m; j++)
            for (int k = 0; k < n; k++)
                if (mtrx[i][j][k] == z)
                    result.push_back({i, j, k});

    return result;
}