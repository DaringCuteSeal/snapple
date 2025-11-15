#include <iostream>
#include <memory>
#include <raylib.h>
#include "components.hpp"

using std::swap;
using namespace GameComponents;

SceneManager::SceneManager(Scene* initial_scene) {
	this->current_scene = initial_scene;
}

void SceneManager::draw() {
	this->current_scene->draw();
}

void SceneManager::update() {
	this->current_scene->update();
}

void SceneManager::set_scene(Scene* initial_scene) {
	this->current_scene = initial_scene;
}

string SceneManager::current_id() {
	return this->current_scene->id;
}

EventManager::EventManager() {

}

void EventManager::update() {
	while (!this->events.empty()){
		this->events.front()->exec();
		this->events.pop();
	}
}

void EventManager::dispatch(unique_ptr<Event> event){
	this->events.push(std::move(event));
}

SpriteManager::SpriteManager() {

}

void SpriteManager::draw() {
	for (auto& s : this->sprites){
		s->draw();
	}
}

void SpriteManager::update() {
	for (auto& s : this->sprites){
		s->update();
	}
}

void SpriteManager::delete_all() {
	this->sprites.clear();
}

void SpriteManager::append(unique_ptr<Sprite> sprite) {
	this->sprites.push_back(std::move(sprite));
}

Timer::Timer(double(*time_function)()){
	this->time_function = time_function;
}

void Timer::attach(double duration, void (*exec)()){
	this->timers.push_back(TimerItem {.expire = this->time_function() + duration, .exec = exec});
}

void Timer::update() {
	int n = this->timers.size();
	int partition = 0;
	// Mirip teknik partisi lomuto: untuk timer yang udah expired ditaro di
	// ujung. habis itu, vektor-nya di trim biar cuman yang masih belum expired
	// yang disimpan.
	for (size_t i = 0; i < n; i++){
		if (this->time_function() >= this->timers[i].expire){
			this->timers[i].exec();
		} else {
			swap(this->timers[i], this->timers[partition]);
			partition += 1;
		}
	}
	timers.resize(partition);
}

GameStateManager::GameStateManager(Scene* init_scene) : scene_manager(init_scene) {
}

void GameStateManager::update() {
	this->event_manager.update();
	this->timer.update();
	this->scene_manager.update();
	this->sprite_manager.update();
}

void GameStateManager::draw() {
	this->scene_manager.draw();
	this->sprite_manager.draw();
}
