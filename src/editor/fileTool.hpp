// fileTool.hpp
// handles loading/saving of files
#pragma once
#include "editorTool.hpp"
#include <string>

namespace fe
    {
        namespace gui
            {
                class panel;
            }
    }

class fileTool : public editorTool
    {
        public:
            enum class MODE
                {
                    SAVE,
                    LOAD
                };
        private:
            fe::gui::panel *m_dialog;
            MODE m_mode;

            bool m_save;

        public:
            fileTool(fe::baseGameState *state, fe::gameWorld *editWorld, MODE mode, const std::string &previousString = "\0");
            std::string getFilepath() const;
            void setSave(bool value);
            void handleEvent(const fe::gameEvent &event);
            ~fileTool();

    };