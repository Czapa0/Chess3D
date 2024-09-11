#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

constexpr float ANIMATION_SPEED = 4.0;
constexpr float ANIMATION_ANGLE = 30.0;
constexpr float SUN_ROTATION = 5.0;
constexpr float MOON_ROTATION = 5.0;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <array>

#include "Model.h"
#include "Camera.h"
#include "WindowMode.h"
#include "ShadingType.h"
#include "RenderType.h"
#include "CameraType.h"
#include "Light.h"
#include "Callbacks.h"
#include "Skybox.h"
#include "Bezier.h"

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
	Shader m_depthShader_SL;
	Shader m_skyboxShader;
	Shader m_bezierShader;

	// === LIGHTS & MATERIAL ===
	bool m_pointLightsActive = true;
	std::vector<PointLight> m_pointLights;
	bool m_spotLightsActive = true;
	std::vector<SpotLight> m_spotLights;
	bool m_dayNightActive = true;
	DirectionalLight m_sun;
	DirectionalLight m_moon;

	// === DEPTH MAPPING ===
	// point lights
	unsigned int m_depthCubeMapArrayFBO;
	unsigned int m_depthCubeMapArrayStatic;
	unsigned int m_depthCubeMapArray;
	// spot lights
	unsigned int m_depthArrayFBO;
	unsigned int m_depthArray;
	// sun
	unsigned int m_depthSunFBO;
	unsigned int m_depthSun;
	// moon
	unsigned int m_depthMoonFBO;
	unsigned int m_depthMoon;

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

	// === ANIMATION ===
	// queen
	float m_queenSpeedFactor = 1.0;
	float m_queenTranslation = 0.0;
	float m_queenRotation = 0.0;
	// sun
	float m_sunRotation = 0.0;
	glm::vec3 m_sunColor;
	// moon
	float m_moonRotation = 0.0;
	glm::vec3 m_moonColor = glm::vec3(0.21f, 0.53f, 0.63f);

	// === CAMERA ===
	CameraType m_cameraType = CameraType::FreeRoam;
	Camera* m_activeCamera;
	Camera m_freeRoamCamera;
	Camera m_staticCamera;
	Camera m_followCamera;
	Camera m_firstPersonCamera;

	glm::vec3 m_followPoint;
	glm::vec3 m_firstPersonPoint;

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

	Skybox m_skybox;

	Bezier m_bezier;
	bool m_bezierActive = true;
	int m_bezierTesselataion = 128;

	// === METHODS ===
	// init
	int arrange();
	int loadModels();
	// shadow mapping
	void initPointLightShadowMapping();
	void initSpotLightShadowMapping();
	void initSunShadowMapping();
	void initMoonShadowMapping();
	void renderPointLightDepthMap(const PointLight& light, RenderType renderMode);
	void renderSpotlightDepthMap(const SpotLight& light);
	void renderSunDepthMap(const DirectionalLight& light);
	void renderMoonDepthMap(const DirectionalLight& light);
	// camera
	void moveCamera();
	// scene rendering
	void renderUI();
	void renderScene();
	void renderModels(Shader& shader, RenderType renderMode = RenderType::Everything);
	void renderBezier();
	void renderSkybox();
	// animation
	void animateBlackQueen();
	void animateSun();
	void animateMoon();
	// clean
	int terminate();
};

#endif