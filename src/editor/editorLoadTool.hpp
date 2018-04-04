// editorLoadTool.hpp
// A basic class to load stuff
#pragma once
#include "editorTool.hpp"
#include <vector>
#include <functional>
#include <utility>
#include <fe/objectManagement/str.hpp>

namespace fe
    {
        struct gameEvent;
        namespace gui
            {
                class panel;
            }
    }

class editorLoadTool : public editorTool
    {
        protected:
            fe::gui::panel *m_dialog;
            std::function<void(const std::string&)> m_funcOnClose;
            fe::str m_closeEvent;

        public:
            editorLoadTool( fe::baseGameState *state,
                            fe::gameWorld *editWorld,
                            const std::string &dialogName,
                            const std::function<void(const std::string&)> &funcOnClose,
                            fe::str closeEvent);

            void handleEvent(const fe::gameEvent &event);
            virtual ~editorLoadTool();
    };