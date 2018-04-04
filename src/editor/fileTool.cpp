#include "fileTool.hpp"
#include <fe/engine.hpp>
#include <fe/gui/label.hpp>
#include <fe/gui/panel.hpp>
#include <fe/gui/textBox.hpp>
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/serializer/serializerID.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fstream>
#include <string>

fileTool::fileTool(fe::baseGameState *state, fe::gameWorld *editWorld, MODE mode, const std::string &previousString) :
    editorTool(state, editWorld),
    m_mode(mode),
    m_save(false)
    {
        fe::engine::get().getEventSender().subscribe(this, FE_STR("save_dialog_closed"));

        m_dialog = fe::engine::get().getPrefabGui().getGUI("editor_gui_saveDialog");
        std::string prev = previousString;
        switch (mode)
            {
                case fileTool::MODE::SAVE:
                    static_cast<fe::gui::textBox*>(m_dialog->getElement("editor_loadInput"))->setString(previousString);
                    m_dialog->setTitle("Save");
                    break;
                case fileTool::MODE::LOAD:
                    static_cast<fe::gui::textBox*>(m_dialog->getElement("editor_loadInput"))->setString(previousString);
                    m_dialog->setTitle("Load");
                    break;
                default:
                    break;
            }
        m_state->addPanel(m_dialog);
    }

std::string fileTool::getFilepath() const
    {
        return static_cast<fe::gui::textBox*>(m_dialog->getElement("editor_loadInput"))->getString();
    }

void fileTool::setSave(bool value)
    {
        m_save = value;
    }

void fileTool::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("save_dialog_closed"):
                    destroy();
                    break;
                default:
                    break;
            }
    }

fileTool::~fileTool()
    {   
        if (m_save)
            {
                std::string filepath = static_cast<fe::gui::textBox*>(m_dialog->getElement("editor_loadInput"))->getString();
                switch (m_mode)
                    {
                        case fileTool::MODE::SAVE:
                            {
                                std::ofstream out(filepath);
                                m_editingWorld->save(out);
                                out.close();
                            }
                            break;
                        case fileTool::MODE::LOAD:
                            {
                                std::ifstream in(filepath);
                                m_editingWorld->load(in);
                                in.close();
                            }
                            break;
                        default:
                            break;
                    }

            }
        m_dialog->destroy();
        m_dialog = nullptr;
        fe::engine::get().getEventSender().unsubscribeAll(this);
    }
