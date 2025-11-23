#include "../prelude.hpp"
#include "../backend/components.hpp"
#include "../cfg.hpp"
#include <cstdint>
#include <optional>

#define LIVES 3
#define SNAKE_INITIAL_LENGTH 50 - 1 // gak termasuk kepala
#define SNAKE_UNIT_LENGTH 5 // banyak panjang untuk ditambah ketika makan angka
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

// Jenis soal yang dikerjakan
enum Difficulty {
	ADD_SUB_EASY,
	ADD_SUB_HARD,
	MULT_DIV_EASY,
	MULT_DIV_HARD,
	POW_SQRT_EASY,
	POW_SQRT_HARD,
};

enum Food {
	BAD_APPLE,
	APPLE,
	APPLE_PIE,
	GOLDEN_APPLE_PIE
};

enum GameOver {
	FALSE,
	CRASH_WALL,
	CRASH_SELF,
	BAD_FOOD,
};

Food get_food(Difficulty difficulty);
long long get_pts(Food food);

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

	// Putar ulang
	void reset();
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
	raylib::Font* game_font; // harus diinisialisasi

	// Sisa nyawa pemain.
	uint8_t lives = LIVES;

	// TODO: atau ini harusnya ga variabel sendiri, biar kita bisa
	// player_segments.length() gitu? tapi kayaknya gak optimal kalau gitu karena
	// variabel ini dibaca setiap update() scene.
	uint length = (1 + SNAKE_INITIAL_LENGTH) / SNAKE_UNIT_LENGTH;

	// Skor pemain.
	long long pts = 0;

	void reset();
	void turn(Direction direction);
	void init(raylib::Font* game_font);
	void draw_lives(raylib::Vector2 pos);
	void draw_pts(raylib::Vector2 pos);
	void draw_length(raylib::Vector2 pos);
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
	raylib::Vector2 coords_pixel[3];

	// Jawaban (dalam string; untuk display);
	string answers_str[3];

	// Kesulitan (buat variasi poin)
	Difficulty difficulty;

	// Tulis pertanyaannya.
	void draw_question(raylib::Vector2 pos, raylib::Font* game_font, Color color);

	// Cek tabrakan dengan makanan.
	optional<Food> check_collision(raylib::Vector2 head_location, float head_radius);
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

	const Color food_color = WHITE;
	raylib::Font* game_font;

public:
	MathQuestionDisplay();

	// Pertanyaan sekarang. Bisa digunakan sebagai pointer untuk object lain.
	// (Tidak pernah invalid selama kelas ini masih valid).
	MathQuestion q_now;

	void init(raylib::Font* game_font);
	void reset();

	// Generate sebuah pertanyaan dan simpan ke state `this->q_now`.
	void generate_new_question();

	// Gambar jawaban-jawaban
	void draw_answers();
};

enum MathDisplayStatus {
	QUESTION,
	FEEDBACK
};

class StatusBar {
private:
	// Koordinat komponen-komponen (relatif dengan `this->pos`).
	const raylib::Vector2 lives_pos = {0, 10};
	const raylib::Vector2 pts_pos = {330, 3};
	const raylib::Vector2 question_pos = {815, 3};
	const raylib::Vector2 snake_length_pos = {1400, 3};
	const raylib::Vector2 answer_food_tex_pos = {1000, -2};

	const char* apple_texture_file = "assets/apple.png";
	const char* bad_apple_texture_file = "assets/apple_bad.png";
	const char* pie_texture_file = "assets/pie.png";
	const char* golden_pie_texture_file = "assets/golden_pie.png";

	raylib::Texture2D apple_texture;
	raylib::Texture2D bad_apple_texture;
	raylib::Texture2D pie_texture;
	raylib::Texture2D golden_pie_texture;

	const Color math_question_color = ORANGE;
	const Color math_feedback_color_correct = GREEN;
	const Color math_feedback_color_wrong = ORANGE;

	// Posisi untuk menyembunyikan statusbar.
	const float min_statusbar_pos_y = -80;

	// Posisi seharusnya
	const int pos_y = -5;

	const int vy = 3;

	const char* texture_file = "assets/statusbar.png";
	raylib::Texture2D texture;

	bool is_falling = false;

	// Pertanyaan sekarang.
	MathQuestion* math_question;

	raylib::Font* game_font;

public:
	// Statistik pengguna.
	PlayerStats* player_stats;

	// Posisi statusbar.
	raylib::Vector2 pos = {0, 0};

	StatusBar();
	void reset();
	void init(raylib::Font* game_font, MathQuestion* math_question, PlayerStats* player_stats);

	// Gambar bar
	void draw();

	// Tulis statistik ke bar
	void draw_stats();

	// Tulis pertanyaan matematika ke bar
	void draw_question();

	// Tulis feedback jawaban ke bar
	void draw_feedback(Food food);
	void update();
	void fall();
};

class Player {
private:
	/** Radius titik-titik ular. 
	 * Catatan: harus merupakan faktor dari TILE_DIMENSION.
	*/
	const float snake_point_radius = TILE_DIMENSION / 15.0;

	const Color snake_color = Color { .r = 87, .g = 141, .b = 237, .a = 255 };
	const Color snake_color_pattern = Color { .r = 255, .g = 237, .b = 165, .a = 255 };

	// Titik-titik yang menjadi tubuh ular pengguna (tidak termasuk kepala).
	vector<Vector2> points;

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

	// Cek tabrakan dengan makanan dan tambah poin, panjang ular, dsb. jika ya.
	void check_collision_food();

	// Posisi awal ular
	TileCoord initial_pos = {3, TILE_COLUMNS + 1};

	// Bisa dikendalikan atau tidak?
	// Ini akan dirubah ketika intro ular masuk selesai.
	void move(); // ular bergerak sebesar v
	
	// Cek collision atau tidak
	bool try_check_collision = false;

	bool active = true;

public:
	GameOver game_over;

	const float snake_body_radius = TILE_DIMENSION / 5.0;

	bool controllable = false;
	
	void reset();

	// Posisi kepala.
	raylib::Vector2 head_pos;

	Player();
	raylib::Vector2 get_head_pos_center();
	void init();
	void create_snake();
	void check_collision();
	void update();
	void draw();
	void unqueue_turn();
	void add_length();
};

class GameScene : public GameComponents::Scene {
private:
	AppleExplosion explosion_animation;
	const char* dead_crash_wall_file = "assets/dead_crash_wall.png";
	const char* dead_crash_self_file = "assets/dead_crash_self.png";
	const char* dead_bad_food_file = "assets/dead_bad_food.png";
	const char* ground_texture_file = "assets/ground.png";
	const char* ground_texture_apple_file = "assets/interlude_animation/37.png";

	const size_t dead_popup_dimension_y = 587;
	const size_t dead_popup_dimension_x = 970;

	size_t dead_popup_y; // Diitung ketika konstruksi kelas
	size_t dead_popup_x; // Diitung ketika konstruksi kelas

	raylib::Texture2D dead_crash_wall_texture;
	raylib::Texture2D dead_crash_self_texture;
	raylib::Texture2D dead_bad_food_texture;
	raylib::Texture2D ground_texture;
	raylib::Texture2D ground_texture_apple;
	bool is_game_started = false;
	StatusBar status_bar;
	PlayerStats player_stats;
	MathQuestionDisplay math;
	Player player;
	raylib::Font* game_font;
	GameComponents::GameStateManager* game_state_manager;

	// Status dari pertanyaan matematika.
	MathDisplayStatus math_status;

	// Makanan terakhir yang didapat.
	Food last_food;

	// Callback untuk balik ke main menu game.
	function<void()> menu_callback;

public:
	GameScene();
	void reset(bool apple_explode);
	void init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager, function<void()> menu_callback);
	void draw();
	void update();
	void food_check();
};
