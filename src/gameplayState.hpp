// gameplayState.hpp
// the gameplay state
#pragma once
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/messaging/eventHandler.hpp>
#include <fe/math/Vector2.hpp>
#include <stack>
#include "scoreHandler.hpp"
#include "levelManager.hpp"

class gameplayState : public fe::baseGameState, public fe::eventHandler
    {
        private:
            fe::Handle m_inputStart;
            fe::Handle m_inputEnd;

            std::stack<std::pair<fe::Vector2d, fe::Vector2d>> m_collectedPositions;

            scoreHandler m_scoreHandler;
            levelManager m_levelManager;
            
        public:
            gameplayState();
            void init();

            void onActive();
            void onDeactive();

            void preUpdate();
            void handleEvent(const fe::gameEvent &event);
            ~gameplayState();
    };