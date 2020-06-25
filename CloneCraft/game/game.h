#pragma once
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include "../glu/oldFunctions.h"
#include "../world/blocks.h"
#include "../world/chunks.h"
#include "../player/player.h"
#include "../maths/maths.h"
#include "../world/world.h"
#include "../ui/ui.h"
#include "../ui/gui.h"
#include "../player/playerWorldInteraction.h"
#include "../input/input.h"
#include "../player/playerControls.h"
#include "../world/blockInfo.h"

struct Game {
	Game(input::InputManager& inputManager);

	bool shouldUpdateWindow = false;
	bool isFullscreenEnabled = false;

	world::World gameWorld = world::World();

	player::Player player;
	maths::Vec3<int> lastChunkUpdatePosition;
	std::ostringstream debugInfoStream;

	ui::Text debugText = ui::Text("", "Debug", ui::fonts::dos, sf::Color::White, 1, 0, 17);
	ui::Text debugInfoText = ui::Text("", "", ui::fonts::dos, sf::Color::White, 1, 25, 14);

	ui::Button backToGameButton = ui::Button("pause", 1, 190, 500, 34,
		sf::Color(0, 0, 0, 125), "Back to game", ui::fonts::dos, sf::Color::White, 30,
		[this]() { this->inputManager.resetAllTo(false); this->guiManager.setNoGui(); });
	ui::Button optionsButton = ui::Button("pause", 1, 250, 500, 34,
		sf::Color(0, 0, 0, 125), "Options", ui::fonts::dos, sf::Color::White, 30,
		[this]() { this->inputManager.resetAllTo(true); this->guiManager.setGuiByName("options"); });
	ui::Button backToMainMenuButton = ui::Button("pause", 1, 310, 500, 34,
		sf::Color(0, 0, 0, 125), "(Save) and quit to main menu", ui::fonts::dos, sf::Color::White, 30,
		[this]()
		{
			//this->gameWorld.save();
			this->inputManager.resetAllTo(false); this->guiManager.setGuiByName("main_menu");
		});

	ui::Button backToPauseGuiButton = ui::Button("options", 1, 480, 500, 34,
		sf::Color(0, 0, 0, 125), "Back", ui::fonts::dos, sf::Color::White, 30,
		[this]()
		{
			this->guiManager.textFieldManager.clearTextFields("options");
			this->inputManager.resetAllTo(false);
			this->guiManager.setGuiByName("pause");
		});
	ui::Button saveWorldButton = ui::Button("options", 1, 190, 500, 34,
		sf::Color(0, 0, 0, 125), "Save world", ui::fonts::dos, sf::Color::White, 30,
		[this]() { this->gameWorld.save(); });
	ui::TextField mouseSensitivityTextInput = ui::TextField("options", 200, 240, 300, 34, sf::Color(0, 0, 0, 125), sf::Color(255, 255, 255, 125), ui::fonts::dos, sf::Color::White, 30);
	ui::Text      mouseSensitivityText = ui::Text("options", "Mouse Sensitivity: ", ui::fonts::dos, sf::Color::White, 1, 245, 20);
	ui::Button    mouseSensitivityOKButton = ui::Button("options", 505, 240, 60, 34, sf::Color(0, 0, 0, 125), "OK!", ui::fonts::dos, sf::Color::White, 30, 
		[this]()
		{
			this->player.mouseSensitivity = static_cast<float>(std::stod(this->mouseSensitivityTextInput.text));
		}
	);
	ui::Button toggleFullscreenButton = ui::Button("options", 1, 310, 500, 34,
		sf::Color(0, 0, 0, 125), "Toggle fullscreen", ui::fonts::dos, sf::Color::White, 30,
		[this]()
		{
			this->isFullscreenEnabled = !this->isFullscreenEnabled;
			this->shouldUpdateWindow = true;
		});

	ui::Text seedInfoText = ui::Text("main_menu", "Seed:", ui::fonts::dos, sf::Color::White, 1, 120, 30);
	ui::TextField generatedWorldSeedTextField = ui::TextField("main_menu", 1, 155, 420, 34, sf::Color(0, 0, 0, 125), sf::Color(255, 255, 255, 125), ui::fonts::dos, sf::Color::White, 30);
	ui::Button generateWorldButton = ui::Button("main_menu", 431, 155, 280, 34,
		sf::Color(0, 0, 0, 125), "Generate world", ui::fonts::dos, sf::Color::White, 30,
		[this]()
		{
			/*this->gameWorld.generateNewWorldWithNameAndSeed(this->worldNameTextField.text, this->generatedWorldSeedTextField.text);*/
			if (this->worldNameTextField.text.empty())
			{
				std::cout << "World can't have a namen't.\n";
				return;
			}
			std::cout << "Generatingn't world with seed \"" << this->generatedWorldSeedTextField.text
				<< "\" and with name \"" << this->worldNameTextField.text << "\"\n";
			this->player.position = maths::Vec3<float>(0, 20, 0);
			this->guiManager.setNoGui();
			this->guiManager.textFieldManager.clearTextFields("main_menu");
			this->inputManager.resetAllTo(false);
		});
	
	ui::Text worldNameInfoText = ui::Text("main_menu", "World name:", ui::fonts::dos, sf::Color::White, 1, 20, 30);
	ui::Button loadWorldButton = ui::Button("main_menu", 431, 55, 280, 34,
		sf::Color(0, 0, 0, 125), "Load world", ui::fonts::dos, sf::Color::White, 30,
		[this]()
		{
			std::cout << "Loadingn't world with name \"" << this->worldNameTextField.text << "\"\n";
			/*if (this->gameWorld.loadWorldWithName(this->worldNameTextField.text) && */
			/*(Probably) this->player.loadDataByWorldName(this->worldNameTextField.text))*/
			//{
			this->guiManager.setNoGui();
			this->guiManager.textFieldManager.clearTextFields("main_menu");
			this->inputManager.resetAllTo(false);
			//}
		});
	ui::TextField worldNameTextField = ui::TextField("main_menu", 1, 55, 420, 34, sf::Color(0, 0, 0, 125), sf::Color(255, 255, 255, 125), ui::fonts::dos, sf::Color::White, 30);

	ui::Rect simpleBackgroundRect = ui::Rect("", 0, 0, 0, 0, sf::Color(0, 0, 0, 125));
	ui::Rect darkerSimpleBackgroundRect = ui::Rect("", 0, 0, 0, 0, sf::Color(0, 0, 0, 195));

	ui::Rect crosshairRectangle1 = ui::Rect("", 0, 0, 18, 2, sf::Color(255, 255, 255, 255));
	ui::Rect crosshairRectangle2 = ui::Rect("", 0, 0, 2, 18, sf::Color(255, 255, 255, 255));

	gui::Gui mainMenu = gui::Gui("main_menu", sf::Color(125, 125, 125));
	gui::Gui optionsGui = gui::Gui("options");
	gui::Gui pauseGui = gui::Gui("pause");
	gui::Gui emptyGui = gui::Gui("empty");

	input::InputManager& inputManager;
	gui::GuiManager guiManager;

	void updateLoadedChunks();

	float movementSpeed = 6.9f;
	float rotation = 0.f;
	float friction = 1.f;
	sf::Vector2f windowCenter;
	sf::Vector2f lastMousePos;

	void updateRotation(sf::Vector2u wsize, sf::RenderWindow& window);

	void updatePosition(float elapsedTime);

	void getAndRunCommand();

	void drawGame(sf::Vector2u wsize, sf::RenderWindow& window, sf::Clock& clock);

	void updateDebugInfo();

	void drawUI(sf::RenderWindow& window);

	void manageKeys();
};