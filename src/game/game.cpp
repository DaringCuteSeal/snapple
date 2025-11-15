#include "../prelude.hpp"
#include "game.hpp"

class DefaultScene : public GameComponents::Scene {
private:
public:
	void draw() {
		ClearBackground(WHITE);
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


	EndDrawing();
	this->game_state_manager.draw();
};

Game::Game(raylib::Window* window) : game_state_manager(&default_scene) {
	this->window = window;
}
