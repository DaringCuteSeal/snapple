#include "../prelude.hpp"
#include "../backend/components.hpp"
#include <cstdint>

#define TILE_ROWS 17
#define TILE_COLUMNS 32
#define TILE_DIMENSION 60 // sisi segiempat, dalam piksel

using std::pair;

// Koordinat (dalam tile, bukan piksel)
struct TileCoord {
	int row;
	int col;

	int row_pixels();
	int col_pixels();
};

class AppleExplosion {
	Texture2D texture;
	const uint fps = 13;
	const GameComponents::Coordinate pos = {185, 790};
	static const size_t n_frames = 17;
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
	void draw();
	void update();
};

class PlayerStats {
private:
	const int hearts_gaps = 15;

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
	void update();
};

struct MathQuestion {
	// Pertanyaan yang ditampilkan
	// Kita tidak menyimpan angka-angka aslinya karena tidak penting dan
	// polymorphism itu ribet.
	string display;

	// Jawaban-jawaban pertanyaan.
	// Untuk memudahkan, jawaban benarnya ada di answers[0]. Semua jawaban
	// di sini berbeda (harus digaransikan ketika generasi pertanyaan). Array ini
	// menyimpan koordinat jawabannya juga (untuk digunakan ketika render).
	pair<TileCoord, long long> answers[3];
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

public:
	MathQuestionDisplay();

	// Generate sebuah pertanyaan dan simpan ke state `this->q_now`.
	void generate_new_question();

	// Dapatkan pointer ke array 3 jawaban yang memungkinkan.
	// Digunakan untuk collision checking.
	MathQuestion* get_question();
	void draw_bar_item();
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
	void draw();
	void update();
};

class Player {
private:
	// Titik-titik yang menjadi tubuh ular pengguna.
	vector<Vector2> points;

	// Posisi titik-titik (koordinat tile, bukan dalam piksel).
	vector<TileCoord> pos;
public:
	void update();
	void draw();
};

class GameScene : public GameComponents::Scene {
private:
	AppleExplosion explosion_animation;
	raylib::Texture2D ground_texture;
	bool is_game_started = false;
	StatusBar status_bar;
	PlayerStats player_stats;
	MathQuestionDisplay math;


public:
	GameScene();
	void draw();
	void update();
};
