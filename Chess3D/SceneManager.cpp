#include "SceneManager.h"

SceneManager::SceneManager() : m_camera(), m_title("Chess3D") {
    m_pointLights.emplace_back(glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(0.6f), glm::vec3(0.0f, 0.0f, 5.0f), 1.0, 0.045, 0.0075);
}

int SceneManager::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(m_monitor, nullptr, nullptr, &m_width, &m_height);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, cursor_pos_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetWindowUserPointer(m_window, this);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

    initShadowMapping();

    for (PointLight& light : m_pointLights) {
        light.init();
    }

    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    //ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);

    return arrange();
}

int SceneManager::loadModels() {
    // TODO: maybe add some error checking in model class
    // === CHESSBOARD ===
    m_chessBoard = Model("WoodenChessBoard_v1_L3.123cf3414759-151a-4f2d-be41-f4e663d384e3/12938_WoodenChessBoard_v1_l3.obj");
    return 0;
}

void SceneManager::initShadowMapping()
{
    // init depth map array FBO
    glGenFramebuffers(1, &m_depthCubeMapArrayFBO);

    // init cube depth map array
    glGenTextures(1, &m_depthCubeMapArray);

    // assign 2D texture to each face of the cube
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubeMapArray);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, m_pointLights.size() * 6);

    // texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // bind depth cube to FBO as depth attatchment
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapArrayFBO);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMapArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneManager::moveCamera() {
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        m_camera.ProcessKeyboard(FORWARD, m_deltaTime);
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        m_camera.ProcessKeyboard(BACKWARD, m_deltaTime);
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        m_camera.ProcessKeyboard(LEFT, m_deltaTime);
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        m_camera.ProcessKeyboard(RIGHT, m_deltaTime);
    }
}

int SceneManager::arrange() {
    // === SHADERS ===
    m_goroudShader = Shader("shaders/goroud.vert", "shaders/goroud.frag");
    m_depthShader = Shader("shaders/depth.vert", "shaders/depth.frag");

    // === DATA ===
    if (loadModels()) {
        std::cerr << "Error loading models" << std::endl;
        return 1;
    }

    return 0;
}

int SceneManager::run() {
    while (!glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
        m_accumTime += m_deltaTime;

        std::ostringstream ss;
        ss << "[";
        ss.precision(0);
        ss << std::fixed << 2137; // TODO: add FPS
        ss << " FPS] " << m_title;
        glfwSetWindowTitle(m_window, ss.str().c_str());
        moveCamera();

        glClearColor(m_backgroundColor.at(0), m_backgroundColor.at(1), m_backgroundColor.at(2), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const PointLight& light : m_pointLights) {
            renderPointLightDepthMap(light);
        }
        renderScene();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return terminate();
}

void SceneManager::renderPointLightDepthMap(const PointLight& light)
{
    // TODO: use render scene
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, light.depthMapFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapArrayFBO);

    glClear(GL_DEPTH_BUFFER_BIT);
    m_depthShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1));
    m_depthShader.setMat4("modelMatrix", model);
    m_depthShader.setVec3("lightPos", light.position);

    for (unsigned int i = 0; i < 6; i++)
    {
        // GLenum face =  light.id * 6 + i;
        GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, light.depthCubeMap, 0);
        //glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMapArray, 0, face);
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
        m_depthShader.setMat4("viewMatrix", light.shadowTransformataions[i]);
        m_chessBoard.Draw(m_depthShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneManager::renderScene() {
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_width / (float)m_height, 0.1f, 100.0f);
    glm::mat4 view = m_camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1));

    Shader* shader = &m_goroudShader;

    shader->use();
    shader->setMat4("projMatrix", projection);
    shader->setMat4("viewMatrix", view);
    shader->setMat4("modelMatrix", model);
    shader->setVec3("cameraPos", m_camera.Position);

    // material
    shader->setVec3("material.ambient", m_chessBoard.material.ambient);
    shader->setVec3("material.diffuse", m_chessBoard.material.diffuse);
    shader->setVec3("material.specular", m_chessBoard.material.specular);
    shader->setFloat("material.shininess", m_chessBoard.material.shininess);

    // point lights TODO: better management
    for (int i = 0; i < m_pointLights.size(); i++) {
        std::string light = "pointLights[" + std::to_string(i) + "]";
        shader->setVec3(light + ".ambient", m_pointLights[i].ambient);
        shader->setVec3(light + ".diffuse", m_pointLights[i].diffuse);
        shader->setVec3(light + ".specular", m_pointLights[i].specular);
        shader->setVec3(light + ".position", m_pointLights[i].position);
        shader->setVec3("ligthPosition", m_pointLights[i].position);
        shader->setFloat(light + ".k0", m_pointLights[i].constant);
        shader->setFloat(light + ".k1", m_pointLights[i].linear);
        shader->setFloat(light + ".k2", m_pointLights[i].quadratic);
        shader->setFloat("farPlane", FAR_PLANE_PL);
    }
    shader->setInt("pointLightCount", static_cast<int>(m_pointLights.size()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->ID, "depthMap"), 0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, m_pointLights[0].depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubeMapArray);

    // draw scene
    m_chessBoard.Draw(*shader);
}

int SceneManager::terminate() {
    glfwTerminate();

    return 0;
}