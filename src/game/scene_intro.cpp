#include "scene_intro.hpp"
#include <cstdlib>
#include <raylib.h>

IntroScene::IntroScene() {
	// Load semua frame animasi ke memori
	for (size_t i = 0; i < this->n_frames; i++) {
		this->intro_animation[i] = LoadTexture(this->frames[i]);
	}
	this->time_per_frame = 1.0/this->fps;
	this->last_time = GetTime();
	this->hint_text_last_time = GetTime();
	this->is_looping = false;
	this->ready_to_start_game = false;

	this->background = LoadTexture(this->background_filename);
	this->show_hint_text = false;
	this->hint_text_alpha = 0;
	this->active = true;

	// load texture nya letters kita
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].load_texture(this->letters[i]);
		this->letter_sprites[i].pos.row = this->letters_y_min;
		this->letter_sprites[i].pos.col = this->letters_x_pos[i];
	}
}

void IntroScene::init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager, function<void()> start_interlude_callback){
	this->game_font = game_font;
	this->game_state_manager = game_state_manager;
	this->start_interlude_callback = start_interlude_callback;
}

void IntroScene::letters_bounce_down() {
	size_t y = this->letters_y_min;
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].bounce_down();
		this->letter_sprites[i].set_ground(this->ground_y[i]);
		this->letter_sprites[i].pos.col = this->letters_x_pos[i];
		this->letter_sprites[i].pos.row = y;
		y -= 50;
	}
}

void IntroScene::letters_fly_up() {
	double v = 2;
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].set_ground(this->letters_y_min);
		this->letter_sprites[i].pos.col = this->letters_x_pos[i];
		this->letter_sprites[i].fly_up(v);
		v += 2;
	}
}

void IntroScene::draw() {
	this->background.Draw();

	// draw sprite-sprite judul game
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].draw();
	}

	this->intro_animation[this->current_frame].Draw(0, 0);

	if (this->show_hint_text){
		// kasih hint mulai main
		DrawTextEx(*(this->game_font), string("SPASI untuk mulai..."), raylib::Vector2(1450, 1011), 60, 1.0, {255, 255, 255, this->hint_text_alpha});
	}
}

void IntroScene::update() {
	// update sprite-sprite judul game
	for (size_t i = 0; i < this->n_letters; i++) {
		this->letter_sprites[i].update();
	}

	if (!this->active) return;
	if (this->is_looping){ // kalo animasi awal udah selesai, loop
		if (GetTime() - this->last_time >= this->time_per_frame){
			if (this->current_frame == this->n_frames - 1){
				this->current_frame = this->start_loop;
			} else {
				this->current_frame += 1;
			}
			this->last_time = GetTime();
		}

		// teks, update kalo muncul
		if (this->show_hint_text) {
			if (GetTime() - this->hint_text_last_time >= this->hint_text_blink_duration){
				if (this->hint_text_alpha == 255){
					hint_text_alpha = 0;
				} else {
					hint_text_alpha = 255;
				}
				this->hint_text_last_time = GetTime();
			}
		}

		if (this->ready_to_start_game && this->current_frame == 140){
			// tunggu judul selesai, terus baru interlude
			// sebenarnya bukan solusi elegan, tapi oke lah. daripada kebanyakan
			// variabel state.
			this->game_state_manager->timer.attach(1.3, [this](){
				this->start_interlude_callback();

			});
			// matiin update, kalo ga callback nya bisa gak
			// sengaja ke-call beberapa kali
			this->active = false;
		}

		// Untuk mulai main
		if (IsKeyPressed(KEY_SPACE)) {
			this->game_state_manager->event_manager.dispatch(GameComponents::Event {[this](){
				this->show_hint_text = false;
				this->letters_fly_up();
			}});
			this->ready_to_start_game = true;
		}

	} else { // masih animasi awal
		if (GetTime() - this->last_time >= this->time_per_frame){
			this->current_frame += 1;
			this->last_time = GetTime();
			if (this->current_frame == this->n_frames - 1){
				this->show_hint_text = true;
				this->is_looping = true;
				this->letters_bounce_down();
			}
		}
	}
}

LetterSprite::LetterSprite() {
	this->state = NONE;
	this->vy = 0;
}

void LetterSprite::load_texture(const char* letter_file_name) {
	this->texture = LoadTexture(letter_file_name);
}

void LetterSprite::draw() {
	this->texture.Draw(this->pos.col, this->pos.row);
}

void LetterSprite::bounce_down() {
	this->bounces = 0;
	this->state = BOUNCE_DOWN;
	this->vy = 0;
	this->ay = abs(this->ay);
}

/** Bikin teks terbang ke atas, dengan awalnya turun ke bawah dulu.
 * Catatan: `velocity` harus positif.
*/
void LetterSprite::fly_up(double velocity) {
	this->state = FLY_UP;
	this->vy = velocity;
	this->ay = -1 * abs(this->ay); // jadi negatif biar naik
}

void LetterSprite::update() {
	switch(this->state) {
		case BOUNCE_DOWN:
			if (bounces >= this->max_bounces) return;
			this->vy += this->ay; // akselerasi
			this->pos.row += this->vy; // ubah posisi berdasarkan kecepatan

			if (this->pos.row >= this->ground_y){
				this->vy = this->vy * -1 * this->COR; // balik kecepatan (mantul)
				this->pos.row = this->ground_y;
				bounces += 1;
			}
		break;

		case FLY_UP:
			if (this->pos.row <= this->ground_y) return;
			this->vy += this->ay; // akselerasi
			this->pos.row += this->vy; // ubah posisi berdasarkan kecepatan
		break;

		case NONE:
			break;
	}
}

void LetterSprite::set_ground(size_t y){
	this->ground_y = y;
}
