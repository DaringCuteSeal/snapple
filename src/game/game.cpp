#include "../prelude.hpp"
#include "game.hpp"

#include <raylib.h>

void Game::update() {
	this->game_state_manager.update();
	if (!this->play_bgmusic) this->background_music.Stop();
	if (this->play_bgmusic && !this->background_music.IsPlaying()) this->background_music.Play();
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
	this->game_scene.reset(true);
	this->game_state_manager.scene_manager.set_scene(&this->game_scene);
}

void Game::go_back_to_menu() {
	this->game_state_manager.scene_manager.set_scene(&this->intro_scene);
	this->interlude_scene.reset();
	this->intro_scene.reset();
	this->intro_scene.letters_bounce_down();
	this->play_bgmusic = true;
}

Game::Game() : game_state_manager(&this->intro_scene) {
	this->play_bgmusic = true;
	this->game_font.Load(FONT_PATH, 50, 0, 0);
	this->intro_scene.init(&this->game_font, &this->game_state_manager, [this](){this->start_game_interlude();});
	this->interlude_scene.init(&this->game_state_manager, [this](){this->start_game();});
	this->game_scene.init(&this->game_font, &this->game_state_manager, [this](){this->go_back_to_menu();}, &this->play_bgmusic);

	this->background_music.Load(this->music_file);
}
