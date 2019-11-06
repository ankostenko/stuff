#include <vector>
#include <cstdio>
#include <cmath>
#include <chrono>
#include <thread>

using namespace std::chrono;

int nFieldWidth = 8;

void printField(std::vector<bool>& field)
{
    printf("\n\n\n\n\n");
    
    for (int i = 0; i < nFieldWidth; i++)
    {
        for (int j = 0; j < nFieldWidth; j++)
            printf("%c ", field[i * nFieldWidth + j] ? 'g' : '_');
        printf("\n");
    }
}


bool dfs(std::vector<bool> used, int posX, int posY)
{
    used[posY * nFieldWidth + posX] = true;
    printField(used);
    std::this_thread::sleep_for(milliseconds(1000));
    
    for (int x = -2; x < 3; x++)
        for (int y = -2; y < 3; y++)
            if (abs(x) + abs(y) == 3 &&
                posX + x >= 0 && posX + x < 8 &&
                posY + y >= 0 && posY + y < 8 &&
                !used[(posY + y) * nFieldWidth + (posX + x)])
            {
                if (dfs(used, posX + x, posY + y))
                    return true;
            }
    
    return false;
}


int main(int argc, const char * argv[]) {
    
    std::vector<bool> bUsed(64, false);
    
    for (int i = 0; i < nFieldWidth; i++)
        for (int j = 0; j < nFieldWidth; j++)
            dfs(bUsed, j, i);
    
    
    

    return 0;
}
