#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GLFW/glfw3.h>

#include "shader_s.h"
#include "camera.h"
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void pointData2vao(const GLuint& VAO, const GLuint& VBO, std::vector<GLfloat> data);

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int mode = 0;
bool isFPS = false;

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
	glfwSetCursorPosCallback(window, cursorPosCallback);
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


	// cube
	float vertices[] = {
		// 背面
		-0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
		0.2f, -0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
		0.2f,  0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
		0.2f,  0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
		-0.2f,  0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
		-0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
		// 左
		-0.2f,  0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
		-0.2f,  0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
		-0.2f, -0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
		-0.2f, -0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
		-0.2f, -0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
		-0.2f,  0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
		// 正面
		-0.2f, -0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
		0.2f, -0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
		0.2f,  0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
		0.2f,  0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
		-0.2f,  0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
		-0.2f, -0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
		// 右
		0.2f,  0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
		0.2f,  0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
		0.2f, -0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
		0.2f, -0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
		0.2f, -0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
		0.2f,  0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
		// 下
		-0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.8f,
		0.2f, -0.2f, -0.2f,   0.8f, 0.2f, 0.8f,
		0.2f, -0.2f,  0.2f,   0.8f, 0.2f, 0.8f,
		0.2f, -0.2f,  0.2f,   0.8f, 0.2f, 0.8f,
		-0.2f, -0.2f,  0.2f,  0.8f, 0.2f, 0.8f,
		-0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.8f,
		// 上
		-0.2f,  0.2f, -0.2f,  0.8f, 0.8f, 0.2f,
		0.2f,  0.2f, -0.2f,   0.8f, 0.8f, 0.2f,
		0.2f,  0.2f,  0.2f,   0.8f, 0.8f, 0.2f,
		0.2f,  0.2f,  0.2f,   0.8f, 0.8f, 0.2f,
		-0.2f,  0.2f,  0.2f,  0.8f, 0.8f, 0.2f,
		-0.2f,  0.2f, -0.2f,  0.8f, 0.8f, 0.2f
	};

	float ortho[4] = { -3.0f, 3.0f, -3.0f, 3.0f };
	float perspect[3] = { 45.0f, 1.0f, 1.0f };
	float near_far[2] = { 0.1f, 100.0f };

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ImVec4 our_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	glEnable(GL_DEPTH_TEST);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		glfwPollEvents();
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (mode != 3) {
			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			// 模式选择框
			ImGui::RadioButton("Ortho Projection", &mode, 0);
			ImGui::RadioButton("Perspective Projection", &mode, 1);
			ImGui::RadioButton("View changing", &mode, 2);
			ImGui::RadioButton("Bonus", &mode, 3);

			ImGui::InputFloat4("Othro paras", ortho, 2);
			ImGui::InputFloat3("Perspect paras", perspect, 2);
			ImGui::InputFloat2("Near and Far", near_far, 2);

			switch (mode)
			{
			case 0:
				ImGui::Text("orthographic projection");
				break;
			case 1:
				ImGui::Text("perspective projection");
				break;
			case 2:
				ImGui::Text("View Changing");
				break;
			case 3:
				isFPS = true;
				break;
			default:
				break;
			}

			ImGui::End();
		}

		// render
		// ------
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!


		// render the triangle
		ourShader.use();

		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float Radius = 5.0f;
		float camPosX = sin(glfwGetTime()) * Radius;
		float camPosZ = cos(glfwGetTime()) * Radius;
		

		switch (mode)
		{
		case 0:
			model = glm::translate(model, glm::vec3(-1.5, 0, -1.5));
			view = glm::lookAt(
				glm::vec3(0, 0, 3),
				glm::vec3(0, 0, 0),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			projection = glm::ortho(ortho[0], ortho[1], ortho[2], ortho[3], near_far[0], near_far[1]);
			break;
		case 1:
			model = glm::translate(model, glm::vec3(-1.5, 0, -1.5));
			view = glm::lookAt(
				glm::vec3(0, 0, 3),
				glm::vec3(0, 0, 0),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			projection = glm::perspective(perspect[0], perspect[1] / perspect[2], near_far[0], near_far[1]);
			break;
		case 2:
			view = glm::lookAt(glm::vec3(camPosX, 0.0f, camPosZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
			break;
		case 3:
			if (isFPS) {
				model = glm::mat4(1.0f);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			view = camera.GetViewMatrix();
			projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
			break;
		default:
			break;
		}
		
		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		ourShader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isFPS = false;
		mode = 0;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
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