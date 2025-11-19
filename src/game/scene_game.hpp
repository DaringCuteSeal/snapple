#include "../prelude.hpp"
#include "../backend/components.hpp"
#include <cstdint>

#define TILE_ROWS = 17
#define TILE_COLUMNS = 32
#define TILE_DIMENSION = 60 // sisi segiempat, dalam piksel

using std::pair;

// Koordinat (dalam tile, bukan piksel)
struct TileCoord {
	int row;
	int col;

	int row_pixels();
	int col_pixels();
};

class PlayerStats {
private:
	raylib::Texture2D heart_texture;
public:
	// Sisa nyawa pemain.
	uint8_t lives = 3;

	// TODO: atau ini harusnya ga variabel sendiri, biar kita bisa
	// player_segments.length() gitu? tapi kayaknya gak optimal kalau gitu karena
	// variabel ini dibaca setiap update() scene.
	uint length = 3;

	// Skor pemain.
	long long score = 0;

	void draw();
	void update();
};

struct MathQuestion {
	// Pertanyaan yang ditampilkan
	// Kita tidak menyimpan angka-angka aslinya karena tidak penting dan
	// polymorphism itu ribet.
	string display;

	// Jawaban-jawaban pertanyaan.
	// Untuk memudahkan, jawaban benarnya ada di answers[0].
	// Array ini menyimpan koordinat jawabannya juga (untuk digunakan ketika
	// render).
	pair<TileCoord, long long> answers[3];
};

/** Kelas yang mengatur display matematika. Ada bagian untuk widget di bar dan
 * jawaban yang bisa "dimakan" pengguna.
 */
class MathQuestionDisplay {
private:
	MathQuestion q_now;
	const Color BAR_COLOR = ORANGE;
	const Color FOOD_COLOR = WHITE;
public:
	// Generate sebuah pertanyaan dan simpan ke state `this->q_now`.
	void generate_new_question();

	// Dapatkan pointer ke array 3 jawaban yang memungkinkan.
	// Digunakan untuk collision checking.
	pair<GameComponents::Coordinate, long long>* get_result();
	void draw_bar_item();
	void draw_answers();
};

class StatusBar {
private:
	const GameComponents::Coordinate lives_pos = {20, 20};
	const GameComponents::Coordinate pts_pos = {20, 280};
	const GameComponents::Coordinate question_pos = {20, 815};
	const GameComponents::Coordinate snake_length_pos = {20, 1551};

	PlayerStats stats;

public:
	StatusBar();
	void draw();
	void update();
};

class GameScene : public GameComponents::Scene {
private:

public:
	GameScene();
	void draw();
	void update();
};
