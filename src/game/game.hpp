#include "../backend/components.hpp"
#include "../prelude.hpp"

#include "scene_intro.hpp"
#include "scene_interlude.hpp"
#include "scene_game.hpp"

#define FONT_PATH "assets/delius-font/delius-regular.ttf"

/** Kelas utama untuk game. Menampung scene-scene yang ditampilkan pada game.
*/
class Game {
private:
	GameComponents::GameStateManager game_state_manager;
	raylib::Font game_font;

	raylib::Window* window;
	IntroScene intro_scene;
	GameScene game_scene;
	InterludeScene interlude_scene;

	const char* music_file = "assets/byte-blast.mp3";
	raylib::Sound background_music;
	bool play_bgmusic = true;

public:
	Game();
	/** Update state game.
	*/
	void update();
	/** Gambar state game.
	*/
	void draw();
	/** Mulai interlude game (di tengah intro dan game).
	*/
	void start_game_interlude();
	/** Mulai game.
	*/
	void start_game();
	/** Kembali ke main menu game.
	*/
	void go_back_to_menu();
};
