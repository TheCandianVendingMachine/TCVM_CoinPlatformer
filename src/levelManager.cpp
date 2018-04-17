#include "levelManager.hpp"
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/subsystems/messaging/gameEvent.hpp>
#include <fe/engine.hpp>
#include <fe/objectManagement/str.hpp>
#include <fe/subsystems/gameState/gameWorld.hpp>
#include "gameEvents.hpp"

levelManager::levelManager(fe::gameWorld &gameWorld) :
    m_gameWorld(gameWorld),
    m_newLevel(false)
    {
    }

void levelManager::init()
    {
        fe::engine::get().getEventSender().subscribe(this, FE_STR("hit_exit"));
    }

void levelManager::deinit()
    {
        fe::engine::get().getEventSender().unsubscribeAll(this);
    }

void levelManager::update()
    {
        if (m_newLevel)
            {
                getLevel();
                fe::engine::get().getEventSender().send(fe::gameEvent(), gameEvents::LEVEL_ENDED);
                m_newLevel = false;
            }
    }

void levelManager::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("hit_exit"):
                    m_newLevel = true;
                    break;
                default:
                    break;
            }
    }

void levelManager::addLevel(const std::string &levelPath)
    {
        m_futureLevels.push(levelPath);
    }

const std::string &levelManager::getCurrentLevel() const
    {
        return m_currentLevel;
    }

void levelManager::getLevel()
    {
        if (m_futureLevels.empty())
            {
                m_gameWorld.load("");
                fe::engine::get().getEventSender().send(fe::gameEvent(), gameEvents::ALL_LEVELS_ENDED);
                return;
            }
        m_currentLevel = m_futureLevels.front();
        m_futureLevels.pop();
        m_gameWorld.load(m_currentLevel);
    }
