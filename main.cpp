#include <iostream>
using namespace std;

int getBestShift(const int shiftTime[30][24], const int shiftCount, int demand[24], int nightShiftCount);
int getNightShiftCount(const int workSchedule[100][31], const int shiftTime[30][24], const int day, const int staff);

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
        int demand[24], demandCount = 0;
        for (int j = 0; j < 24; j++)
        {
            demand[j] = workerDemand[i][j];
            demandCount += workerDemand[i][j];
        }

        if (i % 2 == 0)
        {
            for (int j = 0; j < staffCount; j++)
            {
                if (demandCount > 0)
                {
                    // total work
                    if (workDays[j] == dayCount - vacationCount)
                    {
                        workSchedule[j][i] = 0;
                    }

                    // conti work
                    if (i >= 6)
                    {
                        int contiWorkCount = 0;
                        for (int k = i - 6; k < i; k++)
                        {
                            if (workSchedule[j][k] != 0)
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
                            workSchedule[j][i] = 0;
                        }
                    }
                    
                    // nightnight shift check
                    int nightShiftCount = getNightShiftCount(workSchedule, shiftTime, i, j);
                    int bestShiftIdx = getBestShift(shiftTime, shiftCount, demand, nightShiftCount);
                    
                    // request
                    for (int k = 0; k < vacationRequestCount; k++)
                    {
                        if (vacationRequest[k][0] == j && vacationRequest[k][1] == i)
                        {
                            workSchedule[j][i] = 0;
                        } 
                    }
                    
                    // assign shift
                    if (workSchedule[j][i] == -1)
                    {
                        workSchedule[j][i] = bestShiftIdx;
                        workDays[j]++;
                        for (int k = 0; k < 24; k++)
                        {
                            if (demand[k] > 0)
                            {
                                demandCount--;
                            }
                            if (shiftTime[bestShiftIdx][k] == 1)
                            {
                                demand[k]--;
                            } 
                        }
                    } 
                }
                else
                {
                    workSchedule[j][i] = 0;
                }
            }
        }
        else
        {
            for (int j = staffCount-1; j >= 0; j--)
            {
                if (demandCount > 0)
                {
                    // total work
                    if (workDays[j] == dayCount - vacationCount)
                    {
                        workSchedule[j][i] = 0;
                    }

                    // conti work
                    if (i >= 6)
                    {
                        int contiWorkCount = 0;
                        for (int k = i - 6; k < i; k++)
                        {
                            if (workSchedule[j][k] != -1 && workSchedule[j][k] != 0)
                            {
                                contiWorkCount++;
                            }
                            else if (workSchedule[j][k] == 0)
                            {
                                break;
                            }
                        }

                        if (contiWorkCount >= 6)
                        {
                            workSchedule[j][i] = 0;
                        }
                    }
                    
                    // night shift check
                    int nightShiftCount = getNightShiftCount(workSchedule, shiftTime, i, j);
                    int bestShiftIdx = getBestShift(shiftTime, shiftCount, demand, nightShiftCount);
                    
                    // request
                    
                    for (int k = 0; k < vacationRequestCount; k++)
                    {
                        if (vacationRequest[k][0] == j && vacationRequest[k][1] == i)
                        {
                            workSchedule[j][i] = 0;
                        } 
                    }

                    
                    // assign shift
                    if (workSchedule[j][i] == -1)
                    {
                        workSchedule[j][i] = bestShiftIdx;
                        workDays[j]++;
                        for (int k = 0; k < 24; k++)
                        {
                            if (demand[k] > 0)
                            {
                                demandCount--;
                            }
                            if (shiftTime[bestShiftIdx][k] == 1)
                            {
                                demand[k]--;
                            } 
                        }
                    } 
                }
                else
                {
                    workSchedule[j][i] = 0;
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