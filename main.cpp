#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <time.h>


MYMODCFG(net.rusjj.realtime, GTA Real Time, 1.1.1, RusJJ)

// Savings
void* hGame;

// Game Vars
char *ms_nGameClockHours, *ms_nGameClockMinutes;

// Hooks
DECL_HOOK(void, ClockUpdate, void* self)
{
    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    *ms_nGameClockHours = (char)(tm_struct->tm_hour);
    *ms_nGameClockMinutes = (char)(tm_struct->tm_min);
    
    ClockUpdate(self);
    
    *ms_nGameClockHours = (char)(tm_struct->tm_hour);
    *ms_nGameClockMinutes = (char)(tm_struct->tm_min);
}
DECL_HOOK(void, ClockInit, unsigned int msPerMin)
{
    ClockInit(60000); // I was so lazy to patch it so i just hooked it...
}

extern "C" void OnModLoad()
{
    hGame = dlopen("libGTASA.so", RTLD_LAZY);
    if(hGame == NULL) hGame = dlopen("libGTAVC.so", RTLD_LAZY);

    HOOK(ClockUpdate, aml->GetSym(hGame, "_ZN6CClock6UpdateEv"));
    HOOK(ClockInit, aml->GetSym(hGame, "_ZN6CClock10InitialiseEj"));
    SET_TO(ms_nGameClockHours, aml->GetSym(hGame, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(ms_nGameClockMinutes, aml->GetSym(hGame, "_ZN6CClock20ms_nGameClockMinutesE"));
}
