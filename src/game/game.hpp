#include "../backend/components.hpp"
#include "../prelude.hpp"
#include "scene_intro.hpp"
#include "scene_interlude.hpp"

#define FONT_PATH "assets/delius-font/delius-regular.ttf"

class Game {
private:
	GameComponents::GameStateManager game_state_manager;
	raylib::Font game_font;

	raylib::Window* window;
	IntroScene intro_scene;
	InterludeScene interlude_scene;

public:
	Game();
	void update();
	void draw();
	void start_game_interlude();
	void start_game();
};
