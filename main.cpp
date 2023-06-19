#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <time.h>


MYMODCFG(net.rusjj.realtime, GTA Real Time, 1.2, RusJJ)

// Savings
void* hGame;
int lastDay;

// Game Vars
char *CurrentDay, *ms_nGameClockMonths, *ms_nGameClockDays, *ms_nGameClockHours, *ms_nGameClockMinutes, *ms_nGameClockSeconds;
unsigned int *ms_nMillisecondsPerGameMinute;
int *DaysPassed;

// Game Funcs
void (*IncrementStat)(uint16_t, float);

// Helpers
inline struct tm* Now()
{
    time_t tmp = time(NULL);
    return localtime(&tmp);
}

// Hooks
DECL_HOOKv(ClockUpdate_SA, void* self)
{
    auto now = Now();
    
    if(now->tm_yday != lastDay) IncrementStat(0x86, 1.0f);
    lastDay = now->tm_yday;

	*CurrentDay = now->tm_wday + 1;
    *ms_nGameClockMonths = now->tm_mon + 1;
	*ms_nGameClockDays = now->tm_mday;
	*ms_nGameClockHours = now->tm_hour;
    *ms_nGameClockMinutes = now->tm_min;
    *ms_nGameClockSeconds = now->tm_sec;
}
DECL_HOOKv(ClockUpdate_VC, void* self)
{
    auto now = Now();

    if(now->tm_yday != lastDay) ++(*DaysPassed);
    lastDay = now->tm_yday;

    *ms_nGameClockHours = now->tm_hour;
    *ms_nGameClockMinutes = now->tm_min;
    *ms_nGameClockSeconds = now->tm_sec;
}

extern "C" void OnModLoad()
{
    hGame = aml->GetLibHandle("libGTASA.so");
    if(hGame != NULL)
    {
        SET_TO(CurrentDay, aml->GetSym(hGame, "_ZN6CClock10CurrentDayE"));
        SET_TO(ms_nGameClockMonths, aml->GetSym(hGame, "_ZN6CClock19ms_nGameClockMonthsE"));
        SET_TO(ms_nGameClockDays, aml->GetSym(hGame, "_ZN6CClock17ms_nGameClockDaysE"));
        SET_TO(IncrementStat, aml->GetSym(hGame, "_ZN6CStats13IncrementStatEtf"));
        HOOK(ClockUpdate_SA, aml->GetSym(hGame, "_ZN6CClock6UpdateEv"));
    }
    else
    {
        hGame = aml->GetLibHandle("libGTAVC.so");
        if(hGame == NULL) hGame = aml->GetLibHandle("libR1.so");
        if(hGame != NULL)
        {
            HOOK(ClockUpdate_VC, aml->GetSym(hGame, "_ZN6CClock6UpdateEv"));
            SET_TO(DaysPassed, aml->GetSym(hGame, "_ZN6CStats10DaysPassedE"));
        }
        else
        {
            logger->Error("The game is not supported!");
            return;
        }
    }
    SET_TO(ms_nGameClockHours,   aml->GetSym(hGame, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(ms_nGameClockMinutes, aml->GetSym(hGame, "_ZN6CClock20ms_nGameClockMinutesE"));
    SET_TO(ms_nGameClockSeconds, aml->GetSym(hGame, "_ZN6CClock20ms_nGameClockSecondsE"));

    lastDay = Now()->tm_yday;
}
