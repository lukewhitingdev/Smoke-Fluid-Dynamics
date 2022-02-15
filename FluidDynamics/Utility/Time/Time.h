#pragma once
#include <Windows.h>

class TimeUtility
{
public:
    static float deltaTime;
    static float calculateDeltaTime()
    {
        // Update our time
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        deltaTime = (timeCur - timeStart) / 1000.0f;
        timeStart = timeCur;

        float FPS60 = 1.0f / 60.0f;
        static float cummulativeTime = 0;

        // cap the framerate at 60 fps 
        cummulativeTime += deltaTime;
        if (cummulativeTime >= FPS60) {
            cummulativeTime = cummulativeTime - FPS60;
        }
        else {
            return 0;
        }

        return deltaTime;
    }

    static float getDeltaTime() { return deltaTime; }
};