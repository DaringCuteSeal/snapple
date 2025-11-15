#include "../prelude.hpp"
#include <memory>
#include "components.hpp"
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

GameStateManager::GameStateManager(Scene* init_scene) : scene_manager(init_scene) {
}
