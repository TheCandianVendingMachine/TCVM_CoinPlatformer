// tilePlaceTool.hpp
// the tool to place tiles
#pragma once
#include "editorTool.hpp"
#include <fe/entity/baseEntity.hpp>

namespace fe
    {
        namespace gui
            {
                class panel;
            }
    }

class tilePlaceTool : public editorTool
    {
        private:
            struct tileHighlight
                {
                    fe::baseEntity *entity; // tile place representation
                    fe::str id; // tile ID
                };

            fe::gui::panel *m_tileSelectDialog;
            tileHighlight m_tileHighlight;

            fe::Vector2d m_currentTileSize;
            fe::Vector2<int> m_currentTilePosition;

            fe::Vector2d &m_mousePositionWindow;
            fe::Vector2d &m_mousePositionWorld;
            int &m_currentGridSize;

            bool m_tileSelected;
            bool m_canPlace;
            bool m_deleted;

            void loadTileSelectionGUI(fe::gui::panel *tileSelectionPanel);

        public:
            tilePlaceTool(fe::baseGameState *state, fe::gameWorld *editWorld, int &gridSize, fe::Vector2d &mousePositionWindow, fe::Vector2d &mousePositionWorld);

            void onLeftMouseClick();
            void onLeftMouseRelease();
            void onRightMouseClick();
            void onRightMouseRelease();

            void update();

            void handleEvent(const fe::gameEvent &event);
            void handleEvent(const sf::Event &event);

            ~tilePlaceTool();
    };