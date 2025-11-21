#include "../prelude.hpp"
#include "../backend/components.hpp"
#include <cstdint>

enum TextState {
	NONE,
	BOUNCE_DOWN,
	FLY_UP
};

class LetterSprite : GameComponents::Sprite {
private:
	const uint8_t max_bounces = 3;
	const double COR = 0.45; // koefisien restitusi (vy setelah mantul ÷ vy sebelum mantul)
	double ay = 0.8; // percepatan sumbu y
	raylib::Texture2D texture;
	int ground_y;
	double vy; // kecepatan sumbu y
	uint8_t bounces;
	TextState state;
public:
	GameComponents::Coordinate pos;

	LetterSprite();

	void bounce_down();
	void fly_up(double velocity);

	// TODO: ini harusnya ada di constructor. tapi gara gara ada kelas lain
	// yang ada field kelas ini, jadinya harus di construct langsung. tapi
	// kalo aku taro di constructor nanti malah kelas lain itu gabisa di
	// construct -_-
	void load_texture(const char* letter_file_name);
	void set_ground(size_t y);
	void draw();
	void update();
};

class IntroScene : public GameComponents::Scene {
private:
	const char* id = "intro";
	const uint fps = 700;
	static const size_t n_frames = 141;
	const size_t start_loop = 119; // mulai loop sampe pemain mulai game
	const char* frames[n_frames] = {
		"assets/intro_animation/0.png",
		"assets/intro_animation/1.png",
		"assets/intro_animation/2.png",
		"assets/intro_animation/3.png",
		"assets/intro_animation/4.png",
		"assets/intro_animation/5.png",
		"assets/intro_animation/6.png",
		"assets/intro_animation/7.png",
		"assets/intro_animation/8.png",
		"assets/intro_animation/9.png",
		"assets/intro_animation/10.png",
		"assets/intro_animation/11.png",
		"assets/intro_animation/12.png",
		"assets/intro_animation/13.png",
		"assets/intro_animation/14.png",
		"assets/intro_animation/15.png",
		"assets/intro_animation/16.png",
		"assets/intro_animation/17.png",
		"assets/intro_animation/18.png",
		"assets/intro_animation/19.png",
		"assets/intro_animation/20.png",
		"assets/intro_animation/21.png",
		"assets/intro_animation/22.png",
		"assets/intro_animation/23.png",
		"assets/intro_animation/24.png",
		"assets/intro_animation/25.png",
		"assets/intro_animation/26.png",
		"assets/intro_animation/27.png",
		"assets/intro_animation/28.png",
		"assets/intro_animation/29.png",
		"assets/intro_animation/30.png",
		"assets/intro_animation/31.png",
		"assets/intro_animation/32.png",
		"assets/intro_animation/33.png",
		"assets/intro_animation/34.png",
		"assets/intro_animation/35.png",
		"assets/intro_animation/36.png",
		"assets/intro_animation/37.png",
		"assets/intro_animation/38.png",
		"assets/intro_animation/39.png",
		"assets/intro_animation/40.png",
		"assets/intro_animation/41.png",
		"assets/intro_animation/42.png",
		"assets/intro_animation/43.png",
		"assets/intro_animation/44.png",
		"assets/intro_animation/45.png",
		"assets/intro_animation/46.png",
		"assets/intro_animation/47.png",
		"assets/intro_animation/48.png",
		"assets/intro_animation/49.png",
		"assets/intro_animation/50.png",
		"assets/intro_animation/51.png",
		"assets/intro_animation/52.png",
		"assets/intro_animation/53.png",
		"assets/intro_animation/54.png",
		"assets/intro_animation/55.png",
		"assets/intro_animation/56.png",
		"assets/intro_animation/57.png",
		"assets/intro_animation/58.png",
		"assets/intro_animation/59.png",
		"assets/intro_animation/60.png",
		"assets/intro_animation/61.png",
		"assets/intro_animation/62.png",
		"assets/intro_animation/63.png",
		"assets/intro_animation/64.png",
		"assets/intro_animation/65.png",
		"assets/intro_animation/66.png",
		"assets/intro_animation/67.png",
		"assets/intro_animation/68.png",
		"assets/intro_animation/69.png",
		"assets/intro_animation/70.png",
		"assets/intro_animation/71.png",
		"assets/intro_animation/72.png",
		"assets/intro_animation/73.png",
		"assets/intro_animation/74.png",
		"assets/intro_animation/75.png",
		"assets/intro_animation/76.png",
		"assets/intro_animation/77.png",
		"assets/intro_animation/78.png",
		"assets/intro_animation/79.png",
		"assets/intro_animation/80.png",
		"assets/intro_animation/81.png",
		"assets/intro_animation/82.png",
		"assets/intro_animation/83.png",
		"assets/intro_animation/84.png",
		"assets/intro_animation/85.png",
		"assets/intro_animation/86.png",
		"assets/intro_animation/87.png",
		"assets/intro_animation/88.png",
		"assets/intro_animation/89.png",
		"assets/intro_animation/90.png",
		"assets/intro_animation/91.png",
		"assets/intro_animation/92.png",
		"assets/intro_animation/93.png",
		"assets/intro_animation/94.png",
		"assets/intro_animation/95.png",
		"assets/intro_animation/96.png",
		"assets/intro_animation/97.png",
		"assets/intro_animation/98.png",
		"assets/intro_animation/99.png",
		"assets/intro_animation/100.png",
		"assets/intro_animation/101.png",
		"assets/intro_animation/102.png",
		"assets/intro_animation/103.png",
		"assets/intro_animation/104.png",
		"assets/intro_animation/105.png",
		"assets/intro_animation/106.png",
		"assets/intro_animation/107.png",
		"assets/intro_animation/108.png",
		"assets/intro_animation/109.png",
		"assets/intro_animation/110.png",
		"assets/intro_animation/111.png",
		"assets/intro_animation/112.png",
		"assets/intro_animation/113.png",
		"assets/intro_animation/114.png",
		"assets/intro_animation/115.png",
		"assets/intro_animation/116.png",
		"assets/intro_animation/117.png",
		"assets/intro_animation/118.png",
		"assets/intro_animation/119.png",
		"assets/intro_animation/120.png",
		"assets/intro_animation/121.png",
		"assets/intro_animation/122.png",
		"assets/intro_animation/123.png",
		"assets/intro_animation/124.png",
		"assets/intro_animation/125.png",
		"assets/intro_animation/126.png",
		"assets/intro_animation/127.png",
		"assets/intro_animation/128.png",
		"assets/intro_animation/129.png",
		"assets/intro_animation/130.png",
		"assets/intro_animation/131.png",
		"assets/intro_animation/132.png",
		"assets/intro_animation/133.png",
		"assets/intro_animation/134.png",
		"assets/intro_animation/135.png",
		"assets/intro_animation/136.png",
		"assets/intro_animation/137.png",
		"assets/intro_animation/138.png",
		"assets/intro_animation/139.png",
		"assets/intro_animation/140.png",
	};

	raylib::Texture2D intro_animation[n_frames];

	static const size_t n_letters = 7;
	const char* letters[n_letters] = {
		"assets/title_S.png",
		"assets/title_N.png",
		"assets/title_A.png",
		"assets/title_P1.png",
		"assets/title_P2.png",
		"assets/title_L.png",
		"assets/title_E.png"
	};

	const int ground_y[n_letters] = {
		56,
		171,
		81,
		140,
		178,
		108,
		112
	};

	const int letters_x_pos[n_letters] = {
		274,
		523,
		735,
		953,
		1077,
		1214,
		1370
	};

	const int letters_y_min = -500;
	const double letters_v_up = 2;

	LetterSprite letter_sprites[n_letters];

	const char* background_filename = "assets/background.png";
	raylib::Texture2D background;
	double time_per_frame;
	size_t current_frame = 0;
	double last_time;
	bool is_looping; // Must be set to false to restart TODO: write the restart method
	bool ready_to_start_game; // Must be set to false to restart
	bool show_hint_text; // Must be set to false to restart
	bool active; // Must be set to true to restart
	uint8_t hint_text_alpha;
	double hint_text_last_time;
	const double hint_text_blink_duration = 1.0;
	function<void()> start_interlude_callback;
	raylib::Font* game_font;
	GameComponents::GameStateManager* game_state_manager;

public:
	IntroScene();

	// gara gara kalo ada kelas yang ada kelas lain, maka kelas yang
	// dimiliki kelas lain itu harus ada default constructor, makanya aku
	// jadi pake fungsi init kayak gini. ini gunanya buat dependency
	// injection doang.
	//
	// *perhatian*: fungsi ini HARUS dipanggil SETELAH
	// konstruksi dan harus diberi argumen non-null! ! !
	void init(raylib::Font* game_font, GameComponents::GameStateManager* game_state_manager, function<void()> start_game_callback);

	// Bikin tulisan judul membal ke bawah (sampai `ground_y`).
	void letters_bounce_down();

	// Bikin tulisan judul terbang ke atas (sampai `letters_y_min`).
	void letters_fly_up();

	void draw();
	void update();
};
