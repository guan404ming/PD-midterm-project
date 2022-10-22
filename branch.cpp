#include <iostream>
using namespace std;

int getBestShift(const int shiftTime[30][24], const int shiftCount, const int demand[24], const int nightShiftCount);
int getNightShiftCount(const int workSchedule[100][31], const int shiftTime[30][24], const int day, const int staff);
int getBestDemandCount(const int shiftTime[30][24], const int shiftCount, const int sum, const int demand[24]);
void handleSortStaff(const int workDays[100], int result[100], int staffCount);

int main()
{
    
    // staff max = 100, day max = 31, shift max = 30, vacaiton max = 8, request max = 3100
    int staffCount = 0, dayCount = 0, shiftCount = 0, vacationCount, noVacationWeight, overNightShiftWeight, vacationRequestCount;
    cin >> staffCount >> dayCount >> shiftCount >> vacationCount >> noVacationWeight >> overNightShiftWeight >> vacationRequestCount;

    // [shift:[0 or 1]]
    // [demand18, demand23] is nightshift
    int shiftTime[30][24];
    for (int i = 1; i < shiftCount + 1; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            cin >> shiftTime[i][j]; 
        }
    }

    for (int i = 0; i < 24; i++)
    {
        cin >> shiftTime[0][i];
    }

    // [day:[demand1, demand2,...,demand24]]
    int workerDemand[31][24];
    for (int i = 0; i < dayCount; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            cin >> workerDemand[i][j];
        }
    }
    
    // [[staff, day]]
    int vacationRequest[3100][2];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < vacationRequestCount; j++)
        {
            cin >> vacationRequest[j][i];
            vacationRequest[j][i]--;
        } 
    }

    // [[staff, shift]]
    int workDays[100] = {0}, workSchedule[100][31];
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 31; j++)
        {
            workSchedule[i][j] = -1;
        }
    }

    // ================================================================ //

    for (int i = 0; i < dayCount; i++)
    {
        // init
        int demand[24], sum = 0;
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[i][j];
            sum += workerDemand[i][j];
        }
        
        int demandCount = getBestDemandCount(shiftTime, shiftCount, sum, demand);
        int result[100] = {0};
        handleSortStaff(workDays, result, staffCount);

        for (int j = 0; j < staffCount; j++)
        {
            if (demandCount > 0)
            {
                // total work
                if (workDays[result[j]] == dayCount - vacationCount)
                {
                    workSchedule[result[j]][i] = 0;
                }

                // conti work
                if (i >= 6)
                {
                    int contiWorkCount = 0;
                    for (int k = i - 6; k < i; k++)
                    {
                        if (workSchedule[result[j]][k] != 0)
                        {
                            contiWorkCount++;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (contiWorkCount >= 6)
                    {
                        workSchedule[result[j]][i] = 0;
                    }
                }
                
                // =============================================== // 
                
                
                // nightnight shift
                int nightShiftCount = getNightShiftCount(workSchedule, shiftTime, i, result[j]);
                int bestShiftIdx = getBestShift(shiftTime, shiftCount, demand, nightShiftCount);
                
                // request
                if (noVacationWeight > 0)
                {
                    for (int k = 0; k < vacationRequestCount; k++)
                    {
                        if (vacationRequest[k][0] == result[j] && vacationRequest[k][1] == i)
                        {
                            workSchedule[result[j]][i] = 0;
                        } 
                    }
                }
                
                // assign shift
                if (workSchedule[result[j]][i] == -1)
                {
                    workSchedule[result[j]][i] = bestShiftIdx;
                    workDays[result[j]]++;
                    demandCount--;
                    for (int k = 0; k < 24; k++)
                    {
                        if (demand[k] > 0 && shiftTime[bestShiftIdx][k] == 1)
                        {
                            demand[k]--;
                        }
                    }
                } 
            }
            else
            {
                workSchedule[result[j]][i] = 0;
            }
        }
    }

    // output
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            if (j != dayCount -1 )
            {
                cout << workSchedule[i][j] << ",";
            }
            else
            {
                cout << workSchedule[i][j];
            }
        }
        cout << "\n";
    }
    return 0;
}

int getBestShift(const int shiftTime[30][24], const int shiftCount, const int demand[24], const int nightShiftCount)
{
    int maxIdx = -1, max = -1;
    for (int i = 0; i < shiftCount; i++)
    {
        int cur = 0;
        for (int j = 0; j < 24; j++)
        {
            if (demand[j] > 0 && shiftTime[i][j] == 1)
            {
                cur++;
            }
        }

        bool isNightShift = false;
        for (int j = 18; j <= 23; j++)
        {
            if (shiftTime[i][j] == 1)
            {
                isNightShift = true;
                break;
            }
        }

        if (nightShiftCount > 0)
        {
            // only not night shift
            maxIdx = (cur >= max && !isNightShift) ? i : maxIdx;
            max = (cur >= max && !isNightShift) ? cur : max;
        }
        else
        {   
            maxIdx = (cur > max || (cur == max && !isNightShift)) ? i : maxIdx;
            max = (cur > max || (cur == max && !isNightShift)) ? cur : max;
        }
    }
    return maxIdx;
}

int getNightShiftCount(const int workSchedule[100][31], const int shiftTime[30][24], const int day, const int staff)
{
    int nightShiftCount = 0;
    if (day >= 6)
    {
        for (int i = day - 6; i < day; i++)
        {
            int curShift = workSchedule[staff][i];
            for (int j = 18; j <= 23; j++)
            {
                if (shiftTime[curShift][j] == 1)
                {
                    nightShiftCount += 1;
                    break;
                }
            }
        }
    }
    else
    {
        
        for (int i = day - 1; i >= 0; i--)
        {
            int curShift = workSchedule[staff][i];
            for (int j = 18; j <= 23; j++)
            {
                if (shiftTime[curShift][j] == 1)
                {
                    nightShiftCount += 1;
                    break;
                }
            }
        }
    }
    return nightShiftCount;
}

int getBestDemandCount(const int shiftTime[30][24], const int shiftCount, const int sum, const int demand[24])
{
    int demandCount = 0, cur = 24, curSum = sum;

    while (curSum > 0 && cur >= 12)
    {
        int maxIdx = -1, max = -1;
        for (int i = 0; i < shiftCount; i++)
        {
            cur = 0;
            for (int j = 0; j < 24; j++)
            {
                if (demand[j] > 0 && shiftTime[i][j] == 1)
                {
                    cur++;
                }
            }
            maxIdx = (cur > max) ? i : maxIdx;
            max = (cur > max) ? cur : max;
        }
        cur = max;
        curSum -= max;
        demandCount++;
        // cout << "result( " << curSum << " " << cur << " )\n";
    }
    // cout << "==============================\n";
    return demandCount;
}

void handleSortStaff(const int workDays[100], int result[100], int staffCount)
{
    int minIdx = 0, count = 0, curWorkDays[100];
    for (int i = 0; i < staffCount; i++)
    {
        curWorkDays[i] = workDays[i];
    }
    
    while (count != staffCount)
    {
        if (count % 2 == 0)
        {
            for (int i = 0; i < staffCount; i++)
            {
                minIdx = (curWorkDays[i] < curWorkDays[minIdx]) ? i : minIdx;
            }
        }
        else
        {
            for (int i = 0; i < staffCount; i++)
            {
                minIdx = (curWorkDays[i] <= curWorkDays[minIdx]) ? i : minIdx;
            }
        }
        
        result[count] = minIdx;
        curWorkDays[minIdx] = 1000;
        count++;
    }
}