// Flat Engine Template
// Template for all Flat engine games
#include <fe/engine.hpp>
#include <fe/subsystems/gameState/gameStateMachine.hpp>
#include "gameplayState.hpp"
#include <fe/gui/guiPrefabricatedElements.hpp>
#include <fe/subsystems/resourceManager/resourceManager.hpp>

#if _DEBUG
int main()
#else
int WinMain()
#endif
    {
        fe::engine game;
        game.startUp(35_MiB, 25_MiB, 5_MiB);

        game.loadResources("resources.txt");

        game.getPrefabGui().loadElements("resources/gui/guiItems.xml");
        game.getPrefabGui().loadGUI("resources/gui/gui.xml");

        game.getResourceManager<sf::Font>()->load("Roboto-Regular.ttf", "default");

        game.getStateMachine().queuePush<gameplayState>();

        game.run();

        game.shutDown();
    }