// gameEvents.hpp
// global game events
#pragma once
#include <fe/engineEvents.hpp>

enum gameEvents
    {
        LEVEL_SAVE_LOAD = fe::engineEvent::COUNT,
        LEVEL_ENDED,
        ALL_LEVELS_ENDED,
    };