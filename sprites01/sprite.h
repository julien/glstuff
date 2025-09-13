#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <math.h>
#include <stdio.h>

const int MAX_SPRITES = 10000;
const int SPRITE_MESH_SIZE = 30;

struct vec2 {
	float x;
	float y;
};

struct sprite {
	struct sprite *next;
	struct vec2 pos;
	struct vec2 vel;
	struct vec2 size;
	struct vec2 o_size;
	float life;
	int magnet_next;
	int magnet_prev;
	int index;
};

static inline void sprite_init(struct sprite *s, float x, float y, float w,
                               float h, float vx, float vy, float l) {
	s->life = l;
	s->pos.x = x;
	s->pos.y = y;
	s->size.x = w;
	s->size.y = h;
	s->o_size.x = w;
	s->o_size.y = h;
	s->vel.x = vx;
	s->vel.y = vy;
}

static inline int sprite_in_use(struct sprite *s) { return s->life > 0; }

static inline int sprite_update(struct sprite *s, struct sprite *sprites) {
	if (!sprite_in_use(s)) {
		return 1;
	}

	float fact = 0.05f;
	float force = 2.0f;

	struct sprite p1 = sprites[s->magnet_next];
	struct sprite p2 = sprites[s->magnet_prev];

	float dx1 = p1.pos.x - s->pos.x;
	float dx2 = p2.pos.x - s->pos.x;
	float dy1 = p1.pos.y - s->pos.y;
	float dy2 = p2.pos.y - s->pos.y;

	float fx = fact * force * fabsf(dx1) / (dx1)*pow(dx1, 2) /
	           (pow(dy1, 2) + pow(dx1, 2));

	float fy = fact * force * fabsf(dy1) / (dy1)*pow(dy1, 2) /
	           (pow(dy1, 2) + pow(dx1, 2));

	float rx = fact * force * fabsf(dx2) / (dx2)*pow(dx2, 2) /
	           (pow(dy2, 2) + pow(dx2, 2));

	float ry = fact * force * fabsf(dy2) / (dy2)*pow(dy2, 2) /
	           (pow(dy2, 2) + pow(dx2, 2));

	s->vel.x += (4 * fx - rx) * 4 / pow(s->life, 0.01);
	s->vel.y += (4 * fy - ry) * 4 / pow(s->life, 0.01);

	s->pos.x += s->vel.x * 0.1;
	s->pos.y += s->vel.y * 0.09;

	float sz = map(s->life, 0, 100, 0, s->o_size.x);
	s->size.x = s->size.y = sz;

	s->life--;

	return s->life < 0;
}

static inline void sprite_magnet(struct sprite *s, int next, int prev) {
	s->magnet_next = next;
	s->magnet_prev = prev;
}

struct spritepool {
	struct sprite *first_available;
	struct sprite sprites[MAX_SPRITES];
	float vertices[MAX_SPRITES * SPRITE_MESH_SIZE];
};

static inline void spritepool_init(struct spritepool *s) {
	s->first_available = &(s->sprites[0]);

	int magnet_next;
	int magnet_prev;

	for (int i = 0; i < MAX_SPRITES; i++) {

		magnet_next = (i + 1) % MAX_SPRITES;
		magnet_prev = (i + 2) % MAX_SPRITES;

		s->sprites[i].index = i;
		sprite_magnet(&(s->sprites[i]), magnet_next, magnet_prev);
		s->sprites[i].next = &(s->sprites[i + 1]);
	}

	s->sprites[MAX_SPRITES - 1].next = NULL;
}

static inline struct sprite *spritepool_at(struct spritepool *s,
                                           unsigned int idx) {
	if (idx < MAX_SPRITES) {
		return &(s->sprites[idx]);
	}
	return NULL;
}

static inline void spritepool_create(struct spritepool *s, float x, float y,
                                     float w, float h, float vx, float vy,
                                     float life) {
	if (!s->first_available) {
		return;
	}

	struct sprite *new_sprite = s->first_available;
	s->first_available = new_sprite->next;
	sprite_init(new_sprite, x, y, w, h, vx, vy, life);
}

static inline void spritepool_update(struct spritepool *s) {
	for (int i = 0; i < MAX_SPRITES; i++) {
		if (sprite_update(&(s->sprites[i]), s->sprites)) {
			s->sprites[i].next = s->first_available;
			s->first_available = &(s->sprites[i]);
		}
	}
}

static inline int spritepool_draw(struct spritepool *s) {
	int quads = 0;
	int quads30 = 0;

	for (int i = 0; i < MAX_SPRITES; i++) {
		if (sprite_in_use(&(s->sprites[i]))) {

			int quads30i = quads30;

			struct vec2 pos = s->sprites[i].pos;
			struct vec2 size = s->sprites[i].size;

			float x = pos.x;
			float y = pos.y;
			float xx = x + size.x;
			float yy = y + size.y;

			s->vertices[quads30i++] = x;
			s->vertices[quads30i++] = y;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 1;

			s->vertices[quads30i++] = xx;
			s->vertices[quads30i++] = y;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 1;
			s->vertices[quads30i++] = 1;

			s->vertices[quads30i++] = x;
			s->vertices[quads30i++] = yy;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 0;

			s->vertices[quads30i++] = x;
			s->vertices[quads30i++] = yy;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 0;

			s->vertices[quads30i++] = xx;
			s->vertices[quads30i++] = y;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 1;
			s->vertices[quads30i++] = 1;

			s->vertices[quads30i++] = xx;
			s->vertices[quads30i++] = yy;
			s->vertices[quads30i++] = 0;
			s->vertices[quads30i++] = 1;
			s->vertices[quads30i++] = 0;

			quads30 += 30;
			quads++;
		}
	}
	return quads * 6;
}

#endif
