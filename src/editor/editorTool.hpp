// editorTool.hpp
// base class for all editor tools
#pragma once
#include <fe/subsystems/messaging/eventHandler.hpp>

namespace fe
    {
        class gameWorld;
        class baseGameState;
    }

namespace sf
    {
        class Event;
    }

class editorTool : public fe::eventHandler
    {
        protected:
            fe::gameWorld *m_editingWorld;
            fe::baseGameState *m_state;
            bool m_destroy;
            
        public:
            editorTool(fe::baseGameState *state, fe::gameWorld *editWorld);

            virtual void initialize() {}
            virtual void deinitialize() {}

            virtual void onLeftMouseClick() {}
            virtual void onLeftMouseRelease() {}
            virtual void onRightMouseClick() {}
            virtual void onRightMouseRelease() {}

            virtual void update() {}

            virtual void handleEvent(const fe::gameEvent &event) {}
            virtual void handleEvent(const sf::Event &event) {}

            void destroy();
            bool destroyed() const;

            virtual ~editorTool() {}

    };