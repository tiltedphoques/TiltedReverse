#pragma once

struct SteamLauncher
{
    SteamLauncher();

protected:

    bool IsRunning() const;
    void Launch() const;
};