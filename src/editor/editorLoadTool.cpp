#include "editorLoadTool.hpp"
#include <fe/engine.hpp>
#include <fe/gui/panel.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/messaging/gameEvent.hpp>
#include <string>

editorLoadTool::editorLoadTool( fe::baseGameState *state,
                                fe::gameWorld *editWorld,
                                const std::string &dialogName,
                                const std::function<void(const std::string&)> &funcOnClose,
                                fe::str closeEvent) :
    editorTool(state, editWorld),
    m_funcOnClose(funcOnClose),
    m_closeEvent(closeEvent)
    {

        fe::engine::get().getEventSender().subscribe(this, closeEvent);
        m_dialog = fe::engine::get().getPrefabGui().getGUI(dialogName.c_str());
        m_state->addPanel(m_dialog);
    }

void editorLoadTool::handleEvent(const fe::gameEvent &event)
    {
        if (event.eventType == m_closeEvent)
            {
                destroy();
            }
    }

editorLoadTool::~editorLoadTool()
    {
        auto element = m_dialog->getElement("editor_loadInput");
        if (element) 
            {
                m_funcOnClose(static_cast<fe::gui::textBox*>(element)->getString());
            }

        fe::engine::get().getEventSender().unsubscribeAll(this);
        m_dialog->destroy();
    }
