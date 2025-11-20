#pragma once

#include "../prelude.hpp"
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

using std::queue, std::string, std::vector, std::unique_ptr, std::function;

namespace GameComponents {

/** Components: komponen-komponen utama dari game.
 * Catatan: BUKAN game-nya sendiri.
*/

// TODO: mungkin ga kepake
/** Abstract base class untuk SpriteManager.
 */
class Sprite {
public:
	virtual void draw() = 0;
	virtual void update() = 0;
};

/** SpritesManager: mengurusi semua sprite yang ada.
*/
class SpriteManager {
private:
	vector<unique_ptr<Sprite>> sprites;
public:
	SpriteManager();
	void draw();
	void update();
	void delete_all();
	void append(unique_ptr<Sprite> sprite);
};

/** Abstract class scene untuk SceneManager. Kita bisa buat implementasi khusus.
 */
class Scene {
private:
public:

	/** ID dari scene.
	 */
	const char* id;

	virtual void draw() = 0;
	virtual void update() = 0;

};

/** SceneManager: mengurusi scene yang ditampilkan di layar. Pada game, kita
 * bisa buat beberapa scene yang memiliki implementasi draw() dan update() yang
 * berbeda-beda dan menambahkannya ke sini.
 *
 * SceneManager tidak akan menyimpan scene, tetapi hanya mengambil pointer ke
 * scene yang ada. Ini untuk memudahkan kepemilikan.
*/
class SceneManager {
private:
	Scene* current_scene;

public:

	void draw();
	void update();

	/** Konstruktor. Berikan argumen `initial_scene` untuk menjalankan scene
	 * awal. Gak ada null pointer protection jadi jangan sembarangan. Oh ya,
	 * scene nya juga gaboleh dihapus sebelum `SceneManager` ini dihapus.
	 */
	SceneManager(Scene* initial_scene);

	/** Setel scene ke scene yang ditunjuk `initial_scene`. Ini akan mengubah
	 * fungsi `draw()` dan `update()` yang dipanggil. Gak ada null pointer
	 * protection jadi jangan sembarangan. Jangan hapus scene sebelum
	 * `SceneManager` ini dihapus.
	 */
	void set_scene(Scene* initial_scene);

	/** ID scene yang sekarang.
	 */
	string current_id();
};

/** Sebuah event.
 */
struct Event {
	function<void()> callback;
};

/** EventManager: mengurusi event dispatching.
 *
 * Eksekusi event itu blocking, sehingga kita tidak boleh menambahkan callback
 * yang blocking pula.
 */
class EventManager {
private:
	queue<Event> events;

public:
	EventManager();

	/** Call untuk tiap update. Method ini akan menjalankan event yang telah di
	* antrikan sesuai urutan.
	*/
	void update();

	/** Kirim sebuah event untuk dijalankan callbacknya.
	*/
	void dispatch(Event event);
};

/** Data type untuk Timer.
 */
struct TimerItem {
public:
	double expire;
	function<void()> callback;
};

/** Timer: jalankan fungsi tertentu (callback) setelah waktu habis.
 */
class Timer {
private:
	double(*time_function)();

public:
	vector<TimerItem> timers;
	Timer(double(*time_function)() = GetTime);

	/** Tambahkan item timer: jalankan callback yang diberikan setelah `duration` detik.
	 */
	void attach(double duration, function<void()> callback);

	/** Jalankan perintah setelah waktu sudah expired.
	 */
	void update();
};

/** Koleksi semua komponen agar bisa diakses game kita dengan mudah.
*/
class GameStateManager {
public:
	GameStateManager(Scene* init_scene);
	EventManager event_manager;
	Timer timer;
	SceneManager scene_manager;

	void update();
	void draw();
};


/** Representing a coordinate.
*/
struct Coordinate {
  int row;
  int col;

  Coordinate operator +(const Coordinate& other) const;
  Coordinate add_row(int r);
  Coordinate add_col(int c);
};

}
