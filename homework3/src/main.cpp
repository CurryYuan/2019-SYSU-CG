#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GLFW/glfw3.h>

#include "shader_s.h"
#include "bresenham.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void pointData2vao(const GLuint& VAO, const GLuint& VBO, std::vector<GLfloat> data);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize OpenGL loader
	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile our shader program
	// ------------------------------------
	Shader ourShader("shader.vs", "shader.fs"); // you can name your shader files however you like

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	GLuint VBO[3], VAO[3];
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	// 三角形
	float vertices[] = {
	   -500, -500,
		100, 100,
		300, -600
	};

	auto triData = getTriData(Point(vertices[0], vertices[1]),
		Point(vertices[2], vertices[3]),
		Point(vertices[4], vertices[5]),false);
	pointData2vao(VAO[0], VBO[0], getGlPoint(triData, SCR_WIDTH, SCR_HEIGHT));
	int triSize = triData.size();

	triData = getTriData(Point(vertices[0], vertices[1]),
		Point(vertices[2], vertices[3]),
		Point(vertices[4], vertices[5]), true);
	pointData2vao(VAO[1], VBO[1], getGlPoint(triData, SCR_WIDTH, SCR_HEIGHT));
	int triSize1 = triData.size();
	
	// 圆
	GLint radius = 100;
	Point origin = Point(0, 0);

	int curr_radius = 99;

	
	auto circleData = getCircleData(origin, radius);
	//pointData2vao(VAO[2], VBO[2], getGlPoint(circleData, SCR_WIDTH, SCR_HEIGHT));

	ImVec4 our_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	int mode = 0;
	bool isChecked = false;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
			
			ImGui::ColorEdit3("clear color", (float*)&our_color); // Edit 3 floats representing a color

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			if (ImGui::Button("Triangle")) {
				mode = 0;	
			}
			if (mode == 0)
				ImGui::Checkbox("Is Filled", &isChecked);
			
			if (ImGui::Button("Circle")) {
				mode = 1;
			}
				
			if(mode == 1)
				ImGui::InputInt("Radius", &curr_radius);
			
			ImGui::End();
		}

		// render
		// ------
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ourShader.setColor("ourColor",our_color);
		glClear(GL_COLOR_BUFFER_BIT);

		// render the triangle
		ourShader.use();
		switch (mode)
		{
		case 0:
			if (isChecked) {
				glBindVertexArray(VAO[1]);
				glDrawArrays(GL_POINTS, 0, triSize1);
			}
			else {
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_POINTS, 0, triSize);
			}
			break;
		case 1:
			if (curr_radius != radius) {
				circleData.clear();
				circleData = getCircleData(origin, curr_radius);
				radius = curr_radius;
				pointData2vao(VAO[2], VBO[2], getGlPoint(circleData, SCR_WIDTH, SCR_HEIGHT));
				glDrawArrays(GL_POINTS, 0, circleData.size());
			}
			else {
				glBindVertexArray(VAO[2]);
				glDrawArrays(GL_POINTS, 0, circleData.size());
			}
			break;
		default:
			break;
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void pointData2vao(const GLuint & VAO, const GLuint & VBO, std::vector<GLfloat> data)
{
	// 1. 绑定VAO
	glBindVertexArray(VAO);
	// 2. 把顶点数组复制到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
	// 3. 设定顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}