#include "../prelude.hpp"
#include "../backend/components.hpp"
#include <cstdint>
#include <optional>

#define SNAKE_INITIAL_LENGTH 49 // gak termasuk kepala
#define SNAKE_LENGTH_FACTOR 5 // banyak panjang untuk ditambah ketika makan angka
                                                                   // haha makan angka huh ..
#define TILE_ROWS 17
#define TILE_COLUMNS 32
#define TILE_DIMENSION 60 // sisi segiempat, dalam piksel

using std::pair, std::optional, std::nullopt;

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
	raylib::Vector2 to_vector2_center();
	raylib::Vector2 to_vector2();
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
	const int hearts_gaps = TILE_DIMENSION + 15;
	const Color text_color = WHITE;

	const char* heart_texture_file = "assets/heart.png";
	raylib::Texture2D heart_texture;
public:
	PlayerStats();
	raylib::Font* game_font;

	// Sisa nyawa pemain.
	uint8_t lives = 3;

	// TODO: atau ini harusnya ga variabel sendiri, biar kita bisa
	// player_segments.length() gitu? tapi kayaknya gak optimal kalau gitu karena
	// variabel ini dibaca setiap update() scene.
	uint length = (1 + SNAKE_INITIAL_LENGTH) / SNAKE_LENGTH_FACTOR;

	// Skor pemain.
	long long pts = 0;

	void turn(Direction direction);
	void init(raylib::Font* game_font);
	void draw_lives(int x, int y);
	void draw_pts(int x, int y);
	void draw_length(int x, int y);
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

	// Tile tile jawaban.
	TileCoord coords[3];

	// Tile tile jawaban (dalam koordinat piksel).
	GameComponents::Coordinate coords_pixel[3];

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
	const GameComponents::Coordinate lives_pos = {10, 0};
	const GameComponents::Coordinate pts_pos = {3, 330};
	const GameComponents::Coordinate question_pos = {3, 815};
	const GameComponents::Coordinate snake_length_pos = {3, 1400};

	// Statistik pengguna.
	PlayerStats stats;

	// Posisi untuk menyembunyikan statusbar.
	const int min_statusbar_pos_y = -80;

	// Posisi seharusnya
	const int pos_y = -5;

	const int vy = 3;

	// Posisi statusbar.
	GameComponents::Coordinate pos = {0, 0};

	const char* texture_file = "assets/statusbar.png";
	raylib::Texture2D texture;

	bool is_falling = false;

public:
	MathQuestionDisplay math;

	StatusBar();
	void init(raylib::Font* game_font);
	void draw();
	void update();
	void draw_math_answers();
	void fall();
};

class Player {
private:
	/** Radius titik-titik ular. 
	 * Catatan: harus merupakan faktor dari TILE_DIMENSION.
	*/
	const float snake_point_radius = TILE_DIMENSION / 15.0;

	const float snake_body_radius = TILE_DIMENSION / 5.0;

	const Color snake_color = Color { .r = 87, .g = 141, .b = 237, .a = 255 };

	// Titik-titik yang menjadi tubuh ular pengguna (tidak termasuk kepala).
	vector<Vector2> points;

	// Posisi kepala.
	raylib::Vector2 head_pos;

	// Arah sekarang (a.k.a arah kepala).
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
	bool check_collision_self();

	// Mengembalikan `true` jika kepala (SEKARANG) sedang bertabrakan dengan
	// dinding.
	bool check_collision_corners();


	// Posisi awal ular
	TileCoord initial_pos = {3, TILE_COLUMNS + 1};

	// Bisa dikendalikan atau tidak?
	// Ini akan dirubah ketika intro ular masuk selesai.
	void move(); // ular bergerak sebesar v

public:
	bool controllable = false;

	Player();
	void create_snake();
	void check_collision();
	void update();
	void draw();
	void unqueue_turn();
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
