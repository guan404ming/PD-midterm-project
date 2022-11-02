#include <iostream>
using namespace std;

int getBestShift(const int shiftTime[30][24], const int shiftCount, int demand[24], int nightShiftCount);
int getNightShiftCount(const int workSchedule[100][31], const int shiftTime[30][24], const int day, const int staff);
void handleSortStaff(const int workDays[100], int result[100], int staffCount);
int handleCountPoint(const int dayCount, const int staffCount, const int workerDemand[31][24], const int workSchedule[100][31], const int shiftTime[30][24], const int overNightShiftWeight);

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
    
    // shift 0
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
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            workSchedule[i][j] = -1;
        }
    }

    // ================================================================ //

    for (int i = 0; i < dayCount; i++)
    {
        int demand[24], demandCount = 0;
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[i][j];
            demandCount += workerDemand[i][j];
        }

        // sort the staff by how many days they work
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
                for (int k = 0; k < vacationRequestCount; k++)
                {
                    if (vacationRequest[k][0] == result[j] && vacationRequest[k][1] == i)
                    {
                        workSchedule[result[j]][i] = 0;
                    } 
                }
                
                // assign shift
                if (workSchedule[result[j]][i] == -1)
                {
                    workSchedule[result[j]][i] = bestShiftIdx;
                    workDays[result[j]]++;
                    for (int k = 0; k < 24; k++)
                    {
                        if (demand[k] > 0)
                        {
                            demandCount--;
                        }
                        if (shiftTime[bestShiftIdx][k] == 1 && demand[k] > 0)
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
    int cur = handleCountPoint(dayCount, staffCount, workerDemand, workSchedule, shiftTime, overNightShiftWeight);
    int curSchedule[100][31];
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            curSchedule[i][j] = workSchedule[i][j];
        }
    }

    // type 2
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            workSchedule[i][j] = -1;
        }
    }
    for (int i = 0; i < staffCount; i++)
    {
        workDays[i] = 0;
    }
    
    for (int i = 0; i < dayCount; i++)
    {
        int demand[24], demandCount = 0;
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[i][j];
            demandCount += workerDemand[i][j];
        }

        // sort the staff by how many days they work
        int result[100] = {0};
        handleSortStaff(workDays, result, staffCount);

        bool hasPoint = true;
        int count = 0;
        for (int j = 0; j < staffCount; j++)
        {
            int max = -1;
            for (int i = 0; i < 24; i++)
            {
                max = (demand[i] > max) ? demand[i] : max;
            }
            
            // cout << "max: " << max << "\n"; 
            
            if (demandCount > 36 && hasPoint && max >= 6)
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
                for (int k = 0; k < vacationRequestCount; k++)
                {
                    if (vacationRequest[k][0] == result[j] && vacationRequest[k][1] == i)
                    {
                        workSchedule[result[j]][i] = 0;
                    } 
                }
            
                // assign shift
                if (workSchedule[result[j]][i] == -1)
                {
                    workSchedule[result[j]][i] = bestShiftIdx;
                    workDays[result[j]]++;
                    int point = 0;
                    for (int k = 0; k < 24; k++)
                    {
                        if (shiftTime[bestShiftIdx][k] == 1 && demand[k] > 0)
                        {
                            demand[k]--;
                            point++;
                        }
                    }
                    // cout << "point " << point << "\n";
                    if (point > 4)
                    {
                        demandCount -= point;
                        count++;
                    }
                    else
                    {
                        hasPoint = false;
                    }
                }
            }
            else
            {
                workSchedule[result[j]][i] = 0;
            }
        }
    }
    int cur2 = handleCountPoint(dayCount, staffCount, workerDemand, workSchedule, shiftTime, overNightShiftWeight);
    if (cur2 < cur)
    {
        for (int i = 0; i < staffCount; i++)
        {
            for (int j = 0; j < dayCount; j++)
            {
                curSchedule[i][j] = workSchedule[i][j];
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
                cout << curSchedule[i][j] << ",";
            }
            else
            {
                cout << curSchedule[i][j];
            }
        }
        cout << "\n";
    }
    
    return 0;
}

int getBestShift(const int shiftTime[30][24], const int shiftCount, int demand[24], int nightShiftCount)
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
            // include all shift, and if equal choose the one which doesn't contain night shift
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

void handleSortStaff(const int workDays[100], int result[100], int staffCount)
{
    int minIdx = 0, count = 0, curWorkDays[100];
    for (int i = 0; i < staffCount; i++)
    {
        curWorkDays[i] = workDays[i];
    }
    
    while (count != staffCount)
    {
        // if (count % 2 == 0)
        // {
        //     for (int i = 0; i < staffCount; i++)
        //     {
        //         minIdx = (curWorkDays[i] < curWorkDays[minIdx]) ? i : minIdx;
        //     }
        // }
        // else
        // {
        //     for (int i = 0; i < staffCount; i++)
        //     {
        //         minIdx = (curWorkDays[i] <= curWorkDays[minIdx]) ? i : minIdx;
        //     }
        // }
        for (int i = 0; i < staffCount; i++)
        {
            minIdx = (curWorkDays[i] < curWorkDays[minIdx]) ? i : minIdx;
        }
        
        result[count] = minIdx;
        curWorkDays[minIdx] = 1000;
        count++;
    }
}

int handleCountPoint(const int dayCount, const int staffCount, const int workerDemand[31][24], const int workSchedule[100][31], const int shiftTime[30][24], const int overNightShiftWeight)
{
    int workforceLack = 0;
    for (int i = 0; i < dayCount; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            int curDemand = workerDemand[i][j];
            int curSupply = 0;
            for (int k = 0; k < staffCount; k++)
            {
                int curShift = workSchedule[k][i];
                if (shiftTime[curShift][j] == 1)
                {
                    curSupply++;
                }
            }
            
            if (curDemand > curSupply)
            {
                workforceLack += curDemand - curSupply;
            }
        }
    }

    int overNightShift = 0;
    for (int i = 6; i < dayCount; i++)
    {
        for (int j = 0; j < staffCount; j++)
        {
            int cur = (getNightShiftCount(workSchedule, shiftTime, i, j));
            bool isNightShift = false;
            for (int k = 18; k <= 23; k++)
            {
                if (shiftTime[workSchedule[j][i]][k] == 1)
                {
                    isNightShift = true;
                    break;
                }
            }
            cur += (isNightShift ? 1 : 0);
            if (cur >= 2)
            {
                cur -= 1;
                overNightShift += cur * overNightShiftWeight;
            }
        }
    }
    
    
    if (workSchedule[0][0] == -1) {
        workforceLack = 0;
        overNightShift = 0;
    }
    // cout << "point: " << workforceLack + overNightShift << "\n";
    return workforceLack + overNightShift;
}