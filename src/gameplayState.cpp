#include "gameplayState.hpp"
#include "editor/levelEditor.hpp"
#include <fe/engine.hpp>
#include <fe/engineEvents.hpp>
#include <fe/subsystems/resourceManager/resourceManager.hpp>
#include <fe/subsystems/gameState/gameStateMachine.hpp>
#include <fe/subsystems/physics/collision/aabbTree.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/entity/baseEntity.hpp>
#include <fe/subsystems/scripting/scriptManager.hpp>
#include <fe/subsystems/physics/physicsEngine.hpp>
#include <sstream>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <fe/subsystems/input/inputManager.hpp>

gameplayState::gameplayState() :
    m_levelManager(getGameWorld())
    {
        fe::engine::get().getEventSender().subscribe(this, fe::engineEvent::TILE_PLACED);
        fe::engine::get().getEventSender().subscribe(this, fe::engineEvent::TILE_REMOVED);
    }

void gameplayState::init()
    {
        getGameWorld().setDynamicBroadphase(new fe::aabbTree);
        //getGameWorld().getDynamicBroadphase()->debugMode(true);

        addPrefab("player");
        addPrefab("coin");
        addPrefab("world_exit");

        m_levelManager.addLevel("level00");
        m_levelManager.addLevel("level01");
        
        m_levelManager.getLevel();

        fe::engine::get().getPhysicsEngine().setGravityZ(0.f);
        fe::engine::get().getPhysicsEngine().setGravityY(500.f);
        fe::engine::get().getPhysicsEngine().setGravityX(0.f);
        fe::engine::get().getPhysicsEngine().setFrictionImpulse(50.f);
    }

void gameplayState::onActive()
    {
        m_inputStart = fe::engine::get().getInputManager().add(sf::Keyboard::F1, fe::input([this](const sf::Event&){
            fe::engine::get().getStateMachine().queuePush<levelEditor>(fe::gameStateMachine::stateOptions::RENDER_OVERTOP, *this);
        }, false, false));

        m_inputEnd = m_inputStart + 1;

        m_scoreHandler.init();
        m_scoreHandler.startLevel();

        m_levelManager.init();

        fe::engine::get().getEventSender().subscribe(this, FE_STR("coin_collected"));
        fe::engine::get().getEventSender().subscribe(this, FE_STR("revert_position"));
    }

void gameplayState::onDeactive()
    {
        for (fe::Handle i = m_inputStart; i < m_inputEnd; i++)
            {
                fe::engine::get().getInputManager().remove(i);
            }

        m_scoreHandler.deinit();
        m_levelManager.deinit();

        fe::engine::get().getEventSender().unsubscribe(this, FE_STR("coin_collected"));
        fe::engine::get().getEventSender().unsubscribe(this, FE_STR("revert_position"));
    }

void gameplayState::preUpdate()
    {
        int i = 0;
    }

void gameplayState::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case fe::engineEvent::TILE_PLACED:
                    getGameWorld().getDynamicBroadphase()->add(static_cast<fe::imp::tileWorld*>(event.args[0].arg.TYPE_VOIDP)->colliderPtr);
                    break;
                case fe::engineEvent::TILE_REMOVED:
                    getGameWorld().getDynamicBroadphase()->remove(static_cast<fe::imp::tileWorld*>(event.args[0].arg.TYPE_VOIDP)->colliderPtr);
                    break;
                case FE_STR("coin_collected"): 
                    {   
                        fe::collider *collider = static_cast<fe::collider*>(event.args[1].arg.TYPE_VOIDP);
                        fe::collider *coin = static_cast<fe::collider*>(event.args[0].arg.TYPE_VOIDP);
                        m_collectedPositions.push(std::make_pair(
                            fe::Vector2d(collider->m_aabb.m_globalPositionX - 5.f, collider->m_aabb.m_globalPositionY - 5.f),
                            fe::Vector2d(coin->m_aabb.m_globalPositionX, coin->m_aabb.m_globalPositionY))
                        );
                    }
                    break;
                case FE_STR("revert_position"):
                    {
                        if (m_collectedPositions.empty()) break;
                        sol::object *solObj = static_cast<sol::object*>(event.args[0].arg.TYPE_VOIDP);
                        fe::baseEntity *obj = solObj->as<fe::scriptObject*>()->getBaseEntity();
                        obj->setPosition(m_collectedPositions.top().first);
                        obj->getRigidBody()->setForce(0, 0);
                        obj->getRigidBody()->setVelocity(0, 0);

                        fe::Handle coin = addObject("coin");
                        getObject(coin)->setPosition(m_collectedPositions.top().second);

                        m_collectedPositions.pop();
                    }
                break;
                default:
                    break;
            }
    }

gameplayState::~gameplayState()
    {
        fe::engine::get().getEventSender().unsubscribeAll(this);
    }
