// scoreHandler.hpp
// Handles the game score. The more coins collected the more score
#pragma once
#include <fe/time/clock.hpp>
#include <fe/subsystems/messaging/eventHandler.hpp>

class scoreHandler : public fe::eventHandler
    {
        private:
            fe::clock m_levelStart;
            unsigned int m_coinCollected;

            unsigned int m_score;

        public:
            void init();
            void deinit();

            void startLevel();
            void handleEvent(const fe::gameEvent &event);
            int endLevel();
    };
