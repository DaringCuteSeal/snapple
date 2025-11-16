#include "prelude.hpp"
#include "cfg.hpp"
#include "game/game.hpp"

/** Fungsi utama dari aplikasi. Fungsi ini menginisalisasi jendela dan
 * menjalankan game hingga jendela ditutup.
 */
int main() {
	raylib::InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

	SetTargetFPS(FPS);

	Game game;

	while (!raylib::Window::ShouldClose()){
		game.update();
		game.draw();
	}

	return 0;
}
