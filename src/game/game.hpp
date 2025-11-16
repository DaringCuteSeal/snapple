#include "../backend/components.hpp"
#include "../prelude.hpp"
#include "intro_scene.hpp"

class Game {
private:
	GameComponents::GameStateManager game_state_manager;
	raylib::Window* window;
	IntroScene intro_scene;

public:
	Game();
	void update();
	void draw();
};
