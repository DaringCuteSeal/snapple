#include "intro_scene.hpp"
#include <raylib.h>

void IntroScene::set_font(raylib::Font* game_font){
	this->game_font = game_font;
}

IntroScene::IntroScene() {
	// Load semua frame animasi ke memori
	for (size_t i = 0; i < this->n_frames; i++) {
		this->intro_animation[i] = LoadTexture(this->frames[i]);
	}
	this->time_per_fps = 1.0/this->fps;
	this->last_time = GetTime();
	this->hint_text_last_time = GetTime();
	this->is_looping = false;

	size_t y = -500;
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].load_texture(this->letters[i]);
		this->letter_sprites[i].set_ground(this->ground_y[i]);
		this->letter_sprites[i].pos.col = this->x_pos[i];
		this->letter_sprites[i].pos.row = y;
		y -= 50;
	}

	this->background = LoadTexture(this->background_filename);
	this->hint_text_alpha = 0;
}

void IntroScene::draw() {
	this->background.Draw();

	for (auto& sprite : this->letter_sprites) {
		sprite.draw();
	}

	if (this->is_looping){
		this->intro_animation[this->current_frame].Draw(0, 0);
	} else {
		this->intro_animation[this->current_frame].Draw(0, 0);
	}

	// kasih hint mulai main
	DrawTextEx(*(this->game_font), string("SPASI untuk mulai..."), raylib::Vector2(1400, 1011), 60, 1.0, {255, 255, 255, this->hint_text_alpha});
}

void IntroScene::update() {
	// update sprite-sprite judul game
	for (auto& sprite : this->letter_sprites) {
		sprite.update();
	}

	if (this->is_looping){ // animasi awal udah selesai
		if (GetTime() - this->last_time >= this->time_per_fps){
			if (this->current_frame == this->n_frames - 1){
				this->current_frame = this->start_loop;
			} else {
				this->current_frame += 1;
			}
			this->last_time = GetTime();
		}

		// teks. pake ulang aja last_time yang awalnya buat animasi.
		if (GetTime() - this->hint_text_last_time >= this->hint_text_blink_duration){
			if (this->hint_text_alpha == 255){
				hint_text_alpha = 0;
			} else {
				hint_text_alpha = 255;
			}
			this->hint_text_last_time = GetTime();
		}

	} else {
		if (GetTime() - this->last_time >= this->time_per_fps){
			this->current_frame += 1;
			this->last_time = GetTime();
			if (this->current_frame == this->n_frames - 1){
				this->is_looping = true;
				for (auto& sprite : this->letter_sprites) {
					sprite.is_bouncing = true;
				}
			}
		}
	}
}

LetterSprite::LetterSprite() {
	this->bounces = 0;
	this->vy = 0;
	this->is_bouncing = false;
}

void LetterSprite::load_texture(const char* letter_file_name) {
	this->texture = LoadTexture(letter_file_name);
}

void LetterSprite::draw() {
	this->texture.Draw(this->pos.col, this->pos.row);
}

void LetterSprite::update() {
	if (!this->is_bouncing) return;
	if (bounces >= this->max_bounces) return;
	this->vy += this->ay; // akselerasi
	this->pos.row += this->vy; // ubah posisi berdasarkan kecepatan


	if (this->pos.row >= this->ground_y){
		this->vy = this->vy * -1 * this->COR; // balik kecepatan (mantul)
		this->pos.row = this->ground_y;
		bounces += 1;
	}
}

void LetterSprite::set_ground(size_t y){
	this->ground_y = y;
}
