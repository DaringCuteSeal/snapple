#include "../backend/components.hpp"

class Game {
private:
	GameComponents::GameStateManager game_state_manager;

public:
	void update();
	void draw();
};
