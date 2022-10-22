#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

int main() {
    srand(time(0));
    int i, j, result[staffCount] = {0};
    for (int i = 0; i < staffCount; i++)
    {
        bool isCopy = true;
        while (isCopy)
        {
            int cur = rand() % staffCount + 1;
            int timer = 0;
            for (int i = 0; i < staffCount; i++)
            {
                if (result[i] != cur)
                {
                    timer++;
                }
            }
            if (timer == staffCount)
            {
                result[i] = cur;
                isCopy = false;
            }
        }
        cout << result[i] << " ";
    }
    
    return 0;
}
