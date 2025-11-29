#pragma once

#include <chrono>

class Clock {
    public:
        static void start();
        static double getTimeSinceStart();
        static float getDeltaTime();

    private:
        static std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        static std::chrono::time_point<std::chrono::steady_clock> m_last_time;
};