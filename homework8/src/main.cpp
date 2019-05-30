#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include "stb_image.h"
#include <GLFW/glfw3.h>

#include "shader_s.h"
#include "camera.h"
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void pointData2vao(const GLuint& VAO, const GLuint& VBO, std::vector<GLfloat> data);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
vector<glm::vec3>::iterator findPointCanControlled(const float xpos, const float ypos, const float threshold);
vector<vector<float>> calculate(vector<glm::vec3> poss, int precision);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// Global value
vector<glm::vec3> poss = { { 353.0f, 383.0f, 0.0f }, { 670.0f, 266.0f, 0.0f }};


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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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


	// build and compile our shader zprogram
	// ------------------------------------

	Shader pointShader("./Shader/pointShader.vs", "./Shader/pointShader.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// 生成顶点数据 t
	float step = 0.001;
	std::vector<float> data;
	data.resize(int(1 / step));
	for (int i = 0; i < data.size(); ++i) {
		data[i] = i * step;
	}

	// 所有任务的VBO构建
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_TRUE, 1 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint pVAO, pVBO;
	glGenVertexArrays(1, &pVAO);
	glGenBuffers(1, &pVBO);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Menu");

		ImGui::Text("Use left mouse button to select or move control points");
		ImGui::Text("Use right mouse button to remove the control points");

		ImGui::End();

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render Control Points
		auto po = calculate(poss, 500);

		auto controlPoints2dataVector = [po]() -> vector<GLfloat> {
			vector<GLfloat> res;
			res.clear();
			for (auto c : po) {
				res.push_back(c[0]);
				res.push_back(c[1]);
				res.push_back(0);
			}
			// 将数据归一化到[-1, 1]
			for (int i = 0; i < res.size(); i = i + 3) {
				auto norx = (2 * res[i]) / SCR_WIDTH - 1;
				auto nory = 1 - (2 * res[i + 1]) / SCR_HEIGHT;
				res[i] = norx;
				res[i + 1] = nory;
			}
			return res;
		};
		auto pointData = controlPoints2dataVector();

		glBindVertexArray(pVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pVBO);
		glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(GLfloat), pointData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		pointShader.use();
		pointShader.setVec4("ourColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		glBindVertexArray(pVAO);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, pointData.size() / 3);
		glBindVertexArray(0);

		auto controlPoints2dataVector1 = []() -> vector<GLfloat> {
			vector<GLfloat> res;
			res.clear();
			for (auto c : poss) {
				res.push_back(c[0]);
				res.push_back(c[1]);
				res.push_back(c[2]);
			}
			// 将数据归一化到[-1, 1]
			// 归一化还有问题
			for (int i = 0; i < res.size(); i = i + 3) {
				auto norx = (2 * res[i]) / SCR_WIDTH - 1;
				auto nory = 1 - (2 * res[i + 1]) / SCR_HEIGHT;
				res[i] = norx;
				res[i + 1] = nory;
			}
			return res;
		};
		pointData = controlPoints2dataVector1();

		glBindVertexArray(pVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pVBO);
		glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(GLfloat), pointData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		pointShader.use();
		pointShader.setVec4("ourColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		glBindVertexArray(pVAO);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, pointData.size() / 3);
		glBindVertexArray(0);


		auto glfwPos2nocPos = [](const glm::vec3 p) -> glm::vec3 {
			glm::vec3 res;
			res.x = (2 * p.x) / SCR_WIDTH - 1;
			res.y = 1 - (2 * p.y) / SCR_HEIGHT;
			res.z = 0.0f;
			return res;
		};


		// render
		// ------
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

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


bool isPointInVector(const float xpos, const float ypos) {
	return find(poss.begin(), poss.end(), glm::vec3(xpos, ypos, 0.0f)) != poss.end();
}

void addPoint(const float xpos, const float ypos) {
	poss.push_back({ xpos, ypos, 0.0f });
}

vector<glm::vec3>::iterator findPointCanControlled(const float xpos, const float ypos, const float threshold) {
	// 粗略查找点击范围中是否有可以控制的点
	vector<glm::vec3>::iterator  res = poss.end();
	auto dist = [&xpos, &ypos](const vector<glm::vec3>::iterator iter) -> float {
		return pow((xpos - iter->x), 2) + pow((ypos - iter->y), 2);
	};
	for (auto iter = poss.begin(); iter != poss.end(); ++iter) {
		auto dis = dist(iter);
		if (dis < threshold) {
			if (res == poss.end()) { res = iter; }
			else { res = (dist(res) < dis) ? res : iter; }
		}
	}

	return res;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		// add one point on the canvas  && move the selected points
		if (action == GLFW_PRESS) {
			if (!isPointInVector(xpos, ypos)) {
				// add the selected point
				addPoint(xpos, ypos);
				cout << "add point" << xpos << "  " << ypos << endl;
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		// delete one point on the canvas	
		poss.erase(findPointCanControlled(xpos, ypos, 80));
	}
}

/**
* @param poss      贝塞尔曲线控制点坐标
* @param precision 精度，需要计算的该条贝塞尔曲线上的点的数目
* @return 该条贝塞尔曲线上的点（二维坐标）
*/
vector<vector<float>> calculate(vector<glm::vec3> poss, int precision) {

	//维度，坐标轴数（二维坐标，三维坐标...）
	int dimersion = 2;

	//贝塞尔曲线控制点数（阶数）
	int number = poss.size();

	//控制点数不小于 2 ，至少为二维坐标系
	if (number < 2 || dimersion < 2)
		return vector<vector<float>>();

	vector<vector<float>> result;
	result.resize(precision);

	for (int i = 0; i < precision; ++i) {
		result[i].resize(dimersion);
	}

	//计算杨辉三角
	int* mi = new int[number];
	mi[0] = mi[1] = 1;
	for (int i = 3; i <= number; i++) {

		int* t = new int[i - 1];
		for (int j = 0; j < i - 1; j++) {
			t[j] = mi[j];
		}

		mi[0] = mi[i - 1] = 1;
		for (int j = 0; j < i - 2; j++) {
			mi[j + 1] = t[j] + t[j + 1];
		}
	}

	//计算坐标点
	for (int i = 0; i < precision; i++) {
		float t = (float)i / precision;
		for (int j = 0; j < dimersion; j++) {
			float temp = 0.0f;
			for (int k = 0; k < number; k++) {
				temp += pow(1 - t, number - k - 1) * poss[k][j] * pow(t, k) * mi[k];
			}
			result[i][j] = temp;
		}
	}

	return result;
}
