#include "../prelude.hpp"
#include "game.hpp"

#include <raylib.h>

void Game::update() {
	this->game_state_manager.update();
};

void Game::draw() {
	BeginDrawing();
	this->game_state_manager.draw();
	EndDrawing();
};

Game::Game() : game_state_manager(&this->intro_scene) {
	this->game_font.Load(FONT_PATH, 50, 0, 0);
	this->intro_scene.set_font(&this->game_font);
}
