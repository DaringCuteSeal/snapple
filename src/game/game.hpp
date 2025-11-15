#include "../backend/components.hpp"
#include "../prelude.hpp"

class Game {
private:
	GameComponents::GameStateManager game_state_manager;
	raylib::Window* window;

public:
	Game(raylib::Window* window);
	void update();
	void draw();
};
