#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Model.h"
#include "WindowMode.h"

class SceneManager
{
public:
	SceneManager();
	int init();
	int run();
private:
	// === SHADERS ===
	Shader m_flatShader;

	// === WINDOW ===
	GLFWwindow* m_window = nullptr;
	GLFWmonitor* m_monitor = nullptr;
	WindowMode m_mode = WindowMode::Windowed;
	std::string m_title;
	bool m_cursor = true;

	int m_width;
	int m_height;
};

#endif