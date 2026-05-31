#include <iostream>
#include <random>

using namespace std;

int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(3, 10);
    int N = distrib(gen);
    
    int arr[N][N] = {};
    
    uniform_int_distribution<> dir(0, 1);
    bool RandSpir = dir(gen); // 0 или 1

    for (int ik = 0; ik < N; ik++) {
        for (int jk = 0; jk < N; jk++) {
            int i = ik + 1;
            int j = jk + 1;

            if (RandSpir) {
                int temp = i;
                i = j;
                j = temp;
            }
            int switcher = (j - i + N) / N;
            int Ic = abs(i - N / 2 - 1) + (i - 1) / (N / 2) * ((N - 1) % 2);
            int Jc = abs(j - N / 2 - 1) + (j - 1) / (N / 2) * ((N - 1) % 2);
            int Ring = N / 2 - (abs(Ic - Jc) + Ic + Jc) / 2;
            int Xs = i - Ring + j - Ring - 1;
            int Coef = 4 * Ring * (N - Ring);
            

            arr[ik][jk] = Coef + switcher * Xs + abs(switcher - 1) * (4 * (N - 2 * Ring) - 2 - Xs);
            
           
            
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}