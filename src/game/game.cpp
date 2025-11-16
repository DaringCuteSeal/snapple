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

void Game::start_game_interlude() {
	this->game_state_manager.scene_manager.set_scene(&this->interlude_scene);
}

void Game::start_game() {
}

Game::Game() : game_state_manager(&this->intro_scene) {
	this->game_font.Load(FONT_PATH, 50, 0, 0);
	this->intro_scene.init(&this->game_font, &this->game_state_manager, [this](){this->start_game_interlude();});
	this->interlude_scene.init(&this->game_state_manager, [this](){this->start_game();});
}
