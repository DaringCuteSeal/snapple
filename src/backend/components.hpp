#pragma once

#include "../prelude.hpp"
#include <memory>
#include <queue>
#include <string>
#include <vector>

using std::queue, std::string, std::vector, std::unique_ptr;

namespace GameComponents {

/** Components: komponen-komponen utama dari game.
 * Catatan: BUKAN game-nya sendiri.
*/

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
	SpriteManager sprite_manager;

	/** ID dari scene.
	 */
	string id;

	virtual void draw() = 0;
	virtual void update() = 0;

	// dipanggil untuk meng-update dan draw sprites jadi tidak harus
	// diimplementasikan ulang oleh kelas-kelas yang inherit kelas Scene ini.
	void draw_all();

	// dipanggil untuk meng-update dan draw sprites jadi tidak harus
	// diimplementasikan ulang oleh kelas-kelas yang inherit kelas Scene ini.
	void update_all();
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

/** Abstract base class untuk EventManager. Kita bisa buat implementasi khusus.
 */
class Event {
public:
	virtual void exec() = 0;
};

/** EventManager: mengurusi event dispatching. Di game, kita bisa buat beberapa
 * Event yang memiliki implementasi sendiri.
 *
 * Eksekusi event itu blocking, sehingga kita tidak boleh menambahkan callback
 * (di sini implementasi dari Event.exec()) yang blocking pula. Kita harus
 * menggunakan pola state machine.
*/
class EventManager {
private:
	queue<unique_ptr<Event>> events;

public:
	EventManager();

	/** Call untuk tiap update. Method ini akan menjalankan event yang telah di
	* antrikan sesuai urutan.
	*/
	void update();

	/** Kirim sebuah event. Method ini menerima event apapun asalkan merupakan
	 * turunan dari kelas `Event`.
	*/
	void dispatch(unique_ptr<Event> event);
};

/** Data type untuk Timer.
 */
struct TimerItem {
public:
	double expire;
	void (*exec)();
};

/** Timer: jalankan fungsi tertentu (callback) setelah waktu habis
 */
class Timer {
private:
	double(*time_function)();

public:
	vector<TimerItem> timers;
	Timer(double(*time_function)() = GetTime);

	/** Tambahkan item timer: jalankan callback yang diberikan setelah `duration` detik.
	 */
	void attach(double duration, void (*exec)());

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

}
