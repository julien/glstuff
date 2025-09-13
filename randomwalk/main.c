#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct particle {
	float x;
	float y;
	float w;
	float h;
	float vx;
	float vy;
	float life;
};

const unsigned int WIDTH = 1024;
const unsigned int HEIGHT = 800;

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

	struct particle p = {
	    (float)WIDTH * 0.5f,
	    (float)HEIGHT * 0.5f,
	    (float)WIDTH,
	    (float)HEIGHT,
	    (float)rand_range(-2, 2),
	    (float)rand_range(-2, 2),
	    100.0f,
	};

	size_t num_points = 1;
	size_t point_size = 2;
	size_t points_length = num_points * point_size;
	GLfloat *points = (float *)malloc(sizeof(float) * points_length);
	if (points == NULL) {
		fprintf(stderr,
		        "ERROR: Could not allocate memory for points\n");
		return 1;
	}

	points[0] = p.x;
	points[1] = p.y;

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) * num_points, points,
	             GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint sp = create_program("vert.glsl", "frag.glsl");

	glUseProgram(sp);

	GLint u_resolution = glGetUniformLocation(sp, "u_resolution");
	GLint u_time = glGetUniformLocation(sp, "u_time");
	GLint u_pointsize = glGetUniformLocation(sp, "u_pointsize");

	glUniform2f(u_resolution, WIDTH, HEIGHT);
	glUniform1f(u_pointsize, 4.0f);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	size_callback(window, WIDTH, HEIGHT);

	double previous_seconds = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		double current_seconds = glfwGetTime();
		previous_seconds = current_seconds;

		int speedX = rand_range(-5, 5);
		int speedY = rand_range(-5, 5);

		p.x += speedX;
		p.y += speedY;

		if (p.x > p.w) {
			p.x = 0;
		} else if (p.x < 0) {
			p.x = p.w;
		}
		if (p.y > p.h) {
			p.y = 0;
		} else if (p.y < 0) {
			p.y = p.h;
		}

		int old_length = (num_points * point_size);
		int new_length = old_length + point_size;

		points = (float *)realloc(points, sizeof(float) * new_length);
		if (points == NULL) {
			fprintf(stderr,
			        "ERROR: Could not reallocate memory for %d "
			        "points\n",
			        new_length);
			exit(1);
		}

		points[old_length] = p.x;
		points[old_length + 1] = p.y;
		num_points++;

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(u_resolution, WIDTH, HEIGHT);
		glUniform1f(u_time, previous_seconds);

		glBufferData(GL_ARRAY_BUFFER, sizeof(points) * num_points,
		             points, GL_STATIC_DRAW);
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, num_points);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	// clean up
	glDisable(GL_PROGRAM_POINT_SIZE);

	if (points != NULL) {
		free(points);
	}

	glfwTerminate();
	return 0;
}
