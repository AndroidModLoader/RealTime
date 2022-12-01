#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <time.h>


MYMODCFG(net.rusjj.realtime, GTA Real Time, 1.1.2, RusJJ)

// Savings
void* hGame;

// Game Vars
char *ms_nGameClockHours, *ms_nGameClockMinutes, *ms_nGameClockSeconds;
unsigned int *ms_nMillisecondsPerGameMinute;

// Hooks
DECL_HOOKv(ClockUpdate, void* self)
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    bool bTimeFine = (*ms_nGameClockHours == t->tm_hour) &&
                     (*ms_nGameClockMinutes == t->tm_min) &&
                     (*ms_nGameClockSeconds >= t->tm_sec - 1) && (*ms_nGameClockSeconds <= t->tm_sec + 1);
    
    if(!bTimeFine)
    {
        *ms_nGameClockHours = (char)(t->tm_hour);
        *ms_nGameClockMinutes = (char)(t->tm_min);
        *ms_nGameClockSeconds = (char)(t->tm_sec);
    }
    ClockUpdate(self);
    /*if(!bTimeFine)
    {
        *ms_nGameClockHours = (char)(t->tm_hour);
        *ms_nGameClockMinutes = (char)(t->tm_min);
        *ms_nGameClockSeconds = (char)(t->tm_sec);
    }*/
}
DECL_HOOKv(ClockInit, unsigned int msPerMin)
{
    ClockInit(60000); // I was so lazy to patch it so i just hooked it...
}
DECL_HOOKv(GenericLoad)
{
    GenericLoad();
    *ms_nMillisecondsPerGameMinute = 60000;
}

extern "C" void OnModLoad()
{
    hGame = dlopen("libGTASA.so", RTLD_LAZY);
    if(hGame == NULL)
    {
        hGame = dlopen("libGTAVC.so", RTLD_LAZY);
        HOOK(GenericLoad,        aml->GetSym(hGame, "_Z11GenericLoadv"));
    }

    HOOK(ClockUpdate,            aml->GetSym(hGame, "_ZN6CClock6UpdateEv"));
    HOOK(ClockInit,              aml->GetSym(hGame, "_ZN6CClock10InitialiseEj"));
    SET_TO(ms_nGameClockHours,   aml->GetSym(hGame, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(ms_nGameClockMinutes, aml->GetSym(hGame, "_ZN6CClock20ms_nGameClockMinutesE"));
    SET_TO(ms_nGameClockSeconds, aml->GetSym(hGame, "_ZN6CClock20ms_nGameClockSecondsE"));
    SET_TO(ms_nMillisecondsPerGameMinute, aml->GetSym(hGame, "_ZN6CClock29ms_nMillisecondsPerGameMinuteE"));
}
