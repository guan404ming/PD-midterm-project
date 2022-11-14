#include <iostream>
#include <algorithm>
using namespace std;

// check staff should break or not
void handleShouldBreak(const int day, const int staff, const int workDays[100], const int dayCount, const int vacationCount, int curSchedule[100][31]) {
    
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
            if (curSchedule[staff][k] != 0)
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

    curSchedule[staff][day] = (shouldBreak) ? 0 : -1;
}

// get total night shift the staff have worked among the past six days
int getNightShiftCount(const int curSchedule[100][31], const int shiftTime[30][24], const int day, const int staff)
{
    int nightShiftCount = 0;
    for (int i = day - 6; i < day; i++)
    {
        if (i >= 0)
        {
            int curShift = curSchedule[staff][i];
            if (curShift != -1)
            {
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
    }
    return nightShiftCount;
}

// get the best shift by current state
int getBestShift(const int shiftTime[30][24], const int shiftCount, int demand[24], int nightShiftCount, int type, int overNightShiftWeight)
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

        if ((nightShiftCount > 0 && type != 1) || (nightShiftCount > 1 && type == 1))
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

// get the current schedule's points
int getPoint(const int dayCount, const int staffCount, const int workerDemand[31][24], const int curSchedule[100][31], const int shiftTime[30][24], const int overNightShiftWeight, const int vacationRequestCount, const int noVacationWeight, const int vacationRequest[3100][2])
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
                int curShift = curSchedule[k][i];
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
            int cur = (getNightShiftCount(curSchedule, shiftTime, i, j));
            bool isNightShift = false;
            for (int k = 18; k <= 23; k++)
            {
                if (shiftTime[curSchedule[j][i]][k] == 1)
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

    int noVacation = 0;
    for (int i = 0; i < vacationRequestCount; i++)
    {
        if (curSchedule[vacationRequest[i][0]][vacationRequest[i][1]] != 0)
        {
            noVacation += noVacationWeight;
        }
    }
    return workforceLack + overNightShift + noVacationWeight;
}

// sort the day by the worker demand
void handleSortDay(int result[31], const int dayCount, const int workerDemand[31][24])
{
    int maxIdx = -1, count = 0, curWorkerDemand[31] = {0};
    
    for (int i = 0; i < dayCount; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            curWorkerDemand[i] += workerDemand[i][j];
        }
    }

    while (count != dayCount)
    {
        for (int i = 0; i < dayCount; i++)
        {
            maxIdx = (curWorkerDemand[i] > curWorkerDemand[maxIdx]) ? i : maxIdx;
        }
        result[count] = maxIdx;
        curWorkerDemand[maxIdx] = -1;
        count++;
    }
}

// sort the staff by the total work day
void handleSortStaff(const int workDays[100], int result[100], int staffCount, const int day, const int curSchedule[100][31], int type)
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
                    if (curSchedule[i][k] != 0)
                    {
                        contiWorkCount++;
                    }
                    else
                    {
                        contiWorkCount = 0;
                    }
                }
            }
            for (int k = day - 6; k < day; k++)
            {
                if (k >= 0)
                {
                    if (curSchedule[minIdx][k] != 0)
                    {
                        minConti++;
                    }
                    else
                    {
                        minConti = 0;
                    }
                }
            }
            if (type == 0)
            {
                minIdx = (curWorkDays[i] - contiWorkCount / 2 < curWorkDays[minIdx] - minConti / 2) ? i : minIdx;
            }
            else
            {
                minIdx = (curWorkDays[i] - contiWorkCount * 10 < curWorkDays[minIdx] - minConti * 10) ? i : minIdx;
            }
        }
        
        result[count] = minIdx;
        curWorkDays[minIdx] = 1000;
        count++;
    }
}

// compare the two schedules' points
void handleComeparePoint(const int dayCount, const int staffCount, const int workerDemand[31][24], const int curSchedule[100][31], const int shiftTime[30][24], const int overNightShiftWeight, int workSchedule[100][31], const int vacationRequestCount, const int noVacationWeight, const int vacationRequest[3100][2])
{
    int cur = getPoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, vacationRequestCount, noVacationWeight, vacationRequest);
    int min = getPoint(dayCount, staffCount, workerDemand, workSchedule, shiftTime, overNightShiftWeight, vacationRequestCount, noVacationWeight, vacationRequest);
    cout << cur << " " << min << "\n";
    if (cur < min)
    {
        for (int i = 0; i < staffCount; i++)
        {
            for (int j = 0; j < dayCount; j++)
            {
                workSchedule[i][j] = curSchedule[i][j];
            }
        }
    }
}

// initialize all varible and prepare to start an new algorism
void handleRestart(int curSchedule[100][31], int workDays[100], const int dayCount, const int staffCount)
{
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            curSchedule[i][j] = -1;
        }
    }
    
    for (int i = 0; i < staffCount; i++)
    {
        workDays[i] = 0;
    }
}

int main()
{
    // staff max = 100, day max = 31, shift max = 30, vacaiton max = 8, request max = 3100
    int staffCount, dayCount, shiftCount, vacationCount, noVacationWeight, overNightShiftWeight, vacationRequestCount;
    cin >> staffCount >> dayCount >> shiftCount >> vacationCount >> noVacationWeight >> overNightShiftWeight >> vacationRequestCount;

    // [shift:[0 or 1]]
    // [demand18, demand23] is nightshift
    shiftCount++;
    int shiftTime[30][24] = {0};
    for (int i = 1; i < shiftCount; i++)
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
    int workerDemand[31][24] = {0};
    for (int i = 0; i < dayCount; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            cin >> workerDemand[i][j];
        }
    }
    
    // [[staff, day]]
    int vacationRequest[3100][2] = {0};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < vacationRequestCount; j++)
        {
            cin >> vacationRequest[j][i];
            vacationRequest[j][i]--;
        } 
    }

    // [[staff, shift]]
    int workDays[100] = {0}, curSchedule[100][31], workSchedule[100][31] = {0};
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            curSchedule[i][j] = -1;
        }
    }

    int resultD[31] = {0};
    handleSortDay(resultD, dayCount, workerDemand);
    
    // ================================================================ //

    // type 1
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
        handleSortStaff(workDays, result, staffCount, i, curSchedule, 0);

        for (int j = 0; j < staffCount; j++)
        {
            if (demandCount > 0)
            {
                // conti work and total work
                handleShouldBreak(i, result[j], workDays, dayCount, vacationCount, curSchedule);
                
                // nightnight shift
                int nightShiftCount = getNightShiftCount(curSchedule, shiftTime, i, result[j]);
                int bestShiftIdx = getBestShift(shiftTime, shiftCount, demand, nightShiftCount, 0, overNightShiftWeight);
                
                // request
                for (int k = 0; k < vacationRequestCount; k++)
                {
                    if (vacationRequest[k][0] == result[j] && vacationRequest[k][1] == i)
                    {
                        curSchedule[result[j]][i] = 0;
                    } 
                }
                
                // assign shift
                if (curSchedule[result[j]][i] == -1)
                {
                    curSchedule[result[j]][i] = bestShiftIdx;
                    workDays[result[j]]++;
                    for (int k = 0; k < 24; k++)
                    {
                        if (demand[k] > 0 && shiftTime[bestShiftIdx][k] == 1)
                        {
                            demandCount--;
                            demand[k]--;
                        }
                    }
                } 
            }
            else
            {
                curSchedule[result[j]][i] = 0;
            }
        }
    }
    handleComeparePoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, workSchedule, vacationRequestCount, noVacationWeight, vacationRequest);
    handleRestart(curSchedule, workDays, dayCount, staffCount);

    // type 2
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
        handleSortStaff(workDays, result, staffCount, i, curSchedule, 0);

        bool hasPoint = true;
        for (int j = 0; j < staffCount; j++)
        {
            if (demandCount > 0 && hasPoint)
            {
                // conti work and total work
                handleShouldBreak(i, result[j], workDays, dayCount, vacationCount, curSchedule);

                // nightnight shift
                int nightShiftCount = getNightShiftCount(curSchedule, shiftTime, i, result[j]);
                int bestShiftIdx = getBestShift(shiftTime, shiftCount, demand, nightShiftCount, 0, overNightShiftWeight);
                
                // request
                for (int k = 0; k < vacationRequestCount; k++)
                {
                    if (vacationRequest[k][0] == result[j] && vacationRequest[k][1] == i)
                    {
                        curSchedule[result[j]][i] = 0;
                    } 
                }
            
                // assign shift
                if (curSchedule[result[j]][i] == -1)
                {
                    curSchedule[result[j]][i] = bestShiftIdx;
                    workDays[result[j]]++;
                    int point = 0;
                    for (int k = 0; k < 24; k++)
                    {
                        if (shiftTime[bestShiftIdx][k] == 1 && demand[k] > 0)
                        {
                            demand[k]--;
                            point++;
                            demandCount--;
                        }
                    }
                    if (point < 7)
                    {
                        hasPoint = false;
                    }
                }
            }
            else
            {
                curSchedule[result[j]][i] = 0;
            }
        }
    }

    handleComeparePoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, workSchedule, vacationRequestCount, noVacationWeight, vacationRequest);
    handleRestart(curSchedule, workDays, dayCount, staffCount);
    
    // type 3
    int vacation[7][10] = {0}, vacationTypeCount = 0;
    for (int i = 0; i < 7; i++)
    {
        int count = 0, cur[10] = {0};
        
        for (int j = 0; j < 5; j++)
        {
            if (i + j * 7 < dayCount)
            {
                cur[count] = i + j * 7;
                count++;
            }

            if (i + j * 7 + 1 < dayCount)
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
        int count = 0, shift = (i % vacationTypeCount + 1 == vacationTypeCount) ? 0 :  i % vacationTypeCount + 1;
        for (int j = 0; j < vacationRequestCount; j++)
        {
            if (vacationRequest[j][0] == i)
            {
                curSchedule[i][vacationRequest[j][1]] = 0;
                count++;
            } 
        }

        for (int j = 0; j < vacationCount - count; j++)
        {
            
            curSchedule[i][vacation[shift][j]] = 0;
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
        handleSortStaff(workDays, result, staffCount, i, curSchedule, 1);

        for (int j = 0; j < staffCount; j++)
        {
            if (curSchedule[result[j]][i] == -1)
            {
                int nightShiftCount = getNightShiftCount(curSchedule, shiftTime, i, result[j]);
                int bestShift = getBestShift(shiftTime, shiftCount, demand, nightShiftCount, 0, overNightShiftWeight);

                handleShouldBreak(i, result[j], workDays, dayCount, vacationCount, curSchedule);
                
                if (curSchedule[result[j]][i] == -1)
                {
                    curSchedule[result[j]][i] = bestShift;
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

    handleComeparePoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, workSchedule, vacationRequestCount, noVacationWeight, vacationRequest);
    handleRestart(curSchedule, workDays, dayCount, staffCount);

    // type 4
    for (int i = 0; i < staffCount; i++)
    {
        int count = 0, shift = (i % vacationTypeCount + 1 == vacationTypeCount) ? 0 :  i % vacationTypeCount + 1;
        for (int j = 0; j < vacationRequestCount; j++)
        {
            if (vacationRequest[j][0] == i && count <= 2)
            {
                int yes = 1;
                for (int k = 0; k < 5; k++)
                {
                    if (vacationRequest[j][1] == resultD[k])
                    {
                        yes = 0;
                        break;
                    }
                }
                if (yes)
                {
                    curSchedule[i][vacationRequest[j][1]] = 0;
                    count++;
                }
            }
        }
        
        for (int j = 0; j < (vacationCount - count); j++)
        {
            curSchedule[i][vacation[shift][j]] = 0;
        }
    }
    for (int i = 0; i < dayCount; i++)
    {
        int demand[24] = {0};
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[resultD[i]][j];
        }

        for (int j = 0; j < staffCount; j++)
        {
            
            if (curSchedule[j][resultD[i]] == -1 || i <= 5)
            {
                handleShouldBreak(resultD[i], j, workDays, dayCount, vacationCount, curSchedule);
                int bestShift = getBestShift(shiftTime, shiftCount, demand, 0, 1, overNightShiftWeight);
                if (curSchedule[j][resultD[i]] == -1)
                {
                    curSchedule[j][resultD[i]] = bestShift;
                    workDays[j]++;
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
    
    handleComeparePoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, workSchedule, vacationRequestCount, noVacationWeight, vacationRequest);
    handleRestart(curSchedule, workDays, dayCount, staffCount);

    // type 5 (all shift contain night shift)
    int N = 0;
    for (int i = 0; i < shiftCount; i++)
    {
        for (int j = 18; j <= 23; j++)
        {
            if (shiftTime[i][j] == 1)
            {
                N += 1;
                break;
            }
        }
    }

    if (N == shiftCount - 1)
    {
        int demand[24] = {0};
        for (int i = 0; i < dayCount; i++)
        {
            for (int j = 0; j < 24; j++)
            {
                demand[j] = workerDemand[i][j];
            }
        }
        
        for (int i = 0; i < staffCount; i++)
        {
            for (int j = 0; j < vacationRequestCount; j++)
            {
                if (vacationRequest[j][0] == i)
                {
                    curSchedule[i][vacationRequest[j][1]] = 0;
                } 
            }
            
            for (int j = 0; j < dayCount; j++)
            {
                int bestShift = getBestShift(shiftTime, shiftCount, demand, 0, 1, overNightShiftWeight);
                
                if (curSchedule[i][j] != 0)
                {
                    curSchedule[i][j] = bestShift;
                }
                else
                {
                    curSchedule[i][j] = 0;
                }
            }
        }
    }

    handleComeparePoint(dayCount, staffCount, workerDemand, curSchedule, shiftTime, overNightShiftWeight, workSchedule, vacationRequestCount, noVacationWeight, vacationRequest);

    // output
    for (int i = 0; i < staffCount; i++)
    {
        for (int j = 0; j < dayCount; j++)
        {
            if (j != dayCount - 1)
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
