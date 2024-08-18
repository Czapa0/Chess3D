#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>

#include "Model.h"
#include "Camera.h"
#include "WindowMode.h"
#include "Light.h"

class SceneManager
{
public:
	SceneManager();
	int init();
	int run();
private:
	// === SHADERS ===
	Shader m_flatShader;

	// === LIGHTS & MATERIAL ===
	std::vector<PointLight> m_pointLights;

	// === WINDOW ===
	GLFWwindow* m_window = nullptr;
	GLFWmonitor* m_monitor = nullptr;
	WindowMode m_mode = WindowMode::Windowed;
	std::string m_title;
	bool m_cursor = true;

	int m_width;
	int m_height;

	// === CAMERA ===
	Camera m_camera;
	float m_lastX = m_width / 2.0f;
	float m_lastY = m_height / 2.0f;
	float m_lastXPull = m_width / 2.0f;
	float m_lastYPull = m_height / 2.0f;
	bool m_pull = false;
	bool m_firstMouse = true;

	float m_accumTime = 0.0f;
	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;

	// === GUI ===
	std::array<float, 3> m_backgroundColor = { 1.0f, 1.0f, 1.0f };

	// === DATA ===
	Model m_chessBoard;

	// === METHODS ===
	int arrange();
	int loadModels();
	void moveCamera();
	// void renderUI();
	void renderScene();
	// void renderPointLightMenu();
	// void youSpinMeRound();
	int terminate();
};

#endif