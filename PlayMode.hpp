#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cmath>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size, SDL_Window *window) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//transform
	Scene::Transform *cat;
	Scene::Transform *rat;
	
	struct Food{
		glm::vec3 position;
		float size;
		float life = 100.0;
		std::vector<Scene::Transform*> target;
	};

	Food food[5];

	//radius
	glm::vec2 table_size = glm::vec2(35.0f, 20.0f);
	float rat_size = 3.0f;
	float block_size = 5.0f;

	//Game status
	int numFood = 5;
	Food* target = nullptr;
	bool tryEating = false;
	bool isCatched = false;

	//Cat status
	int checkStatus = 0;
	float checkTimer = 5.0f;
	float angle = 0;
	float DeathTimer = 0;
	glm::vec3 CatPosDeath;


	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > bgm_loop;
	std::shared_ptr< Sound::PlayingSample > eat_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

	//textUI
	std::string status;
};
