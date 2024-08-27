#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <array>

#include "Model.h"
#include "Camera.h"
#include "WindowMode.h"
#include "ShadingType.h"
#include "Light.h"
#include "Callbacks.h"

class SceneManager
{
	friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	friend void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	SceneManager();
	int init();
	int run();
private:
	// === SHADERS ===
	ShadingType m_shadingType = ShadingType::Phong;
	Shader m_flatShader;
	Shader m_gouraudShader;
	Shader m_phongShader;
	Shader m_depthShader;

	// === LIGHTS & MATERIAL ===
	std::vector<PointLight> m_pointLights;
	unsigned int m_depthCubeMapArrayFBO;
	unsigned int m_depthCubeMapArray;

	// === FOG ===
	bool m_fogActive = false;
	float m_fogIntensity = 0.5;
	int m_fogColor = 255;

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

	float m_range = 10.0f;
	float m_step = 1.0f;

	float m_accumTime = 0.0f;
	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;

	// === GUI ===
	std::array<float, 3> m_backgroundColor = { 1.0f, 1.0f, 1.0f };

	// === DATA ===
	Model m_chessBoard;

	Model m_whiteKing;
	Model m_whiteQueen;
	std::array<Model, 2> m_whiteRooks;
	std::array<Model, 5> m_whitePawns;
	
	Model m_blackKing;
	Model m_blackQueen;
	std::array<Model, 2> m_blackRooks;
	Model m_blackKnight;
	std::array<Model, 5> m_blackPawns;

	// === METHODS ===
	int arrange();
	int loadModels();
	void initShadowMapping();
	void moveCamera();
	void renderUI();
	void renderPointLightDepthMap(const PointLight& light);
	void renderScene();
	// void renderPointLightMenu();
	// void youSpinMeRound();
	void renderModels(Shader& shader);
	int terminate();
};

#endif