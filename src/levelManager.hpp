// levelManager.hpp
// Manages the levels. The next level, current level, etc
#pragma once
#include <string>
#include <queue>
#include <fe/subsystems/messaging/eventHandler.hpp>

namespace fe 
    {
        class gameWorld;
    }

class levelManager : public fe::eventHandler
    {
        private:
            fe::gameWorld &m_gameWorld;

            std::queue<std::string> m_futureLevels;
            std::string m_currentLevel;

            bool m_newLevel;

        public:
            levelManager(fe::gameWorld &gameWorld);

            void init();
            void deinit();

            void update();

            void handleEvent(const fe::gameEvent &event);

            void addLevel(const std::string &levelPath);
            const std::string &getCurrentLevel() const;
            void getLevel();

    };
