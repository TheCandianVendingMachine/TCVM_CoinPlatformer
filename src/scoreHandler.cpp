#include "scoreHandler.hpp"
#include <fe/subsystems/messaging/gameEvent.hpp>
#include <fe/subsystems/messaging/eventSender.hpp>
#include <fe/objectManagement/str.hpp>
#include <fe/engine.hpp>

void scoreHandler::init()
    {
        fe::engine::get().getEventSender().subscribe(this, FE_STR("coin_collected"));
    }

void scoreHandler::deinit()
    {
        fe::engine::get().getEventSender().unsubscribeAll(this);
    }

void scoreHandler::startLevel()
    {
        m_score = 0;
        m_coinCollected = 0;
        m_levelStart.restart();
    }

void scoreHandler::handleEvent(const fe::gameEvent &event)
    {
        switch (event.eventType)
            {
                case FE_STR("coin_collected"):
                    m_coinCollected++;
                    break;
                default:
                    break;
            }
    }

int scoreHandler::endLevel()
    {
        m_score += m_coinCollected;

        return m_score;
    }
