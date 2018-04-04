// entityPlaceTool.hpp
// A tool to place entities in the world
#pragma once
#include <fe/math/Vector2.hpp>
#include <fe/gui/panel.hpp>
#include <fe/entity/baseEntity.hpp>
#include <vector>
#include "editorTool.hpp"

class entityPlaceTool : public editorTool
    {
        private:
            fe::baseGameState &m_editState;
            std::vector<std::string> m_prefabs;

            fe::gui::panel *m_placePanel;
            std::string m_selectedEntityString;

            fe::Vector2<int> m_entitySpawnPosition;

            fe::Vector2d &m_mousePositionWindow;
            fe::Vector2d &m_mousePositionWorld;

            std::vector<fe::Handle> m_placedColliderEntities;

            fe::baseEntity *m_placeEntity;
            fe::baseEntity *m_selectedEntity;
            fe::Handle m_selectedEntityCollider;
            bool m_entitySelected; // If an entity string has been selected
            bool m_canPlace;
            bool m_deleted;

            void addSelectionCollider(fe::Handle entHandle);

        public:
            entityPlaceTool(fe::baseGameState &editState, fe::baseGameState *state, fe::gameWorld *editWorld, int &gridSize, fe::Vector2d &mousePositionWindow, fe::Vector2d &mousePositionWorld);

            void initialize();

            void onLeftMouseClick();
            void onLeftMouseRelease();
            void onRightMouseClick();
            void onRightMouseRelease();

            void update();

            void handleEvent(const fe::gameEvent &event);
            void handleEvent(const sf::Event &event);

            ~entityPlaceTool();
    };