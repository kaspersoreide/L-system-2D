#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <iostream>
#include <GL/glew.h>
#include "glm/ext.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/quaternion.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "glutils.h"
#include "lsystem.h"
#include "turtle.h"
#include "loadshaders.h"

GLFWwindow* window;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch(key) {
	default:
		break;
	}
}

void init() {
	glfwInit();
	window = glfwCreateWindow(1280, 720, "<3", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glfwSetKeyCallback(window, keyCallback);
}

void runTests() {
}


int main() {
	init();
	runTests();

	mat4 Projection = perspective(
		1.2f,
		static_cast<float>(16) / 9,
		0.1f,
		100.0f
	);
	Lsystem tree;
	tree.addRule('F', "F[+F]F[-F][F]", 0.5f);
	tree.addRule('F', "F[+F]F", 0.25f);
	tree.addRule('F', "F[-F]F", 0.25f);
	tree.setAxiom("F");
	tree.iterate(6);
	
	Turtle turtle(0.03f, 0.436332f);
	turtle.build(tree.product);
	//for (vec2 v : turtle.vertices) {
	//	cout << v.x << ", " << v.y << "\n";
	//}
	GLuint treeVAO = vertexArray1x2f(turtle.vertices);
	GLuint treeShader = loadShaders("shaders/tree/vert.glsl", "shaders/tree/frag.glsl");
	glUseProgram(treeShader);
	glBindVertexArray(treeVAO);
	
	while (!glfwWindowShouldClose(window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glDrawArrays(GL_LINES, 0, turtle.vertices.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}