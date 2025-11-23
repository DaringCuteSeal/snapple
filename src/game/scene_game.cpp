#include "scene_game.hpp"
#include <raylib.h>
#include <raymath.h>
#include <string>

using std::to_string, std::max;

// TODO: jangan overlap player
TileCoord get_random_tile_coord() {
	return TileCoord {
		.row = GetRandomValue(0, TILE_ROWS - 1),
		.col = GetRandomValue(0, TILE_COLUMNS - 1),
	};
}

Food get_food(Difficulty difficulty) {
	switch(difficulty) {
		case ADD_SUB_EASY:
		case ADD_SUB_HARD:
			return APPLE;
			break;

		case MULT_DIV_EASY:
		case MULT_DIV_HARD:
			return APPLE_PIE;
			break;

		case POW_SQRT_EASY:
		case POW_SQRT_HARD:
			return GOLDEN_APPLE_PIE;

		default:
			return APPLE;
			break;
	}
}

long long get_pts(Food food) {
	switch(food) {
		case BAD_APPLE:
			return -10;
			break;

		case APPLE:
			return 10;
			break;

		case APPLE_PIE:
			return 20;
			break;

		case GOLDEN_APPLE_PIE:
		return 50;
			break;

		default:
			return 10;
			break;
	}
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
	this->explosion_sound.Load(this->explosion_sound_file);
	this->last_time = GetTime();
	this->time_per_frame = 1.0/this->fps;
}

void AppleExplosion::reset() {
	this->current_frame = 0;
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

void AppleExplosion::explode_sound() {
	this->explosion_sound.Play();
}

void AppleExplosion::update() {
	if (this->current_frame >= this->n_frames - 1) return;
	if (this->current_frame == this->frame_apple_disappear) this->explode_sound();
	if (GetTime() - last_time >= this->time_per_frame) {
		this->current_frame += 1;
		this->last_time = GetTime();
	}
}

PlayerStats::PlayerStats()  {
	this->heart_texture.Load(this->heart_texture_file);
}

void PlayerStats::reset() {
	this->pts = 0;
	this->length = (1 + SNAKE_INITIAL_LENGTH) / SNAKE_UNIT_LENGTH;
	this->lives = LIVES;
}

void PlayerStats::init (raylib::Font* game_font)  {
	this->game_font = game_font;
	this->reset();
}

void PlayerStats::draw_lives(raylib::Vector2 pos) {
	int x_draw = pos.x;
	for (size_t i = 0; i < this->lives; i++){
		x_draw += this->hearts_gaps;
		this->heart_texture.Draw(x_draw, pos.y);
	}
}

void PlayerStats::draw_length(raylib::Vector2 pos) {
	raylib::DrawTextEx(*(this->game_font), "Length: " + to_string(this->length), pos, 50, 1.0, this->text_color);
}

void PlayerStats::draw_pts(raylib::Vector2 pos) {
	raylib::DrawTextEx(*(this->game_font), "Pts: " + to_string(this->pts), pos, 50, 1.0, this->text_color);
}

void MathQuestion::draw_question(raylib::Vector2 pos, raylib::Font* game_font, Color color) {
	raylib::DrawTextEx(*game_font, this->display, pos, 60, 1.0, color);
}

optional<Food> MathQuestion::check_collision(raylib::Vector2 head_location, float head_radius) {
	for (size_t i = 0; i < 3; i++) {
		if (!CheckCollisionCircles(this->coords[i].to_vector2_center(), head_radius*2, head_location, head_radius)) continue;
		if (i == 0) return get_food(this->difficulty); else return BAD_APPLE;
	}
	return nullopt;
}

MathQuestionDisplay::MathQuestionDisplay() {
}

void MathQuestionDisplay::init(raylib::Font* game_font) {
	this->game_font = game_font;
	this->reset();
}

void MathQuestionDisplay::reset() {
	this->generate_new_question();
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
		this->q_now.difficulty = ADD_SUB_EASY;

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

		// opsi ke-3 yang susah: satuannya dibalik (buat pengurangan) ->
		// menjebak kalau tadi penambahan kita balik aja `diff_tens`
		// yang sebelumnya. **Tapi kalau satuan lhs sama dengan satuan
		// rhs, ini bakal sama kayak jawaban aslinya. in that case, kita
		// balik diff_tens kayak pertambahan.
		this->q_now.answers[2] = add ? (lhs+rhs - diff_tens) : (rhs_ones == lhs_ones ? (lhs+rhs - diff_tens) : ((lhs_tens + rhs_ones) - (rhs_tens + lhs_ones)));

		this->q_now.difficulty = ADD_SUB_HARD;


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

		this->q_now.difficulty = MULT_DIV_EASY;

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

		this->q_now.difficulty = MULT_DIV_HARD;
		
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
		this->q_now.difficulty = POW_SQRT_EASY;

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
		this->q_now.difficulty = POW_SQRT_HARD;
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
		this->q_now.coords_pixel[i] = q_now.coords[i].to_vector2().Add({static_cast<float>(((TILE_DIMENSION - q_now.answers_str[i].length()*12)/2)), 15});
	}
}

void MathQuestionDisplay::draw_answers() {
	for (size_t i = 0; i < 3; i++) {
		raylib::DrawTextEx(*(this->game_font), this->q_now.answers_str[i], this->q_now.coords_pixel[i], 30, 1.0, this->food_color);
	}
}

StatusBar::StatusBar() {
	this->texture.Load(this->texture_file);
	this->apple_texture.Load(this->apple_texture_file);
	this->bad_apple_texture.Load(this->bad_apple_texture_file);
	this->pie_texture.Load(this->pie_texture_file);
	this->golden_pie_texture.Load(this->golden_pie_texture_file);
}

void StatusBar::reset() {
	this->pos = raylib::Vector2 {0, this->min_statusbar_pos_y};
}

void StatusBar::fall() {
	this->pos = raylib::Vector2 {0, this->min_statusbar_pos_y};
	this->is_falling = true;
}

void StatusBar::init(raylib::Font* game_font, MathQuestion* math_question, PlayerStats* stats) {
	this->game_font = game_font;
	this->math_question = math_question;
	this->player_stats = stats;
	this->reset();
}

void StatusBar::draw() {
	this->texture.Draw(this->pos);
}

void StatusBar::draw_stats() {
	this->player_stats->draw_lives(this->pos + this->lives_pos);
	this->player_stats->draw_pts(this->pos + this->pts_pos);
	this->player_stats->draw_length(this->pos + this->snake_length_pos);
}

void StatusBar::draw_question() {
	this->math_question->draw_question(this->pos + this->question_pos, this->game_font, this->math_question_color);
}

void StatusBar::draw_feedback(Food food) {
	switch(food) {
		case BAD_APPLE:
			raylib::DrawTextEx(*(this->game_font), "  Wrong!", this->pos + this->question_pos, 50, 1.0, this->math_feedback_color_wrong);
			this->bad_apple_texture.Draw(this->pos + this->answer_food_tex_pos);
			break;

		case APPLE:
			raylib::DrawTextEx(*(this->game_font), "Correct!", this->pos + this->question_pos, 50, 1.0, this->math_feedback_color_correct);
			this->apple_texture.Draw(this->pos + this->answer_food_tex_pos);
			break;

		case APPLE_PIE:
			raylib::DrawTextEx(*(this->game_font), "Correct!", this->pos + this->question_pos, 50, 1.0, this->math_feedback_color_correct);
			this->pie_texture.Draw(this->pos + this->answer_food_tex_pos);
			break;

		case GOLDEN_APPLE_PIE:
			raylib::DrawTextEx(*(this->game_font), "Correct!", this->pos + this->question_pos, 50, 1.0, GOLD);
			this->golden_pie_texture.Draw(this->pos + this->answer_food_tex_pos);
			break;

		default:
			break;
	}
}

void StatusBar::update() {
	if (this->is_falling){
		if (this->pos.y >= this->pos_y) this->is_falling = false;
		this->pos.y += this->vy;
	}
}

Player::Player() {
	this->snake_head_u.Load(this->texture_snake_head_u);
	this->snake_head_l.Load(this->texture_snake_head_l);
	this->snake_head_d.Load(this->texture_snake_head_d);
	this->snake_head_r.Load(this->texture_snake_head_r);
	this->turn_sound.Load(this->turn_sound_file);
	this->reset();
}

void Player::reset() {
	this->game_over = FALSE;
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

void Player::add_length() {
	raylib::Vector2 last_vector = this->points[this->points.size()-1];
	for (size_t i = 0; i < SNAKE_UNIT_LENGTH; i++){
		this->points.push_back(last_vector);
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

raylib::Vector2 Player::get_head_pos_center() {
	float div_2 = TILE_DIMENSION/2.0; // cast ke float di awal
	return raylib::Vector2 { this-> head_pos.x + div_2, this->head_pos.y + div_2 };
}

bool Player::check_collision_self() {
	size_t points_size = this->points.size();
	size_t init = ceil(TILE_DIMENSION/2.0/this->snake_point_radius);
	for (size_t i = init; i < points_size; i++) {
		if (CheckCollisionCircles(get_head_pos_center(), this->snake_body_radius, this->points[i], this->snake_body_radius)) return true;
	}
	return false;
}

bool Player::check_collision_corners() {
	return (this->head_pos.x < 0 || this->head_pos.x + TILE_DIMENSION > TILE_COLUMNS * TILE_DIMENSION || this->head_pos.y < TILE_DIMENSION || this->head_pos.y > TILE_ROWS * TILE_DIMENSION);
}

void Player::check_collision(bool* play_bgmusic) {
	if (this->check_collision_self()) {
		this->game_over = CRASH_SELF;
		*play_bgmusic = false;
		this->active = false;
		this->lose_sound->Play();
	}
	if (this->check_collision_corners()) {
		this->game_over = CRASH_WALL;
		*play_bgmusic = false;
		this->active = false;
		this->lose_sound->Play();
	};
}

void Player::init(raylib::Sound* lose_sound, bool* play_bgmusic) {
	this->lose_sound = lose_sound;
	this->play_bgmusic = play_bgmusic;
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
		this->check_collision(this->play_bgmusic);
	}

	// Hanya terjadi sekali tiap main.
	if (this->head_pos.x <= TILE_COLUMNS * TILE_DIMENSION - TILE_DIMENSION) {
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

	// Aku tidak bisa memutuskan
	// Uncomment kode di bawah ini untuk membuat ularnya bergerak sesuai tiles:
	// if (int(floor(this->head_pos.y)) % TILE_DIMENSION == 0
	// && int(floor(this->head_pos.x)) % TILE_DIMENSION == 0) {
	this->unqueue_turn();
	// }

	this->move();
}

void Player::draw() {
	// Gambar tubuh
	size_t points_size = points.size();
	DrawLineStrip(this->points.data(), this->points.size(), this->snake_color);

	for (size_t i = points_size - 1; i >= 1; i--) {
		if (i == 8 || i == 9 || i == 14 || i == 15) { // gambar pola
			DrawCircle(this->points[i].x, this->points[i].y, this->snake_body_radius, this->snake_color_pattern);
		} else DrawCircle(this->points[i].x, this->points[i].y, this->snake_body_radius, this->snake_color);
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
	this->dead_bad_food_texture.Load(this->dead_bad_food_file);
	this->dead_crash_self_texture.Load(this->dead_crash_self_file);
	this->dead_crash_wall_texture.Load(this->dead_crash_wall_file);
	this->dead_popup_x = (SCREEN_WIDTH - this->dead_popup_dimension_x)/2;
	this->dead_popup_y = (SCREEN_HEIGHT - this->dead_popup_dimension_y)/2;
	this->bad_apple_sound.Load(this->bad_apple_sound_file);
	this->correct_apple_sound.Load(this->correct_apple_sound_file);
	this->heavenly_choir_sound.Load(this->heavenly_choir_sound_file);
	this->lose_sound.Load(this->lose_sound_file);
}

void GameScene::reset(bool apple_explode) {
	if (apple_explode){
		this->explosion_animation.reset();
	}
	this->player.reset();
	this->player.create_snake();
	this->status_bar.player_stats->reset();
	this->status_bar.reset();
	this->is_game_started = false;
	this->math_status = QUESTION;
	this->math.reset();
	*this->play_bgmusic = true;
}

void GameScene::init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager, function<void()> menu_callback, bool* play_bgmusic) {
	this->math.init(game_font);
	this->menu_callback = menu_callback;
	this->status_bar.init(game_font, &this->math.q_now, &this->player_stats);
	this->player_stats.init(game_font);
	this->play_bgmusic = play_bgmusic;
	this->player.init(&this->lose_sound, this->play_bgmusic);
	this->game_state_manager = game_state_manager;
	this->reset(true);
}

void GameScene::food_check() {
	optional<Food> food_collided = this->math.q_now.check_collision(this->player.get_head_pos_center(), this->player.snake_body_radius);
	if (food_collided.has_value()){
		Food food = food_collided.value();
		if (food == BAD_APPLE) {
			this->bad_apple_sound.Play();
		} else if (food == GOLDEN_APPLE_PIE) {
			this->heavenly_choir_sound.Play();
		} else {
			this->correct_apple_sound.Play();
		}

		long long pts = get_pts(food_collided.value());
		this->last_food = food_collided.value();
		this->math_status = FEEDBACK;
		this->game_state_manager->timer.attach(3.0, [this](){this->math_status = QUESTION;});

		// Jangan bikin skor negatif
		this->player_stats.pts = max(static_cast<long long>(0), this->player_stats.pts + pts);
		if (pts > 0) {
			this->player_stats.length += 1;
			this->player.add_length();
		} else {
			this->player_stats.lives -= 1;
			if (this->player_stats.lives == 0) {
				this->draw(); // buat update statusbar untuk terakhir kalinya
				this->lose_sound.Play();
				*this->play_bgmusic = false;
				this->player.game_over = BAD_FOOD;
			}
		}
		this->math.generate_new_question();
	}
}

void GameScene::update() {
	if (this->is_game_started) {
		switch (this->player.game_over) {
			case FALSE:
				this->status_bar.update();
				this->player.update();

				// Kalau kita menabrak makanan. Hanya cek ketika pertanyaan sudah
				// terlihat.
				if (this->math_status == QUESTION) this->food_check();
				break;
			case CRASH_SELF:
			case BAD_FOOD:
			case CRASH_WALL:
				if (IsKeyPressed(KEY_R)) {
					this->reset(false);
				} else if (IsKeyPressed(KEY_M)) {
					this->menu_callback();
				}
				break;
			default:
				break;
		}
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
		this->status_bar.draw_stats();

		switch (this->player.game_over) {
			case FALSE:
				switch(this->math_status) {
					case QUESTION:
						this->status_bar.draw_question();
						this->math.draw_answers();
					break;
					case FEEDBACK:
						this->status_bar.draw_feedback(this->last_food);
					break;
				}

				this->player.draw();
				break;

			case CRASH_SELF:
				this->dead_crash_self_texture.Draw(this->dead_popup_x, this->dead_popup_y);
				break;
			case BAD_FOOD:
				this->dead_bad_food_texture.Draw(this->dead_popup_x, this->dead_popup_y);
				break;
			case CRASH_WALL:
				this->dead_crash_wall_texture.Draw(this->dead_popup_x, this->dead_popup_y);
				break;
			default:
				break;

		}
	} else {
		this->explosion_animation.show_apple()
			? this->ground_texture_apple.Draw(0, 0)
			: this->ground_texture.Draw();
		this->explosion_animation.draw();
	}
}
