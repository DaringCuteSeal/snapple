#include "../prelude.hpp"
#include "game.hpp"

class DefaultScene : public GameComponents::Scene {
	
	void draw() {

	}

	void update() {

	}
};

DefaultScene default_scene;

void Game::update() {
	this->game_state_manager.update();
};

void Game::draw() {
	BeginDrawing();

	this->window->ClearBackground(BROWN);

	DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

	EndDrawing();
	this->game_state_manager.draw();
};

Game::Game(raylib::Window* window) : game_state_manager(&default_scene) {
	this->window = window;
}
