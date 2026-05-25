// #define _LOGGING

#include "script.h"
#include "steam.h"

#include <cstdio>
#include <iostream>

SteamCtx g_Steam;
struct AchInfo { int id; const char* name; };

static constexpr std::pair<const char*, AchInfo> kAchTable[] =
{
    {"ACH_CHP1",         { 1,  "Back in the Mud"       }},
    {"ACH_WNT2",         { 2,  "Just a Scratch"        }},
    {"ACH_CHP2",         { 3,  "To Greener Pastures"   }},
    {"ACH_CHP3",         { 4,  "Settling Feuds"        }},
    {"ACH_CHP4",         { 5,  "Washed Ashore"         }},
    {"ACH_CHP5",         { 6,  "No Traitors"           }},
    {"ACH_DST5",         { 7,  "Third Time Lucky"      }},
    {"ACH_FIN1",         { 8,  "Redemption"            }},
    {"ACH_BE22",         { 9,  "Cowboy Builder"        }},
    {"ACH_ENDLESS",      {10,  "Endless Summer"        }},
    {"ACH_LEND_HAND",    {11,  "Lending a Hand"        }},
    {"ACH_BESTWEST",     {12,  "Best in the West"      }},
    {"ACH_GOLDRUSH",     {13,  "Gold Rush"             }},
    {"ACH_FWB",          {14,  "Friends With Benefits" }},
    {"ACH_MINIGAMES",    {15,  "Hobby Horse"           }},
    {"ACH_B_AND_E",      {16,  "Breaking and Entering" }},
    {"ACH_RCKPT",        {17,  "Artificial Intelligence"}},
    {"ACH_FROM_RICH",    {18,  "Take From the Rich"    }},
    {"ACH_TO_POOR",      {19,  "Give to the Poor"      }},
    {"ACH_UPGRADE_C",    {20,  "Pony Up"               }},
    {"ACH_HONOR",        {21,  "Extreme Personality"   }},
    {"ACH_RC",           {22,  "Western Stranger"      }},
    {"ACH_BOUNTY",       {23,  "Bountiful"             }},
    {"ACH_COLLECT",      {24,  "Collector's Item"      }},
    {"ACH_RESPECT",      {25,  "Paying Respects"       }},
    {"ACH_COMP_REQS",    {26,  "Errand Boy"            }},
    {"ACH_ART",          {27,  "It's Art"              }},
    {"ACH_CRAFT",        {28,  "Self Sufficient"       }},
    {"ACH_SKIN",         {29,  "Skin Deep"             }},
    {"ACH_ZOOLOGIST",    {30,  "Zoologist"             }},
    {"ACH_FISH",         {31,  "It was THIS Big!"      }},
    {"ACH_UPGR_W",       {32,  "Locked and Loaded"     }},
    {"ACH_BEAR",         {33,  "Grin and Bear it"      }},
    {"ACH_MP_BONDING",   {34,  "Trusty Steed"          }},
    {"ACH_MP_INTRO",     {35,  "Breakout"              }},
    {"ACH_MP_SERIES",    {36,  "Series Major"          }},
    {"ACH_MP_GUN4HIRE",  {37,  "Gun For Hire"          }},
    {"ACH_MP_FREEMODE",  {38,  "Eventful"              }},
    {"ACH_MP_BUCKLE",    {39,  "Buckle Up"             }},
    {"ACH_MP_MVP",       {40,  "The Real Deal"         }},
    {"ACH_MP_HORSES",    {41,  "Horses for Courses"    }},
    {"ACH_MP_RANK10",    {42,  "Getting Started"       }},
    {"ACH_MP_RANK50",    {43,  "Notorious"             }},
    {"ACH_MP_COUNTER",   {44,  "All's Fair"            }},
    {"ACH_MP_HOME",      {45,  "Home Comforts"         }},
    {"ACH_MP_CRAFTAMMO", {46,  "Non-Regulation"        }},
    {"ACH_MP_PERSISTENT",{47,  "Posse Up"              }},
    {"ACH_MP_CRAFT",     {48,  "Master Craftsman"      }},
    {"ACH_MP_BUTCHER",   {49,  "Butchered"             }},
    {"ACH_MP_HERBS",     {50,  "Picked to Perfection"  }},
    {"ACH_MP_POSSE",     {51,  "Strength in Numbers"   }},
};

static constexpr size_t kAchCount = std::size(kAchTable);

static uint32_t g_Hash[kAchCount];
static int      g_LastProg[kAchCount];
static bool     g_Done[kAchCount];

void main()
{
#ifdef _LOGGING
    InitConsole();
#endif

    if (!g_Steam.Init()) {
#ifdef _LOGGING
        std::cout << "[WARN] Steam init failed – achievements won't sync.\n";
#endif
    }

    const Hash spAch = GAMEPLAY::GET_HASH_KEY("SP_ACHIEVEMENTS");

    for (size_t i = 0; i < kAchCount; ++i)
    {
        g_Hash[i] = GAMEPLAY::GET_HASH_KEY(kAchTable[i].first);
        g_LastProg[i] = -1;
        g_Done[i] = false;
    }

    while (true)
    {
        for (size_t i = 0; i < kAchCount; ++i)
        {
            const uint32_t hash = g_Hash[i];
            const AchInfo& info = kAchTable[i].second;

            const int  progress = STATS::CHAL_ACHIEVEMENT_GET_PROGRESS_INT(spAch, hash);
            const bool unlocked = STATS::CHAL_ACHIEVEMENT_IS_COMPLETE(spAch, hash);

#ifdef _LOGGING
            if (g_LastProg[i] == -1)
            {
                g_LastProg[i] = progress;
                std::cout << "[INIT]     " << info.name << " = " << progress << '\n';
            }
            else if (progress != g_LastProg[i])
            {
                std::cout << "[PROGRESS] " << info.name
                    << " : " << g_LastProg[i] << " -> " << progress << '\n';
                g_LastProg[i] = progress;
            }
#else
            if (g_LastProg[i] == -1 || progress != g_LastProg[i])
                g_LastProg[i] = progress;
#endif

            if (unlocked && !g_Done[i])
            {
                g_Done[i] = true;

                char achName[24];
                sprintf_s(achName, "Achievement%d", info.id);

                if (g_Steam.Unlock(achName)) {
#ifdef _LOGGING
                    std::cout << "[UNLOCKED+STEAM] " << info.id
                        << " - " << info.name
                        << " (" << kAchTable[i].first << ")"
                        << " -> " << achName << '\n';
#endif
                }
                else {
#ifdef _LOGGING
                    std::cout << "[UNLOCKED] " << info.id
                        << " - " << info.name
                        << " (Steam sync failed)\n";
#endif
                }
            }
        }

        WAIT(1000);
    }
}

void ScriptMain()
{
    srand(GetTickCount());
    main();
}

static void InitConsole()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$", "r", stdin);
    SetConsoleTitleA("RDR2 Achievement Logger");
    std::cout << "[INFO] Console initialised.\n";
}