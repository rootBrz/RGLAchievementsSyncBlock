#pragma once

#include <iostream>

using FnSteamClient = void* (*)();
using FnGetISteamUserStats = void* (*)(void*, int, int, const char*);
using FnGetHSteamUser = int(*)();
using FnGetHSteamPipe = int(*)();
using FnSetAchievement = bool(*)(void*, const char*);
using FnStoreStats = bool(*)(void*);

struct SteamCtx
{
    void* pUserStats = nullptr;
    FnSetAchievement fnSetAch = nullptr;
    FnStoreStats     fnStore = nullptr;
    bool             valid = false;

    bool Init()
    {
        if (valid) {
#ifdef _LOGGING
            std::cout << "[INFO] SteamAPI already resolved.\n";
#endif
            return true;
        }

        HMODULE hSteam = GetModuleHandleA("steam_api64.dll");
        if (!hSteam) return false;

        auto fnClient = reinterpret_cast<FnSteamClient>        (GetProcAddress(hSteam, "SteamClient"));
        auto fnGetStats = reinterpret_cast<FnGetISteamUserStats>  (GetProcAddress(hSteam, "SteamAPI_ISteamClient_GetISteamUserStats"));
        auto fnGetUser = reinterpret_cast<FnGetHSteamUser>       (GetProcAddress(hSteam, "SteamAPI_GetHSteamUser"));
        auto fnGetPipe = reinterpret_cast<FnGetHSteamPipe>       (GetProcAddress(hSteam, "SteamAPI_GetHSteamPipe"));

        if (!fnClient || !fnGetStats || !fnGetUser || !fnGetPipe) return false;

        void* pClient = fnClient();
        if (!pClient) return false;

        pUserStats = fnGetStats(pClient, fnGetUser(), fnGetPipe(),
            "STEAMUSERSTATS_INTERFACE_VERSION011");
        if (!pUserStats) return false;

        void** vtable = *reinterpret_cast<void***>(pUserStats);
        fnSetAch = reinterpret_cast<FnSetAchievement>(vtable[7]);
        fnStore = reinterpret_cast<FnStoreStats>    (vtable[10]);

        valid = true;
#ifdef _LOGGING
        std::cout << "[INFO] SteamAPI resolved.\n";
#endif
        return true;
    }

    bool Unlock(const char* achName) const
    {
        if (!valid) return false;
        if (fnSetAch(pUserStats, achName))
        {
            fnStore(pUserStats);
            return true;
        }
        return false;
    }
};

extern SteamCtx g_Steam;