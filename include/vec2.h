#ifndef __VEC2_H__
#define __VEC2_H__

#include <math.h>

struct vec2 {
	float x;
	float y;
};

static void vec2_add(struct vec2 *v1, struct vec2 v2) {
	v1->x += v2.x;
	v1->y += v2.y;
}

static float vec2_angle(struct vec2 v1, struct vec2 v2) {

	if (v1.x == 0 && v1.y == 0 && v2.x == 0 && v2.y == 0)
		return 0.0f;

	double dot = v1.x * v2.x + v1.y * v2.y;
	double v1mag = sqrt(v1.x * v1.x + v1.y * v1.y);
	double v2mag = sqrt(v2.x * v2.x + v2.y * v2.y);
	double amt = dot / (v1mag * v2mag);

	if (amt <= -1) {
		return M_PI;
	} else if (amt >= 1) {
		return 0;
	}
	return (float)acos(amt);
}

static float vec2_dist(struct vec2 v1, struct vec2 v2) {
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	return (float)sqrt(dx * dx + dy * dy);
}

static void vec2_div(struct vec2 *v, float n) {
	v->x /= n;
	v->y /= n;
}

static float vec2_dot(struct vec2 v1, struct vec2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

static struct vec2 vec2_from_angle(float angle) {
	struct vec2 v = {(float)cos(angle), (float)sin(angle)};

	return v;
}

static float vec2_get_mag(struct vec2 v) {
	return (float)sqrt(v.x * v.x + v.y * v.y);
}

static float vec2_get_magsq(struct vec2 v) { return (v.x * v.x + v.y * v.y); }

static float vec2_heading(struct vec2 v) { return (float)atan2(v.x, v.y); }

static void vec2_mul(struct vec2 *v, float n) {
	v->x *= n;
	v->y *= n;
}

static void vec2_normalize(struct vec2 *v) {
	float m = vec2_get_mag(*v);
	if (m != 0 && m != 1) {
		v->x /= m;
		v->y /= m;
	}
}

static void vec2_limit(struct vec2 *v, float max) {
	if (vec2_get_magsq(*v) > max * max) {
		vec2_normalize(v);
		v->x *= max;
		v->y *= max;
	}
}

static void vec2_rotate(struct vec2 *v, float theta) {
	float t = v->x;

	v->x = v->x * cos(theta) - v->y * sin(theta);
	v->y = t * sin(theta) + v->y * cos(theta);
}

static void vec2_set(struct vec2 *v, float x, float y) {
	v->x = x;
	v->y = y;
}

static void vec2_set_mag(struct vec2 *v, float len) {
	vec2_normalize(v);
	v->x *= len;
	v->y *= len;
}

static void vec2_sub(struct vec2 *v1, struct vec2 v2) {
	v1->x -= v2.x;
	v1->y -= v2.y;
}

#endif
