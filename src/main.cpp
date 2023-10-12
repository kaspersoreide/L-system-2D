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
	tree.addRule('F', "F[+F]F", 0.3f);
	tree.addRule('F', "F[-F]F", 0.2f);
	tree.addRule('[', "[", 1.0f);
	tree.addRule(']', "]", 1.0f);
	tree.addRule('+', "+", 1.0f);
	tree.addRule('-', "-", 1.0f);
	tree.setAxiom("F");
	for (int i = 0; i < 5; i++) tree.iterateParallel(1);
	//tree.iterate(6);
	
	Turtle turtle(0.02f, 0.2f, 0.136332f);
	//turtle.build(tree.product);
	turtle.buildGPU(tree.outputBuffer);
	
	/*
	Lsystem triangle;
	triangle.addRule('B', "A-B-A", 1.0f);
	triangle.addRule('A', "B+A+B", 1.0f);
	triangle.setAxiom("A");
	triangle.iterate(1);
	Turtle turtle2(0.003f, 0.66667f, 1.047198f);
	turtle2.build(triangle.product);
	
	//for (vec2 v : turtle.vertices) {
	//	cout << v.x << ", " << v.y << "\n";
	//}
	//GLuint treeVAO = vertexArray1x2f(turtle.vertices);
	GLuint triangleVAO = vertexArray1x2f(turtle2.vertices);
	*/
	GLuint treeShader = loadShaders("shaders/tree/vert.glsl", "shaders/tree/frag.glsl");
	glUseProgram(treeShader);
	glBindVertexArray(turtle.VAO);
	
	while (!glfwWindowShouldClose(window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glDrawArrays(GL_LINES, 0, turtle.vertexCount);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}