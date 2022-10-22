#include <iostream>
using namespace std;

int main() {
    int workDays[100] = {6, 2, 3, 5, 4, 3, 2, 3,3, 9};
    int result[100] = {0};
    int staffCount = 5;
    int minIdx = 0, count = 0, curWorkDays[100];
    for (int i = 0; i < staffCount; i++)
    {
        curWorkDays[i] = workDays[i];
    }
    
    do
    {
        for (int i = 0; i < staffCount; i++)
        {
            minIdx = (curWorkDays[i] < curWorkDays[minIdx]) ? i : minIdx;
        }
        result[count] = minIdx;
        curWorkDays[minIdx] = 100000;
        count++;
        
    } while (count != staffCount);

        
}
