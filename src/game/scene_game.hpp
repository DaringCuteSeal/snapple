#include "../prelude.hpp"
#include "../backend/components.hpp"
#include <cstdint>
#include <optional>

#define TILE_ROWS 17
#define TILE_COLUMNS 32
#define TILE_DIMENSION 60 // sisi segiempat, dalam piksel

using std::pair, std::optional;

// Arah
enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// Koordinat (dalam tile, bukan piksel)
struct TileCoord {
	int row;
	int col;

	GameComponents::Coordinate to_coord();
	GameComponents::Coordinate to_coord_center();
};

class AppleExplosion {
	Texture2D texture;
	const uint fps = 13;
	const GameComponents::Coordinate pos = {790, 185};
	static const size_t n_frames = 17;
	const size_t frame_apple_disappear = 7;
	const char* frames[n_frames] = {
		"assets/explosion/0.png",
		"assets/explosion/1.png",
		"assets/explosion/2.png",
		"assets/explosion/3.png",
		"assets/explosion/4.png",
		"assets/explosion/5.png",
		"assets/explosion/6.png",
		"assets/explosion/7.png",
		"assets/explosion/8.png",
		"assets/explosion/9.png",
		"assets/explosion/10.png",
		"assets/explosion/11.png",
		"assets/explosion/12.png",
		"assets/explosion/13.png",
		"assets/explosion/14.png",
		"assets/explosion/15.png",
		"assets/explosion/16.png",
	};

	raylib::Texture2D explosion_animation[n_frames];

	double time_per_frame;
	double last_time;
	size_t current_frame = 0;

public:
	AppleExplosion();

	bool ended();
	bool show_apple();
	void draw();
	void update();
};

class PlayerStats {
private:
	const int hearts_gaps = 15;

	const char* heart_texture_file = "assets/heart.png";
	raylib::Texture2D heart_texture;
public:
	PlayerStats();

	// Sisa nyawa pemain.
	uint8_t lives = 3;

	// TODO: atau ini harusnya ga variabel sendiri, biar kita bisa
	// player_segments.length() gitu? tapi kayaknya gak optimal kalau gitu karena
	// variabel ini dibaca setiap update() scene.
	uint length = 3;

	// Skor pemain.
	long long score = 0;

	void draw_lives(int x, int y);
};

struct MathQuestion {
	// Pertanyaan yang ditampilkan
	// Kita tidak menyimpan angka-angka aslinya karena tidak penting dan
	// polymorphism itu ribet.
	string display;

	// Jawaban-jawaban pertanyaan.
	// Untuk memudahkan, jawaban benarnya ada di answers[0]. Semua jawaban
	// di sini berbeda (harus digaransikan ketika generasi pertanyaan).
	long long answers[3];

	// Array ini
	// menyimpan koordinat jawabannya juga (untuk digunakan ketika render).
	GameComponents::Coordinate coords[3];

	// Jawaban (dalam string; untuk display);
	string answers_str[3];
};

/** Kelas yang mengatur display matematika. Ada bagian untuk widget di bar dan
 * jawaban yang bisa "dimakan" pengguna.
 */
class MathQuestionDisplay {
private:
	// di bawah ini dalam persen semua
	const uint probability_addsubtr_easy = 30;
	const uint probability_addsubtr_hard = 30;
	const uint probability_multdiv_easy = 10;
	const uint probability_multdiv_hard = 10;
	// lucu, kenapa ini yang 15% ? 😭
	const uint probability_powerssqrt_easy = 15;
	const uint probability_powerssqrt_hard = 5;

	MathQuestion q_now;
	const Color bar_color = ORANGE;
	const Color food_color = WHITE;
	raylib::Font* game_font;

public:
	MathQuestionDisplay();
	void init(raylib::Font* game_font);

	// Generate sebuah pertanyaan dan simpan ke state `this->q_now`.
	void generate_new_question();

	// Dapatkan pointer ke array 3 jawaban yang memungkinkan.
	// Digunakan untuk collision checking.
	MathQuestion* get_question();
	void draw_bar_item(int x, int y);
	void draw_answers();
};

class StatusBar {
private:
	// Koordinat komponen-komponen (relatif dengan `this->pos`).
	const GameComponents::Coordinate lives_pos = {20, 20};
	const GameComponents::Coordinate pts_pos = {20, 280};
	const GameComponents::Coordinate question_pos = {20, 815};
	const GameComponents::Coordinate snake_length_pos = {20, 1551};

	// Statistik pengguna.
	PlayerStats stats;

	// Math engine
	MathQuestionDisplay math;

	// Posisi untuk menyembunyikan statusbar.
	const int min_statusbar_pos_y = -80;

	// Posisi seharusnya
	const int pos_y = 0;

	double vy;
	const double a = -0.8;
	

	// Posisi statusbar.
	GameComponents::Coordinate pos = {0, 0};

	raylib::Texture2D texture;

public:
	StatusBar();
	void init(raylib::Font* game_font);
	void draw();
	void update();
	void fall();
};

class Player {
private:
	/** Radius ular. Ini juga digunakan untuk membuat jarak antar titik-titik
	 * pemain.
	*/
	const size_t snake_radius = TILE_DIMENSION / 4;

	// Titik-titik yang menjadi tubuh ular pengguna.
	vector<Vector2> points;

	// Arah-arah dari tiap titik. Sebenarnya gak terlalu berguna untuk
	// titik-titik tengah tapi untuk arah ekor berguna.
	vector<Direction> directions;

	Direction current_direction;
	
	// Arah, jika kita perlu berbelok. Kita perlu queue ini karena belokan hanya
	// dilakukan jika titik tengah dari tekstur
	optional<Direction> turn_queue;

	// Tekstur untuk kepala ular.
	const char* texture_snake_head_u = "assets/snake_head_u.png";
	const char* texture_snake_head_d = "assets/snake_head_d.png";
	const char* texture_snake_head_l = "assets/snake_head_l.png";
	const char* texture_snake_head_r = "assets/snake_head_r.png";
	raylib::Texture2D snake_head_u;
	raylib::Texture2D snake_head_d;
	raylib::Texture2D snake_head_l;
	raylib::Texture2D snake_head_r;
	
	// Mengembalikan `true` jika kepala (SEKARANG) sedang bertabrakan dengan
	// titik-titik tubuh lain.
	bool check_collision_with_self();

	// Bisa dikendalikan atau tidak?
	// Ini akan dirubah ketika intro ular masuk selesai.
	bool controllable = false;

public:
	Player();
	void update();
	void draw();
	void turn(Direction dir);
};

class GameScene : public GameComponents::Scene {
private:
	AppleExplosion explosion_animation;
	const char* ground_texture_file = "assets/ground.png";
	const char* ground_texture_apple_file = "assets/interlude_animation/37.png";
	raylib::Texture2D ground_texture_apple;
	raylib::Texture2D ground_texture;
	bool is_game_started = false;
	StatusBar status_bar;
	PlayerStats player_stats;
	Player player;
	raylib::Font* game_font;
	GameComponents::GameStateManager* game_state_manager;

public:
	GameScene();
	void init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager);
	void draw();
	void update();
};
