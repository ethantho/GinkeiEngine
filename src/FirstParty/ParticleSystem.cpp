#include "glm/glm.hpp"
#include "ImageAPI.h"
#include "ImageDB.h"
#include "ParticleSystem.h"
#include <unordered_set>

void ParticleSystem::OnStart()
{
	emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
	emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);

	rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
	scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);

	speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
	rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);
}

void ParticleSystem::OnUpdate()
{
	//VerifySizes(); //LEAVE COMMENTED FOR SUBMIT PERFORMANCE!

	//emission
	if (local_frame_number % (frames_between_bursts < 1 ? 1 : frames_between_bursts) == 0 && playing) {
		for (int i = 0; i < (burst_quantity < 1 ? 1 : burst_quantity); ++i) EmitParticle();
	}

	//active state
	for (int i = 0; i < particles.size(); ++i) {
		if (!is_active[i]) continue;
		if (local_frame_number - start_frame[i] >= (duration_frames > 0 ? duration_frames : 1)) {
			is_active[i] = false;
			free_list.push(i);
			//num_particles-- ?
			continue;
		}
	}

	//movement
	for (int i = 0; i < particle_movement_data.size(); ++i) {
		particle_movement_data[i].x_vel += gravity_scale_x;
		particle_movement_data[i].y_vel += gravity_scale_y;

		particle_movement_data[i].x_vel *= drag_factor;
		particle_movement_data[i].y_vel *= drag_factor;

		particle_movement_data[i].x += particle_movement_data[i].x_vel;
		particle_movement_data[i].y += particle_movement_data[i].y_vel;
	}

	//rotation
	for (int i = 0; i < particle_rotation_data.size(); ++i) {
		particle_rotation_data[i].rot_vel *= angular_drag_factor;
		particle_rotation_data[i].rot += particle_rotation_data[i].rot_vel;
		
	}

	//drawing
	for (size_t i = 0; i < particles.size(); ++i) {
		if (!is_active[i]) continue;
		float lifetime_progress = static_cast<float>(local_frame_number - start_frame[i]) / duration_frames;
		float current_scale = particles[i].initial_scale;
		if (end_scale != -99999999999.0f) {
			current_scale = glm::mix(particles[i].initial_scale, end_scale, lifetime_progress);
		}
		uint8_t current_r = glm::mix(start_color_r, static_cast<uint8_t>(end_color_r), lifetime_progress);
		uint8_t current_g = glm::mix(start_color_g, static_cast<uint8_t>(end_color_g), lifetime_progress);
		uint8_t current_b = glm::mix(start_color_b, static_cast<uint8_t>(end_color_b), lifetime_progress);
		uint8_t current_a = glm::mix(start_color_a, static_cast<uint8_t>(end_color_a), lifetime_progress);
		ImageAPI::DrawEx(particles[i].texture, particle_movement_data[i].x, particle_movement_data[i].y, particle_rotation_data[i].rot, current_scale, current_scale, 0.5f, 0.5f, current_r, current_g, current_b, current_a, sorting_order);
	}
	local_frame_number++;
}

void ParticleSystem::Stop()
{
	playing = false;
}

void ParticleSystem::Play()
{
	playing = true;
}

void ParticleSystem::Burst()
{
	for (int i = 0; i < (burst_quantity < 1 ? 1 : burst_quantity); ++i) EmitParticle();
}

void ParticleSystem::EmitParticle()
{
	float angle_radians = glm::radians(emit_angle_distribution.Sample());
	float radius = emit_radius_distribution.Sample();

	float cos_angle = glm::cos(angle_radians);
	float sin_angle = glm::sin(angle_radians);

	float starting_x_pos = x + cos_angle * radius;
	float starting_y_pos = y + sin_angle * radius;

	float starting_scale = scale_distribution.Sample();

	float starting_rotation = rotation_distribution.Sample();

	float speed = speed_distribution.Sample();
	float rot_speed = rotation_speed_distribution.Sample();

	float starting_x_vel = cos_angle * speed;
	float starting_y_vel = sin_angle * speed;


	std::string starting_texture = image == "" ? "__default_particle" : image;
	ImageDB::CreateDefaultParticleTextureWithName("__default_particle");

	if (free_list.empty()) {
		particles.emplace_back(Particle(starting_texture, starting_scale));
		particle_movement_data.push_back({ starting_x_pos, starting_y_pos, starting_x_vel, starting_y_vel });
		particle_rotation_data.push_back({ starting_rotation, rot_speed });
		is_active.push_back(true);
		start_frame.push_back(local_frame_number);
	}
	else {
		int index = free_list.front();
		free_list.pop();
		particles[index] = Particle(starting_texture, starting_scale);
		particle_movement_data[index] = { starting_x_pos, starting_y_pos, starting_x_vel, starting_y_vel };
		particle_rotation_data[index] = { starting_rotation, rot_speed };
		is_active[index] = true;
		start_frame[index] = local_frame_number;
	}
	
}

void ParticleSystem::VerifySizes()
{
	std::unordered_set<int> sizes;
	sizes.insert(particles.size());
	sizes.insert(is_active.size());
	sizes.insert(start_frame.size());
	sizes.insert(particle_movement_data.size());
	sizes.insert(particle_rotation_data.size());
	if (sizes.size() != 1) {
		assert(false);
	}
}
