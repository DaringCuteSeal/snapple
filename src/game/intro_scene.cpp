#include "intro_scene.hpp"

IntroScene::IntroScene() {
	// Load semua frame animasi ke memori
	for (size_t i = 0; i < this->n_frames; i++) {
		this->intro_animation[i] = (LoadTexture(this->frames[i]));
	}
	this->time_per_fps = 1.0/this->fps;
	this->last_time = GetTime();
	this->is_looping = false;

	this->background = LoadTexture(this->background_filename);
}

void IntroScene::draw() {
	this->background.Draw();

	if (this->is_looping){
		this->intro_animation[this->current_frame].Draw(0, 0);
	} else {
		this->intro_animation[this->current_frame].Draw(0, 0);
	}
}

void IntroScene::update() {
	if (this->is_looping){
		if (GetTime() - this->last_time >= this->time_per_fps){
			if (this->current_frame == this->n_frames - 1){
				this->current_frame = this->start_loop;
			} else {
				this->current_frame += 1;
			}
			this->last_time = GetTime();
		}

	} else {
		if (GetTime() - this->last_time >= this->time_per_fps){
			this->current_frame += 1;
			this->last_time = GetTime();
			if (this->current_frame == this->n_frames - 1){
				this->is_looping = true;
			}
		}
	}
}

