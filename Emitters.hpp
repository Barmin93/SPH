#pragma once
#include <vector>
#include <glm/glm.hpp>

class ParticleSystem;

/**
 * Spits particles around in random direction in specified radius
 * position	position of emitter in local (x, y, z) coordinates
 * ttl	time to live; decreases; if equals 0 then Emitter is removed
 * delay	time interval for eitting another particle
 * emit_radius	radius in which particles appear; r = h
 */
struct Emitter
{
	Emitter(glm::vec3 pos, glm::vec3 vel) : position(pos), emit_velocity(vel), ttl(6.0f), last_emission_time(0.0f), delay(c::dt), emit_radius(0.5f * c::H) { }
	Emitter(glm::vec3 pos) : Emitter(pos, glm::vec3(0.0f)) { }
	glm::vec3 position;
	glm::vec3 emit_velocity;
	float ttl;
	float last_emission_time;
	float delay;
	float emit_radius;
};

class Emitters
{

public:
	Emitters();

	void emit();
	bool is_any_emitter_alive() const;
	void add_emitter(Emitter const & em)
	{
		_emitters.push_back(em);
	}

	void set_particle_system(ParticleSystem & ps) { particle_system_ref = &ps; };

private:
	std::vector<Emitter> _emitters;

	ParticleSystem * particle_system_ref;
};