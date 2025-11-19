#include "../prelude.hpp"
#include "../backend/components.hpp"

class InterludeScene : public GameComponents::Scene {
private:
	const char* id = "interlude";
	const uint fps = 13;
	static const size_t n_frames = 38;
	size_t current_frame;
	double last_time;
	double time_per_frame;
	const char* frames[n_frames] = {
		"assets/interlude_animation/0.png",
		"assets/interlude_animation/1.png",
		"assets/interlude_animation/2.png",
		"assets/interlude_animation/3.png",
		"assets/interlude_animation/4.png",
		"assets/interlude_animation/5.png",
		"assets/interlude_animation/6.png",
		"assets/interlude_animation/7.png",
		"assets/interlude_animation/8.png",
		"assets/interlude_animation/9.png",
		"assets/interlude_animation/10.png",
		"assets/interlude_animation/11.png",
		"assets/interlude_animation/12.png",
		"assets/interlude_animation/13.png",
		"assets/interlude_animation/14.png",
		"assets/interlude_animation/15.png",
		"assets/interlude_animation/16.png",
		"assets/interlude_animation/17.png",
		"assets/interlude_animation/18.png",
		"assets/interlude_animation/19.png",
		"assets/interlude_animation/20.png",
		"assets/interlude_animation/21.png",
		"assets/interlude_animation/22.png",
		"assets/interlude_animation/23.png",
		"assets/interlude_animation/24.png",
		"assets/interlude_animation/25.png",
		"assets/interlude_animation/26.png",
		"assets/interlude_animation/27.png",
		"assets/interlude_animation/28.png",
		"assets/interlude_animation/29.png",
		"assets/interlude_animation/30.png",
		"assets/interlude_animation/31.png",
		"assets/interlude_animation/32.png",
		"assets/interlude_animation/33.png",
		"assets/interlude_animation/34.png",
		"assets/interlude_animation/35.png",
		"assets/interlude_animation/36.png",
		"assets/interlude_animation/37.png",
	};

	raylib::Texture2D interlude_animation[n_frames];
	const char* background_filename = "assets/background.png";
	raylib::Texture2D background;
	GameComponents::GameStateManager* game_state_manager;
	function<void()> start_game_callback;
	bool is_ready_to_start_game; // Must be set to false to restart TODO: write the restart method

public:
	InterludeScene();
	void init(GameComponents::GameStateManager* game_state_manager, function<void()> start_game_callback);
	void draw();
	void update();
};
