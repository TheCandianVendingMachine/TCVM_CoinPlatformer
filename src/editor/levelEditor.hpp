// levelEditor.hpp
// edits the selected game world
#pragma once
#include <fe/subsystems/gameState/gameState.hpp>
#include <fe/subsystems/gameState/gameWorld.hpp>
#include <fe/subsystems/messaging/eventHandler.hpp>
#include <fe/entity/baseEntity.hpp>
#include <string>
#include <vector>
#include <SFML/Graphics/VertexArray.hpp>
#include "editorTool.hpp"

namespace fe
    {
        namespace gui
            {
                class panel;
                class label;
            }
    }

class levelEditor : public fe::baseGameState, public fe::eventHandler
    {
        private:
            fe::gameWorld *m_editingWorld;
            fe::baseGameState &m_editState;
            std::string m_lastFilepath;

            editorTool *m_futureTool;
            editorTool *m_fileTool;
            std::vector<editorTool*> m_currentTools;

            fe::gui::label *m_mouseCoordRealLabel;
            fe::gui::label *m_mouseCoordGridLabel;
            fe::gui::label *m_gridSizeLabel;

            fe::Handle m_firstKey;
            fe::Handle m_lastKey;

            sf::VertexArray m_grid;

            int m_currentGridSize;
            
            fe::Vector2d m_mousePositionWindow;
            fe::Vector2d m_mousePositionWorld;

            unsigned int m_newToolIndex;
            bool m_newTool;

            void addTool(editorTool *tool);
            void drawExtra(sf::RenderTarget &app);
            void updateGrid();

        public:
            levelEditor(fe::baseGameState &editWorld);
            void init();
            void deinit();
            void preUpdate();

            void handleEvent(const fe::gameEvent &event);
            void handleWindowEvent(const sf::Event &event);

            fe::Vector2d alignToGrid(const fe::Vector2d position);

            ~levelEditor();

    };