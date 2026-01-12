#include "AnimationManager.hpp"

AnimationManager::AnimationManager() {}

AnimationManager::~AnimationManager() {}

void AnimationManager::addAnimation(const std::string &name, const std::vector<std::string> &sprites, size_t delay) {
	auto animation = animations.find(name);
	if (animation != animations.end())
		return;

	animations.emplace(name, Animation{ sprites, delay, 0, std::chrono::steady_clock::now() });
}

void AnimationManager::onFrame() {	
	for (auto& a : animations) {
		animate(a.second);
	}
}

void AnimationManager::animate(Animation &anim) {
  	auto currentTime = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - anim.lastDrawTime).count();

	if ((size_t)elapsed > anim.delay) {
		++anim.currentSprite;
		if (anim.currentSprite >= anim.sprites.size())
			anim.currentSprite = 0;
    
		anim.lastDrawTime = currentTime;
  	}
}

std::optional<std::string> AnimationManager::getAnimationSprite(const std::string &name) {
	auto it = animations.find(name);
	if (it == animations.end())
		return std::nullopt;

	auto anim = it->second;
	return anim.sprites[anim.currentSprite];
}