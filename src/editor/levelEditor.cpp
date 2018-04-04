#include "levelEditor.hpp"
#include <fe/engine.hpp>
#include <fe/feAssert.hpp>
#include <fe/gui/panel.hpp>
#include <fe/gui/label.hpp>
#include <fe/gui/button.hpp>
#include <fe/gui/square.hpp>
#include <fe/gui/textBox.hpp>
#include <fe/gui/toggleButton.hpp>
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/subsystems/resourceManager/resourceManager.hpp>
#include <fe/engineEvents.hpp>
#include <fe/subsystems/messaging/gameEvent.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/subsystems/gameState/gameStateMachine.hpp>
#include <fe/subsystems/input/inputManager.hpp>
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>
#include <math.h>
#include <fe/debug/profiler.hpp>
#include <fe/subsystems/physics/collision/aabbTree.hpp>

#include "tilePlaceTool.hpp"
#include "entityPlaceTool.hpp"
#include "editorLoadTool.hpp"
#include "fileTool.hpp"
#include "../gameEvents.hpp"

void levelEditor::addTool(editorTool *tool)
    {
        m_futureTool = tool;
        m_newTool = true;
    }

void levelEditor::drawExtra(sf::RenderTarget &app)
    {
        app.draw(m_grid);
    }

void levelEditor::updateGrid()
    {
        m_grid.clear();
        fe::Vector2<int> mouseGridPos = alignToGrid(m_mousePositionWorld);

        m_grid.append(sf::Vertex({ (float)mouseGridPos.x,                       (float)mouseGridPos.y },                        sf::Color(255, 255, 255, 230)));
        m_grid.append(sf::Vertex({ (float)mouseGridPos.x + m_currentGridSize,   (float)mouseGridPos.y },                        sf::Color(255, 255, 255, 230)));

        m_grid.append(sf::Vertex({ (float)mouseGridPos.x + m_currentGridSize,   (float)mouseGridPos.y },                        sf::Color(255, 255, 255, 230)));
        m_grid.append(sf::Vertex({ (float)mouseGridPos.x + m_currentGridSize,   (float)mouseGridPos.y + m_currentGridSize },    sf::Color(255, 255, 255, 230)));

        m_grid.append(sf::Vertex({ (float)mouseGridPos.x + m_currentGridSize,   (float)mouseGridPos.y + m_currentGridSize },    sf::Color(255, 255, 255, 230)));
        m_grid.append(sf::Vertex({ (float)mouseGridPos.x,                       (float)mouseGridPos.y + m_currentGridSize },    sf::Color(255, 255, 255, 230)));

        m_grid.append(sf::Vertex({ (float)mouseGridPos.x,                       (float)mouseGridPos.y + m_currentGridSize },    sf::Color(255, 255, 255, 230)));
        m_grid.append(sf::Vertex({ (float)mouseGridPos.x,                       (float)mouseGridPos.y },                        sf::Color(255, 255, 255, 230)));

        if (m_gridSizeLabel)
            {
                m_gridSizeLabel->setString("Grid Size: " + std::to_string(m_currentGridSize));
            }
    }

levelEditor::levelEditor(fe::baseGameState &editWorld) :
    m_editingWorld(&editWorld.getGameWorld()),
    m_editState(editWorld),
    m_fileTool(nullptr),
    m_currentGridSize(64),
    m_mousePositionWindow(0.f, 0.f),
    m_mousePositionWorld(0.f, 0.f),
    m_mouseCoordGridLabel(nullptr),
    m_mouseCoordRealLabel(nullptr)
    {
    }

void levelEditor::init()
    {
        getGameWorld().setDynamicBroadphase(new fe::aabbTree);

        FE_ASSERT(m_editingWorld, "World to edit has been reallocated!");
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_closed"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("save_dialog_closed"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_tool_close"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_tile_select_open"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_texture_load_open"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_tile_fabrication_load_open"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_entity_place_open"));

        fe::gui::panel *mainPanel = fe::engine::get().getPrefabGui().getGUI("editor_gui_mainPanel");
        m_mouseCoordGridLabel = static_cast<fe::gui::label*>(mainPanel->getElement("editor_mouseCoordGridString"));
        m_mouseCoordRealLabel = static_cast<fe::gui::label*>(mainPanel->getElement("editor_mouseCoordRealString"));
        m_gridSizeLabel = static_cast<fe::gui::label*>(mainPanel->getElement("editor_gridSizeString"));
        addPanel(mainPanel);
        addPanel(fe::engine::get().getPrefabGui().getGUI("editor_gui_toolPanel"));

        m_firstKey = fe::engine::get().getInputManager().add(sf::Mouse::Left, fe::input([this](const sf::Event&) { for (auto &tool : m_currentTools) { tool->onLeftMouseClick(); } }, true, false));
        fe::engine::get().getInputManager().add(sf::Mouse::Left, fe::input([this](const sf::Event&) { for (auto &tool : m_currentTools) { tool->onLeftMouseRelease(); } }, false, true));
        fe::engine::get().getInputManager().add(sf::Mouse::Right, fe::input([this](const sf::Event&) { for (auto &tool : m_currentTools) { tool->onRightMouseClick(); } }, true, false));
        fe::engine::get().getInputManager().add(sf::Mouse::Right, fe::input([this](const sf::Event&) { for (auto &tool : m_currentTools) { tool->onRightMouseRelease(); } }, false, true));

        fe::engine::get().getInputManager().add(sf::Keyboard::Q, fe::input([this](const sf::Event&) { m_currentGridSize /= (m_currentGridSize != 1 ? 2 : 1); updateGrid(); }, false, false));
        fe::engine::get().getInputManager().add(sf::Keyboard::E, fe::input([this](const sf::Event&) { m_currentGridSize *= 2; updateGrid(); }, false, false));

        fe::engine::get().getInputManager().add(sf::Keyboard::W, fe::input([this](const sf::Event&) { getCamera().setVelocity(getCamera().getVelocity().x, -50.f); }, true, false));
        fe::engine::get().getInputManager().add(sf::Keyboard::A, fe::input([this](const sf::Event&) { getCamera().setVelocity(-50.f, getCamera().getVelocity().y); }, true, false));
        fe::engine::get().getInputManager().add(sf::Keyboard::S, fe::input([this](const sf::Event&) { getCamera().setVelocity(getCamera().getVelocity().x, 50.f); }, true, false));
        fe::engine::get().getInputManager().add(sf::Keyboard::D, fe::input([this](const sf::Event&) { getCamera().setVelocity(50.f, getCamera().getVelocity().y); }, true, false));

        fe::engine::get().getInputManager().add(sf::Keyboard::W, fe::input([this](const sf::Event&) { getCamera().setVelocity(getCamera().getVelocity().x, 0.f); }, false, true));
        fe::engine::get().getInputManager().add(sf::Keyboard::A, fe::input([this](const sf::Event&) { getCamera().setVelocity(0.f, getCamera().getVelocity().y); }, false, true));
        fe::engine::get().getInputManager().add(sf::Keyboard::S, fe::input([this](const sf::Event&) { getCamera().setVelocity(getCamera().getVelocity().x, 0.f); }, false, true));
        fe::engine::get().getInputManager().add(sf::Keyboard::D, fe::input([this](const sf::Event&) { getCamera().setVelocity(0.f, getCamera().getVelocity().y); }, false, true));

        fe::engine::get().getInputManager().add(sf::Mouse::Wheel::VerticalWheel, fe::input([this](const sf::Event &event) { 
            getCamera().zoom(event.mouseWheelScroll.delta * (-10 * std::sqrt(std::abs(getCamera().getZoom()))));
        }, false, false));

        fe::engine::get().getInputManager().add(sf::Keyboard::F1, fe::input([this](const sf::Event&)
        {
            if (m_fileTool)
                {
                    m_fileTool->destroy();
                    m_fileTool = nullptr;
                }
            else
                {
                    m_fileTool = new fileTool(this, m_editingWorld, fileTool::MODE::SAVE, m_lastFilepath.c_str());
                    addTool(m_fileTool);
                }
        }, false, false));
        m_lastKey = fe::engine::get().getInputManager().add(sf::Keyboard::F2, fe::input([this](const sf::Event&)
        {
            if (m_fileTool)
                {
                    m_fileTool->destroy();
                    m_fileTool = nullptr;
                }
            else
                {
                    m_fileTool = new fileTool(this, m_editingWorld, fileTool::MODE::LOAD, m_lastFilepath.c_str());
                    addTool(m_fileTool);
                }
        }, false, false));

        m_grid = sf::VertexArray(sf::PrimitiveType::Lines, 0);
        updateGrid();

        getCamera().setMaxSpeed(100.f);
    }

void levelEditor::deinit()
    {
        fe::engine::get().getEventSender().unsubscribeAll(this);

        for (fe::Handle i = m_firstKey; i <= m_lastKey; i++) 
            {
                fe::engine::get().getInputManager().remove(i);
            }
    }

void levelEditor::preUpdate()
    {
        if (m_currentGridSize <= 0)
            {
                FE_LOG_ERROR("Grid Size <= 0", m_currentGridSize);
                m_currentGridSize = 1;
            }

        if (m_newTool)
            {
                m_newToolIndex = m_currentTools.size() > 0 ? m_currentTools.size() - 1 : 0;
                m_currentTools.push_back(m_futureTool);
            }

        for (auto &it = m_currentTools.begin(); it != m_currentTools.end();)
            {
                if (!*it || (*it)->destroyed())
                    {
                        if (*it)
                            {
                                delete (*it);
                                (*it) = nullptr;
                            }
                        it = m_currentTools.erase(it);
                    }
                else
                    {
                        (*it)->update();
                        ++it;
                    }
            }

        if (m_newTool)
            {
                for (auto it = m_currentTools.begin() + m_newToolIndex; it != m_currentTools.end(); it++)
                    {
                        (*it)->initialize();
                    }
                m_newTool = false;
            }

        char gridBuffer[256];
        if (m_mouseCoordGridLabel)
            {
                std::sprintf(gridBuffer, "Grid Coord: (%d, %d)", (int)alignToGrid(m_mousePositionWorld).x, (int)alignToGrid(m_mousePositionWorld).y);
                m_mouseCoordGridLabel->setString(gridBuffer);
            }
        if (m_mouseCoordRealLabel)
            {
                std::sprintf(gridBuffer, "Real Coord: (%.2f, %.2f)", m_mousePositionWorld.x, m_mousePositionWorld.y);
                m_mouseCoordRealLabel->setString(gridBuffer);
            }
    }

void levelEditor::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("editor_closed"):
                    fe::engine::get().getStateMachine().queuePop();
                    break;
                case FE_STR("save_dialog_closed"):
                    static_cast<fileTool*>(m_fileTool)->setSave(true); // we do this so it wont save if you open a new tool or w/e
                    m_lastFilepath = static_cast<fileTool*>(m_fileTool)->getFilepath();
                    m_fileTool = nullptr;

                    fe::engine::get().getEventSender().send(fe::gameEvent(), LEVEL_SAVE_LOAD);
                    break;
                case FE_STR("editor_tile_select_open"):
                    addTool(new tilePlaceTool(this, m_editingWorld, m_currentGridSize, m_mousePositionWindow, m_mousePositionWorld));
                    break;
                case FE_STR("editor_entity_place_open"):
                    addTool(new entityPlaceTool(m_editState, this, m_editingWorld, m_currentGridSize, m_mousePositionWindow, m_mousePositionWorld));
                    break;
                case FE_STR("editor_texture_load_open"):
                    addTool(new editorLoadTool( this,
                                                m_editingWorld, 
                                                "editor_gui_textureLoadDialog",
                                                [this](const std::string &str) { m_editingWorld->getTileMap().addGlobalTexture(str); },
                                                FE_STR("texture_load_dialog_closed")));
                    break;
                case FE_STR("editor_tile_fabrication_load_open"):
                    addTool(new editorLoadTool( this,
                                                m_editingWorld,
                                                "editor_gui_tileFabricationLoadDialog",
                                                [this](const std::string &str) { m_editingWorld->getTileMap().loadFabrications(str.c_str()); },
                                                FE_STR("tile_fab_load_dialog_closed")));
                    break;
                default:
                    break;
            }
    }

void levelEditor::handleWindowEvent(const sf::Event &event)
    {
        switch (event.type)
            {
                case sf::Event::MouseMoved:
                    m_mousePositionWindow = fe::Vector2d(event.mouseMove.x, event.mouseMove.y);
                    m_mousePositionWorld = fe::engine::get().getRenderer().getRenderWindow().mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
                    updateGrid();
                    break;
                default:
                    break;
            }

        for (auto &tool : m_currentTools)
            {
                tool->handleEvent(event);
            }
    }

fe::Vector2d levelEditor::alignToGrid(const fe::Vector2d position)
    {
        fe::Vector2d finalGrid(0, 0);
        finalGrid.x = position.x < 0 ? (position.x - (m_currentGridSize + (unsigned int)position.x) % m_currentGridSize) : position.x - ((int)position.x % m_currentGridSize);
        finalGrid.y = position.y < 0 ? (position.y - (m_currentGridSize + (unsigned int)position.y) % m_currentGridSize) : position.y - ((int)position.y % m_currentGridSize);
        return finalGrid;
    }

levelEditor::~levelEditor()
    {}
