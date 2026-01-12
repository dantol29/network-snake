#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <optional>
#include <iostream>

struct Animation {
	std::vector<std::string> sprites;
	size_t delay;
	size_t currentSprite;
	std::chrono::steady_clock::time_point lastDrawTime;
};

class AnimationManager {
public:
	AnimationManager();
	AnimationManager(const AnimationManager& m) = delete;
	AnimationManager& operator=(const AnimationManager& m) = delete;
	~AnimationManager();


	void onFrame();
	std::optional<std::string> getAnimationSprite(const std::string &name);
	void addAnimation(const std::string &name, const std::vector<std::string> &sprites, size_t delay);
private:
	void animate(Animation &anim);
	std::unordered_map<std::string, Animation> animations;
};