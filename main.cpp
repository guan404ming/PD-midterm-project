#include <iostream>
using namespace std;

int getBestShift(const int shiftTime[30][24], const int shiftCount, int demand[24], int nightShiftCount);
int getNightShiftCount(const int workSchedule[100][31], const int shiftTime[30][24], const int day, const int staff);
bool getShouldBreak(const int day, const int staff, const int workDays[100], const int dayCount, const int vacationCount, const int workSchedule[100][31]);
void handleSortStaff(const int workDays[100], int result[100], int staffCount, const int day, const int workSchedule[100][31]);
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

    int vacation[7][10] = {0}, vacationTypeCount = 0;
    for (int i = 0; i < 7; i++)
    {
        int count = 0, cur[10] = {0};
        if (i == 6) 
        {
            cur[count] = 0;
            count++;
        }
        
        for (int j = 0; j < 6; j++)
        {
            
            if (i + j * 7 <= dayCount - 1)
            {
                cur[count] = i + j * 7;
                count++;
            }

            if (i + j * 7 + 1 <= dayCount - 1)
            {
                cur[count] = i + j * 7 + 1;
                count++;
            }
        }
        
        for (int j = 0; j < vacationCount; j++)
        {
            vacation[vacationTypeCount][j] = cur[j];
        }
        vacationTypeCount++;
    }
    for (int i = 0; i < staffCount; i++)
    {
        int count = 0;
        for (int j = 0; j < vacationRequestCount; j++)
        {
            if (vacationRequest[j][0] == i)
            {
                workSchedule[i][vacationRequest[j][1]] = 0;
                count++;
            } 
        }
    
        for (int j = 0; j < vacationCount - count; j++)
        {
            if (vacationTypeCount != 0)
            {
                workSchedule[i][vacation[i % vacationTypeCount][j]] = 0;
            }
        }
    }
    for (int i = 0; i < dayCount; i++)
    {
        int demand[24];
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[i][j];
        }

        int result[100] = {0};
        handleSortStaff(workDays, result, staffCount, i, workSchedule);

        for (int j = 0; j < staffCount; j++)
        {
            if (workSchedule[result[j]][i] != 0)
            {
                int nightShiftCount = getNightShiftCount(workSchedule, shiftTime, i, result[j]);
                int bestShift = getBestShift(shiftTime, shiftCount, demand, nightShiftCount);
                workSchedule[result[j]][i] = (getShouldBreak(i, result[j], workDays, dayCount, vacationCount, workSchedule)) ? 0 : -1;
                if (workSchedule[result[j]][i] != 0)
                {
                    workSchedule[result[j]][i] = bestShift;
                    workDays[result[j]]++;
                    for (int k = 0; k < 24; k++)
                    {
                        if (shiftTime[bestShift][k] == 1 && demand[k] > 0)
                        {
                            demand[k]--;
                        }
                    }
                }
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

void handleSortStaff(const int workDays[100], int result[100], int staffCount, const int day, const int workSchedule[100][31])
{
    int minIdx = 0, count = 0, curWorkDays[100];
    for (int i = 0; i < staffCount; i++)
    {
        curWorkDays[i] = workDays[i];
    }
    
    while (count != staffCount)
    {
        for (int i = 0; i < staffCount; i++)
        {
            int contiWorkCount = 0, minConti = 0;
            for (int k = day - 6; k < day; k++)
            {
                if (k >= 0)
                {
                    if (workSchedule[i][k] != 0)
                    {
                        contiWorkCount++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            contiWorkCount = (contiWorkCount > 3) ? contiWorkCount : -(contiWorkCount);
            for (int k = day - 6; k < day; k++)
            {
                if (k >= 0)
                {
                    if (workSchedule[minIdx][k] != 0)
                    {
                        minConti++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            minConti = (minConti > 3) ? minConti : -(minConti);
            minIdx = (curWorkDays[i] + contiWorkCount < curWorkDays[minIdx] + minConti) ? i : minIdx;
        }
        
        result[count] = minIdx;
        curWorkDays[minIdx] = 1000;
        count++;
    }
}

bool getShouldBreak(const int day, const int staff, const int workDays[100], const int dayCount, const int vacationCount, const int workSchedule[100][31]) {
    
    bool shouldBreak = false;

    // total work
    if (workDays[staff] == dayCount - vacationCount)
    {
        shouldBreak = true;
    }

    // conti work
    if (day >= 6)
    {
        int contiWorkCount = 0;
        for (int k = day - 6; k < day; k++)
        {
            if (workSchedule[staff][k] != 0)
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
            shouldBreak = true;
        }
    }

    return shouldBreak;
}
