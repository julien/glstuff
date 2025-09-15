#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"
#include "sprite.h"

#include <stdio.h>
#include <time.h>

int WIDTH = 1024;
int HEIGHT = 768;
GLfloat view_matrix[] = {
    2.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f,
};

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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "  ", NULL, NULL);

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	int wx = (int)((mode->width - WIDTH) * 0.5);
	int wy = (int)((mode->height - HEIGHT) * 0.5);

	glfwSetWindowPos(window, wx, wy);
	glfwSetFramebufferSizeCallback(window, size_callback);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	struct spritepool *sprites = malloc(sizeof(struct spritepool));
	if (!sprites) {
		fprintf(stderr, "cloudn't allocate memory for sprites");
		exit(1);
	}

	spritepool_init(sprites);

	GLuint sp = create_program("vert.glsl", "frag.glsl");
	glUseProgram(sp);

	GLint u_matrix = u_matrix = glGetUniformLocation(sp, "u_matrix");
	GLint u_image = glGetUniformLocation(sp, "u_image");

	view_matrix[0] *= 1.0f / (float)WIDTH;
	view_matrix[5] *= -1.0f / (float)HEIGHT;

	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

	GLuint tex;
	load_texture("../img/alien.png", &tex, 0);
	glUniform1i(u_image, 0);

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);

	int fsize = sizeof(GLfloat);
	int stride = fsize * 5;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)12);
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	double previous_seconds = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int newsprites = (int)(elapsed_seconds * 10000.0);
		if (newsprites >= 5)
			newsprites = 5;

		for (int i = 0; i < newsprites; i++) {
			float x = (WIDTH * 0.5) + rand_range(-60, 60);
			float y = (HEIGHT * 0.5) + rand_range(-60, 60);
			float size = 8 + rand_range(8, 12);
			float vx = rand_range(-2, 2);
			float vy = rand_range(-2, 2);

			spritepool_create(sprites, x, y, size, size, vx, vy,
			                  100);
		}

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		spritepool_update(sprites);

		int quads = spritepool_draw(sprites);

		glBufferData(GL_ARRAY_BUFFER,
		             MAX_SPRITES * 30 * sizeof(GLfloat),
		             sprites->vertices, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, quads);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
