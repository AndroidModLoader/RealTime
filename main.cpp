#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <time.h>


MYMODCFG(net.rusjj.realtime, GTA Real Time, 1.0, RusJJ)
NEEDGAME(com.rockstargames.gtasa)

// Savings
uintptr_t pGTASA;
void* hGTASA;

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

extern "C" void OnModLoad()
{
    logger->SetTag("GTARealTime");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = dlopen("libGTASA.so", RTLD_LAZY);

    HOOK(ClockUpdate, aml->GetSym(hGTASA, "_ZN6CClock6UpdateEv"));
    SET_TO(ms_nGameClockHours, aml->GetSym(hGTASA, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(ms_nGameClockMinutes, aml->GetSym(hGTASA, "_ZN6CClock20ms_nGameClockMinutesE"));
}
