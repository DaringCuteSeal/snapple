#include "scene_game.hpp"
#include <raylib.h>
#include <raymath.h>
#include <string>

using std::to_string;

// TODO: jangan overlap player
TileCoord get_random_tile_coord() {
	return TileCoord {
		.row = GetRandomValue(0, TILE_ROWS - 1),
		.col = GetRandomValue(0, TILE_COLUMNS - 1),
	};
}

GameComponents::Coordinate TileCoord::to_coord() {
	return GameComponents::Coordinate { TILE_DIMENSION * this->row, TILE_DIMENSION * this->col };
}

GameComponents::Coordinate TileCoord::to_coord_center() {
	return GameComponents::Coordinate { TILE_DIMENSION * this->row + TILE_DIMENSION/2, TILE_DIMENSION * this->col + TILE_DIMENSION/2};
}

raylib::Vector2 TileCoord::to_vector2() {
	return raylib::Vector2 { float(this->col * TILE_DIMENSION), float(this->row * TILE_DIMENSION) };
}

raylib::Vector2 TileCoord::to_vector2_center() {
	return raylib::Vector2 { float(this->col * TILE_DIMENSION + (TILE_DIMENSION/2.0)), float(this->row * TILE_DIMENSION + (TILE_DIMENSION/2.0)) };
}

bool get_random_bool() {
	return GetRandomValue(0, 1);
}

AppleExplosion::AppleExplosion() {
	for (size_t i = 0; i < this->n_frames; i++){
		this->explosion_animation[i].Load(this->frames[i]);
	}
	this->last_time = GetTime();
	this->time_per_frame = 1.0/this->fps;
}

bool AppleExplosion::show_apple() {
	return this->current_frame <= this->frame_apple_disappear;
}

void AppleExplosion::draw() {
	if (this->current_frame >= this->n_frames - 1) return;
	this->explosion_animation[this->current_frame].Draw(this->pos.row, this->pos.col);
}

bool AppleExplosion::ended() {
	return this->current_frame == this->n_frames - 1;
}

void AppleExplosion::update() {
	if (this->current_frame >= this->n_frames - 1) return;
	if (GetTime() - last_time >= this->time_per_frame) {
		this->current_frame += 1;
		this->last_time = GetTime();
	}
}

PlayerStats::PlayerStats()  {
	this->heart_texture.Load(this->heart_texture_file);
}

void PlayerStats::init (raylib::Font* game_font)  {
	this->length = 1 + SNAKE_INITIAL_LENGTH;
	this->game_font = game_font;
}

void PlayerStats::draw_lives(int x, int y) {
	int x_draw = x;
	for (size_t i = 0; i < this->lives; i++){
		x_draw += this->hearts_gaps;
		this->heart_texture.Draw(x_draw, y);
	}
}

void PlayerStats::draw_length(int x, int y) {
	DrawTextEx(*(this->game_font), "Length: " + to_string(this->length / SNAKE_LENGTH_FACTOR), raylib::Vector2(x, y), 50, 1.0, this->text_color);
}

void PlayerStats::draw_pts(int x, int y) {
	DrawTextEx(*(this->game_font), "Pts: " + to_string(this->pts), raylib::Vector2(x, y), 50, 1.0, this->text_color);
}


MathQuestionDisplay::MathQuestionDisplay() {
	// Generate question langsung biar gak ada masalah null pointer kalo
	// `this->get_question()` dipanggil.
	this->generate_new_question();
}
void MathQuestionDisplay::init(raylib::Font* game_font) {
	this->game_font = game_font;
}

// Generate pertanyaan matematika baru.
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
		int rhs = GetRandomValue(0, 16);

		// Yang gampang pakai margin aja.
		int margin1 = GetRandomValue(1, 3);
		int margin2 = GetRandomValue(-3, -1);

		// kalau ini false, jadi pengurangan.
		bool add = get_random_bool();

		this->q_now.display = to_string(lhs) + (add ? " + " : " - ") + to_string(rhs) + " = ?";
		this->q_now.answers[0] = add ? (lhs+rhs) : (lhs-rhs);
		this->q_now.answers[1] = add ? (lhs+rhs + margin1) : (lhs-rhs + margin1);
		this->q_now.answers[2] = add ? (lhs+rhs + margin2) : (lhs-rhs + margin2);

	} else if (random <= addsubtr_hard) {
		int lhs_tens = 10 * GetRandomValue(2, 7);
		int lhs_ones = GetRandomValue(0, 8);
		int lhs = lhs_tens + lhs_ones;

		int rhs_tens = 10 * GetRandomValue(2, 8);
		int rhs_ones = GetRandomValue(0, 9);
		int rhs = rhs_tens + rhs_ones;

		// kalau ini false, jadi pengurangan.
		bool add = GetRandomValue(0, 1);

		// opsi ke-2 yang susah: ketika puluhan nya beda 1 atau 2
		int diff_tens = 10 * GetRandomValue(2, 3) * (get_random_bool() ? 1 : -1);
		diff_tens = diff_tens == 0 ? 10 : diff_tens; // kalo tadi dapet 0 kita paksa jadi 10 aja. jadi agak biased tapi gapapa

		this->q_now.display = to_string(lhs) + (add ? " + " : " - ") + to_string(rhs) + " = ?";
		this->q_now.answers[0] = add ? (lhs+rhs) : (lhs-rhs);
		this->q_now.answers[1] = add ? (lhs+rhs + diff_tens) : (lhs-rhs + diff_tens);

		// opsi ke-3 yang susah: satuannya dibalik (buat pengurangan) -> menjebak
		// kalau tadi penambahan kita balik aja `diff_tens` yang sebelumnya.
		this->q_now.answers[2] = add ? (lhs+rhs - diff_tens) : ((lhs_tens + rhs_ones) - (rhs_tens + lhs_ones));

	} else if (random <= multdiv_easy) {
		// 1 sampai 10 buat yang gampang.
		// hindari angka 0 agar tidak ada pembagian dengan 0 yang bisa crash.
		int lhs = GetRandomValue(1, 10) * (get_random_bool() ? 1 : -1);
		int rhs = GetRandomValue(1, 10) * (get_random_bool() ? 1 : -1);

		// kalau ini false, jadi pembagian.
		bool mult = get_random_bool();

		// kalau pembagian, buat menghindari pecahan, kita balik persamaannya.
		// tadi itu lhs × rhs = c, sekarang kita lakukan c ÷ lhs = rhs misalnya.
		if (!mult) {
			int tmp = lhs*rhs; // c
			rhs = lhs;
			lhs = tmp;
		}

		this->q_now.display = to_string(lhs) + (mult ? " x " : " / ") + to_string(rhs) + " = ?";
		this->q_now.answers[0] = mult ? (lhs*rhs) : (lhs/rhs);

		// opsi ke-2: shift by margin
		int margin = GetRandomValue(1, 10);
		if (get_random_bool()) margin = -margin; // flip

		this->q_now.answers[1] = mult ? (lhs*rhs + margin) : (lhs/rhs + margin);

		// opsi ke-3: off by 1 untuk salah satu lengan (kalau perkalian)
		// kalau pembagian yang kita off-by-one itu hasilnya aja, bukan lhs/rhs.
		if (mult) {
			if (get_random_bool()) {
				// lengan kiri
				lhs += get_random_bool() ? 1 : -1;
			} else {
				// lengan kanan
				rhs += get_random_bool() ? 1 : -1;
			}
			this->q_now.answers[2] = lhs*rhs;
		} else {
			int margin = get_random_bool() ? 1 : -1;
			this->q_now.answers[2] = lhs/rhs + margin;
		}

	} else if (random <= multdiv_hard) {
		// 20 sampai 50 buat yang gampang.
		int lhs = GetRandomValue(20, 50);
		if (get_random_bool()) lhs = -lhs;
		// rhs nya jangan gede gede tapi.
		int rhs = GetRandomValue(10, 30);
		if (get_random_bool()) rhs = -rhs;

		// kalau ini false, jadi pembagian.
		bool mult = get_random_bool();

		// kalau pembagian, buat menghindari pecahan, kita balik persamaannya.
		// tadi itu lhs × rhs = c, sekarang kita lakukan c ÷ lhs = rhs misalnya.
		if (!mult) {
			int tmp = lhs*rhs; // c
			rhs = lhs;
			lhs = tmp;
		}

		this->q_now.display = to_string(lhs) + (mult ? " x " : " / ") + to_string(rhs) + " = ?";
		this->q_now.answers[0] = mult ? (lhs*rhs) : (lhs/rhs);

		// opsi ke-2: shift by margin (tens)
		int margin = 10 * GetRandomValue(1, 3);
		if (get_random_bool()) margin = -margin; // flip

		this->q_now.answers[1] = mult ? (lhs*rhs + margin) : (lhs/rhs + margin);

		// opsi ke-3: off by 1 untuk salah satu lengan (kalau perkalian)
		// kalau pembagian yang kita off-by-one itu hasilnya aja, bukan lhs/rhs.
		if (mult) {
			if (get_random_bool()) {
				// lengan kiri
				lhs += get_random_bool() ? 1 : -1;
			} else {
				// lengan kanan
				rhs += get_random_bool() ? 1 : -1;
			}
			this->q_now.answers[2] = lhs*rhs;
		} else {
			int margin = get_random_bool() ? 1 : -1;
			this->q_now.answers[2] = lhs/rhs + margin;
		}
		
	} else if (random <= powerssqrt_easy) {
		int num = GetRandomValue(2, 20);

		// opsi 2: margin puluhan
		int margin1 = 10 * GetRandomValue(1, 2);
		if (get_random_bool()) margin1 = -margin1;

		// opsi 3: margin satuan
		int margin2 = GetRandomValue(1, 2);

		// kalau ini false, jadi akar pangkat 2.
		bool power = get_random_bool();
		if (get_random_bool()) margin2 = -margin2;

		// kita balik kayak di perkalian pembagian
		if (!power) {
			num = pow(num, 2);
		}

		this->q_now.display = power ? to_string(num) + "^2" : "sqrt " + to_string(num) + " = ?";
		this->q_now.answers[0] = power ? (pow(num, 2)) : (sqrt(num));
		this->q_now.answers[1] = power ? (pow(num, 2) + margin1) : (sqrt(num) + margin2);
		this->q_now.answers[2] = power ? (pow(num + margin2, 2)) : (sqrt(num) - margin2);

	} else if (random <= powerssqrt_hard) {
		int num = GetRandomValue(30, 50);

		// opsi 2: margin puluhan
		int margin1 = 10 * GetRandomValue(1, 2);
		if (get_random_bool()) margin1 = -margin1;

		// opsi 3: margin satuan
		int margin2 = GetRandomValue(1, 2);
		if (get_random_bool()) margin2 = -margin2;

		// kalau ini false, jadi akar pangkat 2.
		bool power = get_random_bool();

		// kita balik kayak di perkalian pembagian
		if (!power) {
			num = pow(num, 2);
		}

		this->q_now.display = power ? to_string(num) + "^2" : "sqrt " + to_string(num) + " = ?";
		this->q_now.answers[0] = power ? (pow(num, 2)) : sqrt(num);
		this->q_now.answers[1] = power ? (pow(num, 2) + margin1) : (sqrt(num) + margin1);
		this->q_now.answers[2] = power ? (pow(num + margin2, 2)) : (sqrt(num) - margin1);
	}

	// Ubah ke string
	for (size_t i = 0; i < 3; i++) {
		this->q_now.answers_str[i] = to_string(this->q_now.answers[i]);
	}

	// Generate koordinat-koordinat
	int offset_cols_1 = GetRandomValue(1, TILE_COLUMNS - 2);
	int offset_rows_1 = GetRandomValue(1, TILE_ROWS - 2);

	int offset_cols_2 = GetRandomValue(1, TILE_COLUMNS - 2);
	if (offset_cols_2 == offset_cols_1) offset_cols_2 += 3; // aga biased .. oklah im too lazy tho
	int offset_rows_2 = GetRandomValue(1, TILE_ROWS - 2);
	if (offset_rows_2 == offset_rows_1) offset_rows_2 += 5; // aga biased .. oklah im too lazy tho

	this->q_now.coords[0] = TileCoord {
		.row = GetRandomValue(1, TILE_ROWS - 1),
		.col = GetRandomValue(1, TILE_COLUMNS - 1),
	};
	this->q_now.coords[1] = TileCoord { 
		1 + ((q_now.coords[0].row + offset_rows_1*TILE_DIMENSION) % (TILE_ROWS-1)),
		1 + ((q_now.coords[0].col + offset_cols_1*TILE_DIMENSION) % (TILE_COLUMNS-1))
	};

	this->q_now.coords[2] = TileCoord { 
		1 + ((q_now.coords[0].row + offset_rows_2*TILE_DIMENSION) % (TILE_ROWS - 1)),
		1 + ((q_now.coords[0].col + offset_cols_2*TILE_DIMENSION) % (TILE_COLUMNS - 1))
	};

	// Koordinat asli (dalam piksel)
	for (size_t i = 0; i < 3; i++) {
		this->q_now.coords_pixel[i] = q_now.coords[i].to_coord().add_row(15).add_col((60 - q_now.answers_str[i].length()*12)/2);
	}
}

MathQuestion* MathQuestionDisplay::get_question() {
	return &this->q_now;
}

void MathQuestionDisplay::draw_bar_item(int x, int y) {
	DrawTextEx(*(this->game_font), this->q_now.display, raylib::Vector2(x, y), 60, 1.0, this->bar_color);

}

void MathQuestionDisplay::draw_answers() {
	for (size_t i = 0; i < 3; i++) {
		// TODO: mungkin pakai raylib::Vector2 langsung daripada struct kita
		// sendiri hehe
		DrawTextEx(*(this->game_font), this->q_now.answers_str[i], raylib::Vector2(this->q_now.coords_pixel[i].col, this->q_now.coords_pixel[i].row), 30, 1.0, this->food_color);
	}
}

StatusBar::StatusBar() {
	this->pos = {this->min_statusbar_pos_y, 0};
	this->texture.Load(this->texture_file);
}

void StatusBar::fall() {
	this->is_falling = true;
	this->pos = {this->min_statusbar_pos_y, 0};
}

void StatusBar::init(raylib::Font* game_font) {
	this->math.init(game_font);
	this->stats.init(game_font);
}

void StatusBar::draw() {
	this->texture.Draw(this->pos.col, this->pos.row);
	this->stats.draw_lives(this->pos.col + this->lives_pos.col, this->pos.row + this->lives_pos.row);
	this->stats.draw_pts(this->pos.col + this->pts_pos.col, this->pos.row + this->pts_pos.row);
	this->stats.draw_length(this->pos.col + this->snake_length_pos.col, this->pos.row + this->snake_length_pos.row);
	this->math.draw_bar_item(this->pos.col + this->question_pos.col, this->pos.row + this->question_pos.row);
	this->math.draw_answers();
}

void StatusBar::update() {
	if (this->is_falling){
		if (this->pos.row >= this->pos_y) this->is_falling = false;
		this->pos.row += this->vy;
	}
}

Player::Player() {
	this->snake_head_u.Load(this->texture_snake_head_u);
	this->snake_head_l.Load(this->texture_snake_head_l);
	this->snake_head_d.Load(this->texture_snake_head_d);
	this->snake_head_r.Load(this->texture_snake_head_r);
	this->head_pos = this->initial_pos.to_coord().to_vector2();
	this->controllable = false;
}

void Player::create_snake() {
	this->active = true;
	this->try_check_collision = false;
	this->head_pos = this->initial_pos.to_vector2();
	this->current_direction = LEFT;
	this->turn_queue = nullopt;
	this->points.clear();
	float x = this->head_pos.x + this->snake_point_radius;
	float y = this->head_pos.y;

	// badan
	for (size_t i = 0; i < SNAKE_INITIAL_LENGTH; i++) {
		this->points.push_back({x, y});
		x += this->snake_point_radius;
	}
}

void Player::move() {
	size_t points_size = points.size();

	if (points_size == 0) return; // siapa tau gitu kan ya meski tidak mungkin

	// Yang lainnya mengikuti
	for (size_t i = points_size - 1; i >= 1; i--) {
		this->points[i].x = this->points[i-1].x;
		this->points[i].y = this->points[i-1].y;
	}
	this->points[0] = this->head_pos + raylib::Vector2 { TILE_DIMENSION/2.0, TILE_DIMENSION/2.0};

	switch(this->current_direction) {
		case UP:
			this->head_pos.y -= this->snake_point_radius;
			break;
		case DOWN:
			this->head_pos.y += this->snake_point_radius;
			break;
		case LEFT:
			this->head_pos.x -= this->snake_point_radius;
			break;
		case RIGHT:
			this->head_pos.x += this->snake_point_radius;
			break;
	}

}

bool Player::check_collision_self() {
	size_t points_size = this->points.size();
	size_t init = ceil(TILE_DIMENSION/2/this->snake_point_radius);
	for (size_t i = init; i < points_size; i++) {
		if (CheckCollisionCircles(raylib::Vector2 {this->head_pos.x + TILE_DIMENSION/2, this->head_pos.y + TILE_DIMENSION/2}, this->snake_body_radius, this->points[i], this->snake_body_radius)) return true;
	}
	return false;
}

bool Player::check_collision_corners() {
	return (this->head_pos.x < 0 || this->head_pos.x + TILE_DIMENSION > TILE_COLUMNS * TILE_DIMENSION || this->head_pos.y < TILE_DIMENSION || this->head_pos.y > TILE_ROWS * TILE_DIMENSION);
}

void Player::check_collision() {
	if (this->check_collision_self() || this->check_collision_corners()) {
		this->active = false;
	};
}

void Player::unqueue_turn() {
	// Kalau pemainnya ngelunjak mau putar balik, jangan bolehin
	if ((this->current_direction == UP && this->turn_queue == DOWN)
		|| (this->current_direction == DOWN && this->turn_queue == UP)
		|| (this->current_direction == LEFT && this->turn_queue == RIGHT)
		|| (this->current_direction == RIGHT && this->turn_queue == LEFT)
	) return;
	if (!this->turn_queue.has_value()) return;
	this->current_direction = this->turn_queue.value();
	turn_queue = nullopt;
}

void Player::update() {
	if (!active) return;

	if (this->try_check_collision) {
		this->check_collision();
	}

	// Hanya terjadi sekali tiap main.
	if (this->head_pos.x <= 26 * TILE_DIMENSION) {
		this->controllable = true;
		this->try_check_collision = true;
	};

	if (this->controllable){
		if (IsKeyPressed(KEY_UP)) {
			this->turn_queue = UP;
		} else if (IsKeyPressed(KEY_DOWN)){
			this->turn_queue = DOWN;
		} else if (IsKeyPressed(KEY_LEFT)){
			this->turn_queue = LEFT;
		} else if (IsKeyPressed(KEY_RIGHT)){
			this->turn_queue = RIGHT;
		}
	}

	if (int(floor(this->head_pos.y)) % TILE_DIMENSION == 0
	&& int(floor(this->head_pos.x)) % TILE_DIMENSION == 0) {
		this->unqueue_turn();
	}

	this->move();
}

void Player::draw() {
	// Gambar tubuh
	size_t points_size = points.size();
	DrawLineStrip(this->points.data(), this->points.size(), this->snake_color);

	for (size_t i = 1; i < points_size; i++) {
		DrawCircle(this->points[i].x, this->points[i].y, this->snake_body_radius, this->snake_color);
	}

	// Gambar kepala
	switch(this->current_direction) {
		case UP:
			this->snake_head_u.Draw(this->head_pos);
			break;
		case DOWN:
			this->snake_head_d.Draw(this->head_pos);
			break;
		case LEFT:
			this->snake_head_l.Draw(this->head_pos);
			break;
		case RIGHT:
			this->snake_head_r.Draw(this->head_pos);
			break;
	}
}

GameScene::GameScene() {
	this->ground_texture_apple.Load(this->ground_texture_apple_file);
	this->ground_texture.Load(this->ground_texture_file);
	this->player.create_snake();
}

void GameScene::init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager) {
	this->status_bar.init(game_font);
	this->game_state_manager = game_state_manager;
}

void GameScene::update() {
	if (this->is_game_started) {
		this->status_bar.update();
		this->player.update();
	} else {
		this->explosion_animation.update();
		if (this->explosion_animation.ended()) {
			this->is_game_started = true;
			explosion_animation.update();
			this->game_state_manager->timer.attach(1, [this](){this->status_bar.fall();});
		}
	}
}

void GameScene::draw() {
	if (this->is_game_started) {
		this->ground_texture.Draw(0, 0);
		this->status_bar.draw();
		this->player.draw();
	} else {
		this->explosion_animation.show_apple()
			? this->ground_texture_apple.Draw(0, 0)
			: this->ground_texture.Draw();
		this->explosion_animation.draw();
	}
}
