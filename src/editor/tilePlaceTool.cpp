#include "tilePlaceTool.hpp"
#include <fe/engine.hpp>
#include <fe/objectManagement/str.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/gameState/gameWorld.hpp>
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/subsystems/physics/collision/broadphaseAbstract.hpp>
#include <fe/gui/panel.hpp>
#include <fe/gui/button.hpp>
#include <fe/subsystems/resourceManager/resourceManager.hpp>
#include <fe/engineEvents.hpp>
#include <algorithm>
#include <math.h>
#include <SFML/Window/Event.hpp>
#include "levelEditor.hpp"

void tilePlaceTool::loadTileSelectionGUI(fe::gui::panel *tileSelectionPanel)
    {
        // how far across the grid will go
        unsigned int squareSize = std::ceilf(std::sqrtf((float)m_editingWorld->getTileMap().getFabrications().size()));

        const int speed = 1; // speed which tiles will scale to minSize
        const int maxSize = 48; // size of tiles at fabrications.size() == 1
        const int minSize = 16; // minimum size of tiles
        unsigned int selectionSize = std::max(minSize, (-speed * (int)m_editingWorld->getTileMap().getFabrications().size() + maxSize) + speed); 
        unsigned int currentTileX = 1;
        unsigned int currentTileY = 1;

        const int padding = 16;
        unsigned int sizeOfEditBox = (squareSize * selectionSize) + (padding * 2);

        tileSelectionPanel->setSize(fe::Vector2d(sizeOfEditBox, sizeOfEditBox));

        float positionSizeX = (float)selectionSize / std::max((float)sizeOfEditBox, tileSelectionPanel->getMinSize().x);
        float positionSizeY = (float)selectionSize / (float)sizeOfEditBox;

        for (auto &prefab : m_editingWorld->getTileMap().getFabrications())
            {
                fe::gui::guiElement *tileButton = fe::engine::get().getPrefabGui().getElement("editor_tileSelection");
                auto tile = m_editingWorld->getTileMap().getPrefabTile(FE_STR(prefab.id));

                tileButton->setTexture( &fe::engine::get().getResourceManager<sf::Texture>()->get(),
                                        m_editingWorld->getTileMap().getTileTextureOffset(FE_STR(prefab.id)) + m_editingWorld->getTileMap().getTextureOffset(),
                                        fe::Vector2d(tile->xSize, tile->ySize));

                tileButton->setSize(fe::Vector2d(selectionSize, selectionSize));

                static_cast<fe::gui::button*>(tileButton)->setPressCallback([this, tileButton, selectionSize, tile](){
                    fe::Vector2d offset = m_editingWorld->getTileMap().getTileTextureOffset(FE_STR(tile->id)) + m_editingWorld->getTileMap().getTextureOffset();
                    fe::Vector2d size = fe::Vector2d(tile->xSize, tile->ySize);
                    m_tileHighlight.entity->getRenderObject()->m_texCoords[0] = offset.x;
                    m_tileHighlight.entity->getRenderObject()->m_texCoords[1] = offset.y;
                    m_currentTileSize = size;
                    m_tileHighlight.entity->setSize(size);
                    m_tileHighlight.entity->enable(true);

                    sf::Color currentColour = sf::Color::White;
                    m_tileHighlight.entity->setColour(sf::Color(currentColour.r, currentColour.g, currentColour.b, 160));

                    m_tileHighlight.id = FE_STR(tile->id);
                    m_tileSelected = true;
                });

                tileSelectionPanel->setElementPosition(tileSelectionPanel->addElement(tileButton), fe::Vector2d(currentTileX * positionSizeX, currentTileY * positionSizeY));
                if (currentTileX >= squareSize)
                    {
                        currentTileX = 0;
                        currentTileY += 1;
                    }
                currentTileX += 1;
            }
    }

tilePlaceTool::tilePlaceTool(fe::baseGameState *state, fe::gameWorld *editWorld, int &gridSize, fe::Vector2d &mousePositionWindow, fe::Vector2d &mousePositionWorld) :
    editorTool(state, editWorld),
    m_tileSelectDialog(nullptr),
    m_currentTileSize(0, 0),
    m_currentTilePosition(0, 0),
    m_tileSelected(false),
    m_canPlace(false),
    m_deleted(false),
    m_currentGridSize(gridSize),
    m_mousePositionWindow(mousePositionWindow),
    m_mousePositionWorld(mousePositionWorld)
    {
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_tile_select_open"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_tile_select_close"));
        fe::engine::get().getEventSender().subscribe(this, fe::engineEvent::TILE_MAP_TEXTURE_CHANGED);

        m_tileSelectDialog = fe::engine::get().getPrefabGui().getGUI("editor_gui_tileSelection");
        loadTileSelectionGUI(m_tileSelectDialog);
        m_state->addPanel(m_tileSelectDialog);

        m_tileHighlight.entity = m_state->getObject(m_state->addObject("tileHighlight"));
    }

void tilePlaceTool::onLeftMouseClick()
    {
        if (m_canPlace && m_tileSelected && m_editingWorld->getTileMap().get(m_currentTilePosition) < 0)
            {
                fe::Handle tileHandle = m_editingWorld->getTileMap().add(fe::Vector2d(m_currentTilePosition), m_tileHighlight.id);
            }
    }

void tilePlaceTool::onLeftMouseRelease()
    {
    }

void tilePlaceTool::onRightMouseClick()
    {
        if (m_canPlace)
            {
                fe::Handle tile = m_editingWorld->getTileMap().get(m_mousePositionWorld);
                if (tile < 0 && !m_deleted)
                    {
                        m_deleted = false;
                        m_tileSelected = false;
                    }
                else if (tile >= 0)
                    {
                        m_deleted = true;
                        m_editingWorld->getTileMap().remove(tile);
                    }
                m_tileHighlight.entity->enable(false);
            }
    }

void tilePlaceTool::onRightMouseRelease()
    {
        m_deleted = false;
        m_tileHighlight.entity->enable(m_canPlace && m_tileSelected);
    }

void tilePlaceTool::update()
    {
        if (m_tileSelected)
            {
                fe::Vector2<int> tilePosition = static_cast<levelEditor*>(m_state)->alignToGrid(m_mousePositionWorld);
                m_currentTilePosition = tilePosition;
                m_tileHighlight.entity->setPosition(fe::Vector2d(tilePosition.x, tilePosition.y));
            }
    }

void tilePlaceTool::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("editor_tile_select_close"):
                    destroy();
                    break;
                case fe::engineEvent::TILE_MAP_TEXTURE_CHANGED:
                    m_tileSelectDialog->clearAllElements();
                    loadTileSelectionGUI(m_tileSelectDialog);
                    break;
                default:
                    break;
            }
    }

void tilePlaceTool::handleEvent(const sf::Event &event)
    {
        switch (event.type)
            {
                case sf::Event::MouseMoved:
                    m_canPlace = m_state->forEachPanel([](fe::gui::panel *panel) {
                        return !panel->mouseHover() || (panel->id() == FE_STR("editor_mainPanel") && !panel->mouseHoverToolbar());
                    });
                    break;
                default:
                    break;
            }
    }

tilePlaceTool::~tilePlaceTool()
    {
        m_tileSelectDialog->destroy();
        m_state->removePanel(m_tileSelectDialog);
        m_state->removeObject(m_tileHighlight.entity);
        fe::engine::get().getEventSender().unsubscribeAll(this);
    }
