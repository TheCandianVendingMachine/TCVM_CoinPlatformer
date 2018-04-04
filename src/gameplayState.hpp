// gameplayState.hpp
// the gameplay state
#pragma once
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/messaging/eventHandler.hpp>

class gameplayState : public fe::baseGameState, public fe::eventHandler
    {
        private:
            fe::Handle m_inputStart;
            fe::Handle m_inputEnd;
            
        public:
            gameplayState();
            void init();

            void onActive();
            void onDeactive();

            void preUpdate();
            void handleEvent(const fe::gameEvent &event);
            ~gameplayState();
    };