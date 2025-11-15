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
	this->game_state_manager.event_manager.update();
	this->game_state_manager.scene_manager.update();
	this->game_state_manager.sprite_manager.update();
};

void Game::draw() {
	BeginDrawing();

	this->window->ClearBackground(BROWN);

	DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

	EndDrawing();
	this->game_state_manager.scene_manager.draw();
	this->game_state_manager.sprite_manager.draw();
};

Game::Game(raylib::Window* window) : game_state_manager(&default_scene) {
	this->window = window;
}
