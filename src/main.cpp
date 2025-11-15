#include "prelude.hpp"
#include "cfg.hpp"
#include "game/game.hpp"

/** Fungsi utama dari aplikasi. Fungsi ini menginisalisasi jendela dan
 * menjalankan game hingga jendela ditutup.
 */
int main() {
	raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

	SetTargetFPS(60);

	Game game(&window);

	while (!window.ShouldClose()){
		game.update();
		game.draw();
	}

	return 0;
}
