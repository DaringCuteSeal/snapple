#include "scene_interlude.hpp"

InterludeScene::InterludeScene() {
	this->current_frame = 0;
	this->time_per_frame = 1.0/this->fps;
	this->is_ready_to_start_game = false;
	this->last_time = GetTime();
	for (size_t i = 0; i < this->n_frames; i++){
		this->interlude_animation[i].Load(this->frames[i]);
	}
	this->background.Load(this->background_filename);
}

void InterludeScene::init(GameComponents::GameStateManager* game_state_manager, function<void()> start_game_callback){
	this->game_state_manager = game_state_manager;
	this->start_game_callback = start_game_callback;

}

void InterludeScene::draw() {
	this->background.Draw(0, 0);
	this->interlude_animation[this->current_frame].Draw(0, 0);
};

void InterludeScene::update() {
	if (this->is_ready_to_start_game) return;
	if (GetTime() - this->last_time >= this->time_per_frame){
		this->current_frame += 1;
		this->last_time = GetTime();
		if (this->current_frame == this->n_frames - 1) {
			this->game_state_manager->timer.attach(1, this->start_game_callback);
			this->is_ready_to_start_game = true;
		}
	}
};
