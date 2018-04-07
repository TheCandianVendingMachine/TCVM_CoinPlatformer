#include "levelManager.hpp"
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/subsystems/messaging/gameEvent.hpp>
#include <fe/engine.hpp>
#include <fe/objectManagement/str.hpp>
#include <fe/subsystems/gameState/gameWorld.hpp>

levelManager::levelManager(fe::gameWorld &gameWorld) :
    m_gameWorld(gameWorld)
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

void levelManager::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("hit_exit"):
                    getLevel();
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
                return;
            }
        m_currentLevel = m_futureLevels.front();
        m_futureLevels.pop();
        m_gameWorld.load(m_currentLevel);
    }
