#include "timer.h"
#include <utility>

Base_Timer::Base_Timer():cp{{steady_clock::now(),steady_clock::now()}}{}
Base_Timer::~Base_Timer() noexcept = default;
Base_Timer::Base_Timer(const Base_Timer&) = default;

steady_clock::duration Base_Timer::current() const noexcept
{
    return cp ? (steady_clock::now() - cp->first) : cp.error().first;
}

std::string Base_Timer::current_str(time_units tu) const noexcept
{
    return duration_conv(current(),tu);
}

steady_clock::time_point Base_Timer::point() const noexcept
{
    return steady_clock::now();
}

void Base_Timer::pause() noexcept
{
    //pause is achieved by transforming the status, thus only need to "pause" if it's not currently running
    if(cp)
    {
        cp = underlying_t{std::unexpect,current(),steady_clock::duration{}};
    }
}

void Base_Timer::resume() noexcept
{
    if(!cp)
    {
        cp = underlying_t{{steady_clock::now() - current(),steady_clock::now()}};
    }
}

bool Base_Timer::is_paused() const noexcept
{
    return !cp;
}

steady_clock::duration Base_Timer::lap() noexcept
{
    auto ret = cp ? (steady_clock::now() - cp->second) : steady_clock::duration{};
    if(cp)
    {
        cp->second = steady_clock::now();
    }
    return ret;
}

std::string Base_Timer::lap_str(time_units tu) noexcept
{
    return duration_conv(lap(),tu);
}

void Base_Timer::reset() noexcept
{
    cp = underlying_t{std::unexpect};
}

Base_Timer::operator bool() const noexcept
{
    return !is_paused();
}