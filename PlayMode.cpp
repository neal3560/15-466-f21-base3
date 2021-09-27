#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>


// ---------------- Load Scene ----------------------
GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("hexapod.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("hexapod.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

GLuint brunch_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > brunch_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("brunch.pnct"));
	brunch_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > brunch_scene(LoadTagDefault, []() -> Scene const* {
	return new Scene(data_path("brunch.scene"), [&](Scene& scene, Scene::Transform* transform, std::string const& mesh_name) {
		Mesh const& mesh = brunch_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = brunch_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

// ------------------- Load Sound --------------------------
Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

Load< Sound::Sample > bgm_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Foam Rubber.opus"));
});

Load< Sound::Sample > cat_meow_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Cat-Meow.opus"));
});

Load< Sound::Sample > cat_attack_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Cat-Attack.opus"));
});

Load< Sound::Sample > eat_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Eat.opus"));
});

Load< Sound::Sample > victory_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("victory.opus"));
});

Load< Sound::Sample > rat_death_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Rat-Death.opus"));
});


PlayMode::PlayMode() : scene(*brunch_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Cat") cat = &transform;
		else if (transform.name == "Rat") rat = &transform;
		else if (transform.name == "cake") {
			food[0].position = transform.position;
			food[0].size = 5.0f;
			food[0].target.push_back(&transform);
		} 
		else if (transform.name == "pancakes") {
			food[1].position = transform.position;
			food[1].size = 10.0f;
			food[1].target.push_back(&transform);
		}
		else if (transform.name == "sandwich") {
			food[2].position = transform.position;
			food[2].size = 10.0f;
			food[2].target.push_back(&transform);
		}
		else if (transform.name == "bacon") {
			food[3].position = transform.position;
			food[3].size = 10.0f;
			food[3].target.push_back(&transform);
		}
		else if (transform.name == "egg") {
			food[3].target.push_back(&transform);
		}
		else if (transform.name == "bacon.001") {
			food[3].target.push_back(&transform);
		}
		else if (transform.name == "egg.001") {
			food[4].position = transform.position;
			food[4].size = 10.0f;
			food[4].target.push_back(&transform);
		}
		else if (transform.name == "egg.002") {
			food[4].target.push_back(&transform);
		}
		else if (transform.name == "bacon.002") {
			food[4].target.push_back(&transform);
		}
	}
	

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	bgm_loop = Sound::loop(*bgm_sample, 1.0f, 0.0f);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size, SDL_Window *window) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
		if (evt.button.button == SDL_BUTTON_LEFT) {
			tryEating = true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			rat->rotation = glm::normalize(
				rat->rotation * glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
			);
			SDL_WarpMouseInWindow(window, window_size.x / 2, window_size.y / 2);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (isCatched) {
		DeathTimer += elapsed;
		cat->position = CatPosDeath + (rat->position - CatPosDeath) * std::min(DeathTimer, 1.0f);
		status = "You Lose";
		return;
	}

	if (numFood == 0) {
		status = "You Win";
		return;
	}

	//move character
	{	
		if (target == nullptr) {
			//combine inputs into a move:
			constexpr float PlayerSpeed = 30.0f;
			glm::vec2 move = glm::vec2(0.0f);
			if (left.pressed && !right.pressed) move.x = -1.0f;
			if (!left.pressed && right.pressed) move.x = 1.0f;
			if (down.pressed && !up.pressed) move.y = -1.0f;
			if (!down.pressed && up.pressed) move.y = 1.0f;

			//make it so that moving diagonally doesn't go faster:
			if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

			glm::mat4x3 frame = rat->make_local_to_parent();
			glm::vec3 right = -frame[2];
			glm::vec3 forward = -frame[0];

			rat->position += move.x * right + move.y * forward;

			rat->position.x = glm::clamp(rat->position.x, -table_size.x + rat_size, table_size.x - rat_size);
			rat->position.y = glm::clamp(rat->position.y, -table_size.y + rat_size, table_size.y - rat_size);

			glm::vec2 rat_pos = glm::vec2(rat->position.x, rat->position.y);
			if (length(rat_pos) < rat_size + block_size) {
				rat_pos = glm::normalize(rat_pos) * (rat_size + block_size);
				rat->position.x = rat_pos.x;
				rat->position.y = rat_pos.y;
			}
		}	
	}

	//update listener to rat position:
	{ 
		glm::mat4x3 frame = rat->make_local_to_parent();
		glm::vec3 right = -frame[2];
		Sound::listener.set_position_right(rat->position, right, 1.0f / 60.0f);
	}

	//eating
	{
		//Eating Speed
		constexpr float EatingSpeed = 20.0f;

		if (target) {
			target->life -= EatingSpeed * elapsed;
			//finish eating
			if (target->life <= 0.0f) {
				for (auto it = target->target.begin(); it != target->target.end(); ++it) {
					(*it)->position.z += 100.0f;
				}
				numFood--;
				if (numFood == 0) {
					Sound::play(*victory_sample, 1.0f, 0.0f);
				}
				eat_loop->stop();
				target = nullptr;
			}

			//stop eating
			if (tryEating) {
				eat_loop->stop();
				target = nullptr;
			}
		}
		else {
			//start eating
			if (tryEating) {
				for (int i = 0; i < 5; i++) {
					if (glm::length(rat->position - food[i].position) <= food[i].size && food[i].life > 0.0f) {
						target = &food[i];
						eat_loop = Sound::loop(*eat_sample, 1.0f, 0.0f);
						break;
					}
				}
			}
		}

		tryEating = false;
	}

	//Cat behavior
	{
		checkTimer -= elapsed;

		// status change
		if (checkTimer < 0.0f) {
			if (checkStatus == 0) {
				Sound::play_3D(*cat_meow_sample, 1.0f, cat->position, 20.0f);
				checkTimer = 3.0f;
			}
			else if (checkStatus == 1) {
				checkTimer = 5.0f;
			}
			else if (checkStatus == 2) {
				checkTimer = 3.0f;
			}
			else if (checkStatus == 3) {
				checkTimer = (float)(std::rand() % 5 + 3);
			}
			checkStatus = (checkStatus + 1) % 4;
		}

		constexpr float angleSpeed = 30.0f;

		//circulating
		if (checkStatus == 0) {
			angle += angleSpeed * elapsed;
			if (angle > 360.0f) angle -= 360.0f;
			cat->position = 60.0f * glm::vec3(glm::sin(glm::radians(angle)), glm::cos(glm::radians(angle)), 0.0f);
			cat->position.x = glm::clamp(cat->position.x, -40.0f, 40.0f);
			cat->position.y = glm::clamp(cat->position.y, -30.0f, 30.0f);
			cat->rotation = glm::angleAxis(-glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (checkStatus == 1) {
			cat->position.z = 36.0f - 12.0f * checkTimer;
		}
		else if (checkStatus == 2) {
			glm::vec2 cat_coord = glm::vec2(cat->position.x, cat->position.y);
			glm::vec2 rat_coord = glm::vec2(rat->position.x, rat->position.y);
			glm::vec2 direction = cat_coord - rat_coord;
			float distance = glm::dot(direction, normalize(cat_coord));
			if (
				distance < glm::length(cat_coord) ||
				length(direction - distance * normalize(cat_coord)) > 4.0f
			) {
				Sound::play(*cat_attack_sample, 1.0f, 0.0f);
				Sound::play(*rat_death_sample, 1.0f, 0.0f);
				CatPosDeath = cat->position;
				isCatched = true;
			}
		}
		else if (checkStatus == 3) {
			cat->position.z = 12.0f * checkTimer;
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		

		float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; Left-Mouse Click to start/end eat",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; Left-Mouse Click to start/end eat",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		H = 0.4f;
		if (isCatched || numFood == 0) {
	
			lines.draw_text(status,
				glm::vec3(-aspect * 0.4f + 0.1f * H, 0.1f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			
			lines.draw_text(status,
				glm::vec3(-aspect * 0.4f + 0.1f * H + ofs, 0.1f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		}
	}
	GL_ERRORS();
}

//glm::vec3 PlayMode::get_leg_tip_position() {
	//the vertex position here was read from the model in blender:
//	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
//}
