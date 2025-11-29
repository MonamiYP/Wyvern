#include "core/Clock.hpp"

std::chrono::time_point<std::chrono::steady_clock> Clock::m_start_time;
std::chrono::time_point<std::chrono::steady_clock> Clock::m_last_time;

void Clock::start() {
    m_start_time = std::chrono::steady_clock::now();
    m_last_time = m_start_time;
}

double Clock::getTimeSinceStart() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - m_start_time;
    return elapsed.count();
}

float Clock::getDeltaTime() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> delta = now - m_last_time;
    m_last_time = now;
    return delta.count();
}