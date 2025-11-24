#include "prelude.hpp"
#include "cfg.hpp"
#include "game/game.hpp"
#include <raylib.h>

/*! \mainpage Snapple
 *
 * \section intro Intro
 * Dokumentasi ini menyediakan ikhtisar dari Snapple, sebuah permainan ular
 * untuk melatih ketangkasan aritmatika. Dokumentasi ini mendaftarkan
 * arsitektur, komponen-komponen utama, dan penjelasan dari fungsi-fungsi yang
 * ada.
 *
 * Catatan: fungsi-fungsi tanpa penjelasan biasanya merupakan fungsi sederhana
 * dan memiliki tujuan yang sudah terlihat dari namanya.
 *
 *
 * Laman proyek: https://github.com/DaringCuteSeal/snapple
 */

/** Fungsi utama dari aplikasi. Fungsi ini menginisalisasi jendela dan
 * menjalankan game hingga jendela ditutup.
 */
int main() {
	raylib::InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	InitAudioDevice();
	SetMasterVolume(1.0);

	// Setel FPS (frame per second)
	SetTargetFPS(FPS);

	// Beri seed untuk RNG
	SetRandomSeed(time(NULL));

	Game game;

	while (!raylib::Window::ShouldClose()){
		game.update();
		game.draw();
	}

	return 0;
}
