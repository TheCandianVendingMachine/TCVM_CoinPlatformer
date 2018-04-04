#include "entityPlaceTool.hpp"
#include <fe/engine.hpp>
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/gui/button.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/subsystems/physics/collision/collisionWorld.hpp>
#include <SFML/Window/Event.hpp>
#include "levelEditor.hpp"
#include "../gameEvents.hpp"

void entityPlaceTool::addSelectionCollider(fe::Handle entHandle)
    {
        m_placedColliderEntities.push_back(m_state->addObject("colliderEntity"));

        fe::Handle colliderHandle = m_placedColliderEntities.back();

        m_state->getObject(colliderHandle)->setSize(m_editState.getObject(entHandle)->getSize());
        m_state->getObject(colliderHandle)->setPosition(m_editState.getObject(entHandle)->getPosition());

        m_state->getObject(colliderHandle)->getCollider()->m_metaData = reinterpret_cast<void*>(entHandle);
        m_state->getObject(colliderHandle)->getCollider()->m_collisionCallback = [this, colliderHandle](fe::collisionData&) {
            if (!m_selectedEntity) 
                {
                    m_placedColliderEntities.erase(std::remove(m_placedColliderEntities.begin(), m_placedColliderEntities.end(), colliderHandle), m_placedColliderEntities.end());
                    m_state->removeObject(colliderHandle);
                }
            else
                {
                    m_selectedEntityCollider = colliderHandle;
                }
        };
    }

entityPlaceTool::entityPlaceTool(fe::baseGameState &editState, fe::baseGameState *state, fe::gameWorld *editWorld, int &gridSize, fe::Vector2d &mousePositionWindow, fe::Vector2d &mousePositionWorld) :
    editorTool(state, editWorld),
    m_entitySelected(false),
    m_canPlace(false),
    m_deleted(false),
    m_placeEntity(nullptr),
    m_mousePositionWindow(mousePositionWindow),
    m_mousePositionWorld(mousePositionWorld),
    m_editState(editState),
    m_selectedEntity(nullptr),
    m_selectedEntityCollider(-1)
    {
        m_placePanel = fe::engine::get().getPrefabGui().getGUI("editor_gui_entityPlace");
        m_state->addPanel(m_placePanel);

        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_entity_select_close"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_entity_select"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("editor_entity_delete"));
        fe::engine::get().getEventSender().subscribe(this, LEVEL_SAVE_LOAD);

        float posY = 1.f;

        m_editState.getPrefabs(m_prefabs);

        int index = 0;
        for (auto entity : m_prefabs)
            {
                fe::gui::button *entityButton = static_cast<fe::gui::button*>(fe::engine::get().getPrefabGui().getElement("editor_entitySelect"));
                entityButton->setPressCallback([entity, this, entityButton]() {
                    m_selectedEntityString = entity;
                    m_entitySelected = true;
                    m_placeEntity->enable(true);
                });

                entityButton->setActiveColour(sf::Color::Red);
                entityButton->setInactiveColour(sf::Color::Transparent);

                fe::gui::label *entityText = static_cast<fe::gui::label*>(fe::engine::get().getPrefabGui().getElement("editor_entityTypeLabel"));
                entityText->setString(entity);
                entityText->setPixelSize(32.f);
                entityButton->setSize(entityText->getSize());

                entityText->setParent(entityButton);
                entityText->setPositionRelative(0.5f, 0.5f);

                m_placePanel->setElementPosition(m_placePanel->addElement(entityButton), fe::Vector2d(0.5f, (posY * (++index)) / 10.f));
                m_placePanel->addElement(entityText);
            }
        
        m_placeEntity = state->getObject(state->addObject("tileHighlight"));
        m_placeEntity->enable(false);
    }

void entityPlaceTool::initialize()
    {
        std::vector<fe::baseEntity*> allEntities;
        m_editState.getAllObjects(allEntities);
        for (auto &entity : allEntities)
            {
                addSelectionCollider(entity->getHandle());
            }
        m_entitySpawnPosition = static_cast<levelEditor*>(m_state)->alignToGrid(m_mousePositionWorld);
    }

void entityPlaceTool::onLeftMouseClick()
    {
        if (!m_selectedEntity)
            {
                fe::engine::get().getCollisionWorld().queryPoint(m_mousePositionWorld, FE_STR("editor_entity_select"));
            }
    }

void entityPlaceTool::onLeftMouseRelease()
    {
        if (m_canPlace && m_entitySelected)
            {
                fe::Handle entityHandle = m_editState.addObject(m_selectedEntityString.c_str());
                m_editState.getObject(entityHandle)->setPosition(m_entitySpawnPosition);
                addSelectionCollider(entityHandle);
            }
        else if (m_selectedEntity)
            {
                m_selectedEntity = nullptr;
                m_selectedEntityCollider = -1;
            }
    }

void entityPlaceTool::onRightMouseClick()
    {
        if (m_canPlace)
            {
                if (!m_deleted)
                    {
                        m_deleted = true;
                        fe::engine::get().getCollisionWorld().queryPoint(m_mousePositionWorld, FE_STR("editor_entity_delete"));
                    }

                m_placeEntity->enable(false);
            }
    }

void entityPlaceTool::onRightMouseRelease()
    {
        m_deleted = false;
        m_placeEntity->enable(m_canPlace && m_entitySelected);
    }

void entityPlaceTool::update()
    {
        if (m_entitySelected)
            {
                m_placeEntity->setPosition(fe::Vector2d(m_entitySpawnPosition.x, m_entitySpawnPosition.y));
            }

        m_canPlace = m_canPlace && !m_selectedEntity;
    }

void entityPlaceTool::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("editor_entity_select_close"):
                    m_prefabs.clear();
                    destroy();
                    break;
                case FE_STR("editor_entity_delete"):
                    {
                        fe::collider *affected = static_cast<fe::collider*>(event.args[0].arg.TYPE_VOIDP);
                        fe::Handle objHandle = reinterpret_cast<fe::Handle>(affected->m_metaData);

                        m_editState.removeObject(objHandle);
                    }
                    break;
                case FE_STR("editor_entity_select"):
                    {
                        fe::collider *affected = static_cast<fe::collider*>(event.args[0].arg.TYPE_VOIDP);
                        fe::Handle objHandle = reinterpret_cast<fe::Handle>(affected->m_metaData);

                        m_selectedEntity = m_editState.getObject(objHandle);
                    }
                    break;
                case LEVEL_SAVE_LOAD:
                    break;
                default:
                    break;
            }
    }

void entityPlaceTool::handleEvent(const sf::Event &event)
    {
        switch (event.type)
            {
                case sf::Event::MouseMoved:
                    m_canPlace = m_state->forEachPanel([](fe::gui::panel *panel) {
                        return !panel->mouseHover() || (panel->id() == FE_STR("editor_mainPanel") && !panel->mouseHoverToolbar());
                    });

                    m_entitySpawnPosition = static_cast<levelEditor*>(m_state)->alignToGrid(m_mousePositionWorld);

                    if (m_selectedEntity)
                        {
                            m_selectedEntity->setPosition(m_entitySpawnPosition);
                            m_state->getObject(m_selectedEntityCollider)->setPosition(m_selectedEntity->getPosition());
                        }
                    break;
                default:
                    break;
            }
    }

entityPlaceTool::~entityPlaceTool()
    {
        m_placePanel->destroy();
        fe::engine::get().getEventSender().unsubscribeAll(this);

        m_state->removeObject(m_placeEntity);
        for (auto &collider : m_placedColliderEntities)
            {
                m_state->removeObject(collider);
            }
    }
