#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <stdio.h>

const int MAX_PARTICLES = 100000;
const int PARTICLE_MESH_SIZE = 30;

struct vec2 {
	float x;
	float y;
};

struct particle {
	struct particle *next;
	struct vec2 pos;
	struct vec2 vel;
	struct vec2 acc;
	struct vec2 size;
	float life;
};

static inline void particle_init(struct particle *p, float x, float y, float w,
                                 float h, float vx, float vy, float life) {
	p->pos.x = x;
	p->pos.y = y;
	p->vel.x = vx;
	p->vel.y = vy;
	p->acc.x = 0;
	p->acc.y = 0;
	p->size.x = w;
	p->size.y = h;
	p->life = life;
}

static inline int particle_in_use(struct particle *p) { return p->life > 0; }

static inline int particle_update(struct particle *p) {
	if (!particle_in_use(p))
		return 0;

	float delta = 0.05;
	float grav = 2.0;

	p->acc.y += grav;

	p->acc.x *= delta;
	p->acc.y *= delta;

	p->vel.x += p->acc.x;
	p->vel.y += p->acc.y;

	p->pos.x += p->vel.x;
	p->pos.y += p->vel.y;

	p->acc.x = 0.0;
	p->acc.y = 0.0;

	p->life--;

	return p->life < 0;
}

typedef struct pool {
	struct particle *first_available;
	struct particle particles[MAX_PARTICLES];
	float vertices[MAX_PARTICLES * PARTICLE_MESH_SIZE];
} pool;

static inline void pool_init(struct pool *p) {
	p->first_available = &p->particles[0];
	for (int i = 0; i < MAX_PARTICLES - 1; i++) {
		p->particles[i].next = &p->particles[i + 1];
	}
	p->particles[MAX_PARTICLES - 1].next = NULL;
}

static inline void pool_create(struct pool *p, float x, float y, float w,
                               float h, float vx, float vy, float life) {
	if (!p->first_available) {
		return;
	}

	struct particle *new_particle = p->first_available;

	p->first_available = new_particle->next;
	particle_init(new_particle, x, y, w, h, vx, vy, life);
}

static inline void pool_update(struct pool *p) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particle_update(&p->particles[i])) {
			p->particles[i].next = p->first_available;
			p->first_available = &p->particles[i];
		}
	}
}

static inline unsigned int pool_draw(struct pool *p) {
	unsigned int quads = 0, quads30 = 0;

	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particle_in_use(&p->particles[i])) {
			unsigned int quads30i = quads30;

			struct vec2 pos = p->particles[i].pos;
			struct vec2 size = p->particles[i].size;

			float x = pos.x;
			float y = pos.y;
			float xx = x + size.x;
			float yy = y + size.y;

			p->vertices[quads30i++] = x;
			p->vertices[quads30i++] = y;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 1;

			p->vertices[quads30i++] = xx;
			p->vertices[quads30i++] = y;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 1;
			p->vertices[quads30i++] = 1;

			p->vertices[quads30i++] = x;
			p->vertices[quads30i++] = yy;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 0;

			p->vertices[quads30i++] = x;
			p->vertices[quads30i++] = yy;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 0;

			p->vertices[quads30i++] = xx;
			p->vertices[quads30i++] = y;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 1;
			p->vertices[quads30i++] = 1;

			p->vertices[quads30i++] = xx;
			p->vertices[quads30i++] = yy;
			p->vertices[quads30i++] = 0;
			p->vertices[quads30i++] = 1;
			p->vertices[quads30i++] = 0;

			quads30 += 30;
			quads++;
		}
	}
	return quads * 6;
}

#endif
