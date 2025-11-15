#include "../prelude.hpp"
#include "game.hpp"

void Game::update() {
	this->game_state_manager.event_manager.update();
	this->game_state_manager.scene_manager.update();
	this->game_state_manager.sprite_manager.update();
};

void Game::draw() {
	this->game_state_manager.scene_manager.draw();
	this->game_state_manager.sprite_manager.draw();
};
