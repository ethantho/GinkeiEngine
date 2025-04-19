#pragma once

#include "SDL_image.h"
#include "Helper.h"
#include <vector>
#include <queue>
#include <string>

class Actor;

class Particle {
public:
	std::string texture;
	float initial_scale;
	Particle(std::string texture, float scale) {
		this->texture = texture;
		this->initial_scale = scale;
	}

};

struct MovementData {
	float x;
	float y;
	float x_vel;
	float y_vel;
};

struct RotationData {
	float rot;
	float rot_vel;
};

class ParticleSystem
{
public:
	std::string type_name = "ParticleSystem";
	std::string key = "???";
	Actor* actor = nullptr;
	bool enabled = true;

	//////particle data//////
	std::vector<Particle> particles;
	std::vector<bool> is_active;
	std::vector<int> start_frame;
	std::queue<int> free_list;
	std::vector<MovementData> particle_movement_data;
	std::vector<RotationData> particle_rotation_data;
	//////particle data//////

	float x = 0.0f;
	float y = 0.0f;
	int frames_between_bursts = 1;
	int burst_quantity = 1;
	float start_scale_min = 1.0f;
	float start_scale_max = 1.0f;
	float rotation_min = 0.0f;
	float rotation_max = 0.0f;
	uint8_t start_color_r = 255;
	uint8_t start_color_g = 255;
	uint8_t start_color_b = 255;
	uint8_t start_color_a = 255;
	std::string image = "";
	int sorting_order = 9999;
	int duration_frames = 300;
	float start_speed_min = 0.0f;
	float start_speed_max = 0.0f;
	float rotation_speed_min = 0.0f;
	float rotation_speed_max = 0.0f;
	float gravity_scale_x = 0.0f;
	float gravity_scale_y = 0.0f;
	float drag_factor = 1.0f;
	float angular_drag_factor = 1.0f;
	float end_scale = -99999999999.0f; //NO EFFECT UNLESS CONFIGURED!

	int end_color_r = 255;
	int end_color_g = 255;
	int end_color_b = 255;
	int end_color_a = 255;

	float emit_angle_min = 0.0f;
	float emit_angle_max = 360.0f;
	float emit_radius_min = 0.0f;
	float emit_radius_max = 0.5f;

	RandomEngine emit_angle_distribution;
	RandomEngine emit_radius_distribution;
	RandomEngine rotation_distribution;
	RandomEngine scale_distribution;
	RandomEngine speed_distribution;
	RandomEngine rotation_speed_distribution;

	void OnStart();
	void OnUpdate();
	void Stop();
	void Play();
	void Burst();
	

private:
	int local_frame_number = 0;
	void EmitParticle();
	void VerifySizes();
	bool playing = true;
};

