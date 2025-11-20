#include "scene_game.hpp"
#include <raylib.h>

TileCoord get_random_tile_coord() {
	return TileCoord {
		.row = GetRandomValue(0, TILE_ROWS - 1),
		.col = GetRandomValue(0, TILE_COLUMNS - 1),
	};
}

bool get_random_bool() {
	return GetRandomValue(0, 1);
}

AppleExplosion::AppleExplosion() {
	for (size_t i = 0; i < this->n_frames; i++){
		this->explosion_animation[i] = LoadTexture(this->frames[i]);
	}
	this->last_time = GetTime();
	this->time_per_frame = 1.0/this->fps;
}

void AppleExplosion::draw() {
	this->explosion_animation[this->current_frame].Draw(this->pos.row, this->pos.col);
}

bool AppleExplosion::ended() {
	return this->current_frame == this->n_frames - 1;
}

void AppleExplosion::update() {
	if (this->current_frame >= this->n_frames - 1) return;
	if (GetTime() - last_time >= this->time_per_frame) {
		this->current_frame += 1;
	}
}

PlayerStats::PlayerStats()  {

}

void PlayerStats::update() {
}

void PlayerStats::draw_lives(int x, int y) {
	int x_draw = x;
	for (size_t i = 0; i < this->lives; i++){
		x_draw += this->hearts_gaps;
		this->heart_texture.Draw(x_draw, y);
	}
}

MathQuestionDisplay::MathQuestionDisplay() {
	// Generate question langsung biar gak ada masalah null pointer kalo
	// `this->get_question()` dipanggil.
	this->generate_new_question();
}

void MathQuestionDisplay::generate_new_question() {
	int random = GetRandomValue(1, 100);
	int addsubtr_easy = this->probability_addsubtr_easy;
	int addsubtr_hard = addsubtr_easy + this->probability_addsubtr_hard;
	int multdiv_easy = addsubtr_hard + this->probability_multdiv_easy;
	int multdiv_hard = multdiv_easy + this->probability_multdiv_hard;
	int powerssqrt_easy = multdiv_hard + this->probability_powerssqrt_easy;
	int powerssqrt_hard = powerssqrt_easy + this->probability_powerssqrt_hard;

	// dengan asumsi bahwa RNG-nya Raylib gak biased, semua angka dari 1-100
	// ada probabilitas sama untuk kita dapat jadi kita bisa pakai range
	// seperti di bawah.

	// note: kita hanya override fields di `this->q_now` sebenarnya, struct
	// nya gak dioverride dengan yang baru. PASTIKAN benar-benar semua field
	// di override.

	// TODO: extract jadi bbrp methods lain kalo sempet
	if (random <= addsubtr_easy) {
		int lhs = GetRandomValue(0, 15);
		int rhs = GetRandomValue(0, 15);

		// Yang gampang pakai margin aja.
		int margin1 = GetRandomValue(1, 3);
		int margin2 = GetRandomValue(-3, -1);

		// kalau ini false, jadi pengurangan.
		bool add = get_random_bool();

		this->q_now.display = lhs + (add ? " + " : " - ") + rhs;
		this->q_now.answers[0] = {get_random_tile_coord(), add ? (lhs+rhs) : (lhs-rhs)};
		this->q_now.answers[1] = {get_random_tile_coord(), add ? (lhs+rhs + margin1) : (lhs-rhs + margin1)};
		this->q_now.answers[2] = {get_random_tile_coord(), add ? (lhs+rhs + margin2) : (lhs-rhs + margin2)};

	} else if (random <= addsubtr_hard) {
		int lhs_tens = 10 * GetRandomValue(0, 5);
		int lhs_ones = GetRandomValue(0, 9);
		int lhs  = lhs_tens + lhs_ones;

		int rhs_tens = 10 * GetRandomValue(0, 5);
		int rhs_ones = GetRandomValue(0, 9);
		int rhs  = lhs_tens + lhs_ones;

		// kalau ini false, jadi pengurangan.
		bool add = GetRandomValue(0, 1);

		// opsi ke-2 yang susah: ketika puluhan nya beda 1 atau 2
		int diff_tens = 10 * GetRandomValue(-2, 2);
		diff_tens = diff_tens == 0 ? 10 : diff_tens; // kalo tadi dapet 0 kita paksa jadi 10 aja. jadi agak biased tapi gapapa

		this->q_now.display = lhs + (add ? " + " : " - ") + rhs;
		this->q_now.answers[0] = {get_random_tile_coord(), add ? (lhs+rhs) : (lhs-rhs)};
		this->q_now.answers[1] = {get_random_tile_coord(), add ? (lhs+rhs + diff_tens) : (lhs-rhs + diff_tens)};

		// opsi ke-3 yang susah: satuannya dibalik (buat pengurangan) -> menjebak
		// kalau tadi penambahan kita balik aja `diff_tens` yang sebelumnya.
		this->q_now.answers[2] = {get_random_tile_coord(), add ? (lhs+rhs - diff_tens) : ((lhs_tens + rhs_ones) - (rhs_tens + lhs_ones))};

	} else if (random <= multdiv_easy) {
		// 1 sampai 10 buat yang gampang.
		int lhs = GetRandomValue(-10, 10);
		int rhs = GetRandomValue(-10, 10);

		// kalau ini false, jadi pembagian.
		bool mult = get_random_bool();

		// kalau pembagian, buat menghindari pecahan, kita balik persamaannya.
		// tadi itu lhs × rhs = c, sekarang kita lakukan c ÷ lhs = rhs misalnya.
		if (!mult) {
			int tmp = lhs*rhs; // c
			int lhs = tmp;
			int rhs = lhs;
		}

		this->q_now.display = lhs + (mult ? " × " : " ÷ ") + rhs;
		this->q_now.answers[0] = {get_random_tile_coord(), mult ? (lhs*rhs) : (lhs/rhs)};

		// opsi ke-2: off by 1 untuk salah satu lengan (kalau perkalian)
		// kalau pembagian yang kita off-by-one itu hasilnya aja, bukan lhs/rhs.
		if (mult) {
			if (get_random_bool()) {
				// lengan kiri
				lhs += get_random_bool() ? 1 : -1;
			} else {
				// lengan kanan
				rhs += get_random_bool() ? 1 : -1;
			}
			this->q_now.answers[1] = {get_random_tile_coord(), lhs*rhs};
		} else {
			int margin = get_random_bool() ? 1 : -1;
			this->q_now.answers[1] = {get_random_tile_coord(), lhs/rhs + margin};
		}
		
		// opsi ke-3: 

		this->q_now.answers[2] = {get_random_tile_coord(), add ? (lhs+rhs + margin2) : (lhs-rhs + margin2)};

	} else if (random <= multdiv_hard) {
		
	} else if (random <= powerssqrt_easy) {

	} else if (random <= powerssqrt_hard) {

	}


}

StatusBar::StatusBar() {
	this->pos = {this->min_statusbar_pos_y, 0};
}

void StatusBar::draw() {
	this->texture.Draw();
	this->stats.draw_lives(this->pos.col + this->lives_pos.col, this->pos.row + this->lives_pos.row);
}

GameScene::GameScene() {

}

void GameScene::update() {
	if (!this->is_game_started) {
		this->explosion_animation.update();
		return;
	}
}

void GameScene::draw() {
	if (!this->is_game_started) {
		this->explosion_animation.draw();
		return;
	}
	this->ground_texture.Draw(0, 0);
	this->status_bar.draw();
}
