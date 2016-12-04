// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Timer.h"


namespace
{
    double g_seconds_per_tick = 0;

    bool g_initialized = false;
};

void timer::initialize()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    assert(freq.QuadPart != 0); // Make sure system actually supports high-res counter
    g_seconds_per_tick = 1.0 / (double)freq.QuadPart;

    g_initialized = true;
}

uint64_t timer::tick_count()
{
    assert(g_initialized);
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return counter.QuadPart;
}
double timer::seconds()
{
    assert(g_initialized);
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return double(counter.QuadPart) * g_seconds_per_tick;
}

