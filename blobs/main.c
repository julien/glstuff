#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1024
#define HEIGHT 1024

struct blob {
	float cx;
	float cy;
	float cr;
	float nx;
	float ny;
	float nr;
	float ex;
	float ey;
	float er;
};

const int max_blobs = 5;
struct blob *blobs = NULL;

const int max_points = 50000;
const int point_size = 7;
const int num_points = max_points * point_size;
float *points = NULL;

struct blob *create_blobs(void) {
	struct blob *blobs = malloc(sizeof(struct blob) * max_blobs);
	if (!blobs) {
		fprintf(stderr, "couln't allocate memory for blobs");
		return NULL;
	}

	for (int i = 0; i < max_blobs; i++) {
		blobs[i].cx = 0.0 + (i * 0.1);
		blobs[i].cy = 0.0 + (i * 0.1);
		blobs[i].cr = rand_range(0.1, 3) * 0.1;

		blobs[i].nx = rand_range(-10, 10) * 0.1;
		blobs[i].ny = rand_range(-10, 10) * 0.1;
		blobs[i].nr = rand_range(1, 3) * 0.1;

		blobs[i].ex = 0.02 + rand_range(0.2, 1) * 0.1;
		blobs[i].ey = 0.02 + rand_range(0.2, 1) * 0.1;
		blobs[i].er = 0.02 + rand_range(0.2, 1) * 0.1;
	}

	return blobs;
}

void update_blobs(struct blob *bs) {
	for (int i = 0; i < max_blobs; i++) {
		struct blob *b = &(bs[i]);

		float dx = (b->nx - b->cx);
		float dy = (b->ny - b->cy);
		float dr = (b->nr - b->cr);

		float ax = fabs(dx);
		float ay = fabs(dy);
		float ar = fabs(dr);

		float vx = dx * b->ex;
		float vy = dy * b->ey;
		float vr = dr * b->er;

		b->cx += vx;
		b->cy += vy;
		b->cr += vr;

		if (ax < 0.01 && ay < 0.01 && ar < 0.01) {
			b->ex = rand_range(2, 6) * (rand_range(3, 7) * 0.005);
			b->ey = rand_range(2, 6) * (rand_range(1, 5) * 0.004);
			b->er = rand_range(2, 6) * (rand_range(2, 4) * 0.02);

			b->nx = rand_range(-10, 10) * 0.1;
			b->ny = rand_range(-10, 10) * 0.1;
			b->nr = rand_range(1, 30) * 0.01;
		}
	}
}

void create_points(void) {
	points = malloc(sizeof(float) * num_points);
	if (!points) {
		fprintf(stderr, "couln't allocate memory for points");
		return;
	}

	for (int i = 0; i < num_points; i += point_size) {
		// position
		points[i + 0] = rand_range(-1, 1);
		points[i + 1] = rand_range(-1, 1);

		// pointsize
		points[i + 2] = (10 + rand() % 20) * 0.1;

		// velocity
		points[i + 3] = rand_range(-3, 3) * 0.1;
		points[i + 4] = rand_range(-3, 3) * 0.1;

		// acceleation
		points[i + 5] = rand_range(-2, -9) * 0.1;
		points[i + 6] = rand_range(-2, -9) * 0.1;
	}
}

void update_points(void) {
	for (int i = 0; i < num_points; i += point_size) {
		float x = points[i];
		float y = points[i + 1];
		float r = points[i + 2];
		float vx = points[i + 3];
		float vy = points[i + 4];
		float ax = points[i + 5];
		float ay = points[i + 6];

		vx += ax;
		vy += ay;

		ax *= 0;
		ay *= 0;

		for (int k = 0; k < max_blobs; k++) {
			struct blob *b = &(blobs[k]);
			float dx = (x)-b->cx;
			float dy = (y)-b->cy;
			float dist = sqrt(dx * dx + dy * dy);

			if (dist < b->cr) {
				points[i + 0] = b->cx + (dx / dist) * b->cr;
				points[i + 1] = b->cy + (dy / dist) * b->cr;
				points[i + 2] = (10 + rand() % 40) * 0.09;
				x = points[i + 0];
				y = points[i + 1];
				r = points[i + 2];
			}
		}

		x += vx * 0.01;
		y += vy * 0.01;

		if (x > 1.0) {
			x = -1.0;
		} else if (x < -1.0) {
			x = 1.0;
		}
		if (y > 1.0) {
			y = -1.0;
		} else if (y < -1.0) {
			y = 1.0;
		}

		points[i + 0] = x;
		points[i + 1] = y;
		points[i + 2] = r;
	}
}

void size_callback(GLFWwindow *window, int width, int height) {
	(void)(window);
	glViewport(0, 0, width, height);
}

int main(void) {
	srand(time(NULL));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "  ", NULL, NULL);

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	int x = (int)((mode->width - WIDTH) * 0.5);
	int y = (int)((mode->height - HEIGHT) * 0.5);

	glfwSetWindowPos(window, x, y);
	glfwSetFramebufferSizeCallback(window, size_callback);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	GLuint sp = create_program("vert.glsl", "frag.glsl");

	GLint resolutionLoc;

	blobs = create_blobs();
	create_points();

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(GLfloat), points,
	             GL_STATIC_DRAW);

	int stride = sizeof(float) * point_size;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride,
	                      (void *)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glUseProgram(sp);

	GLint u_resolution = glGetUniformLocation(sp, "u_resolution");
	GLint u_time = glGetUniformLocation(sp, "u_time");

	double starttime = glfwGetTime();

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currtime = glfwGetTime();
		double elapsedtime = currtime - starttime;
		starttime = currtime;

		glUniform2f(u_resolution, (float)w, (float)h);
		glUniform1f(u_time, starttime);

		update_blobs(blobs);
		update_points();

		glBufferSubData(GL_ARRAY_BUFFER, 0,
		                num_points * sizeof(GLfloat), points);

		glDrawArrays(GL_POINTS, 0, max_points);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	if (blobs)
		free(blobs);
	if (points)
		free(points);

	glfwTerminate();
	return EXIT_SUCCESS;
}
