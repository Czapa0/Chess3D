#include "SceneManager.h"

SceneManager::SceneManager() : m_camera(glm::vec3(0.0f, 3.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f), m_title("Chess3D") {
    m_pointLights.emplace_back(glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(0.6f), glm::vec3(-1.0f, 3.0f, 1.0f), 1.0, 0.045, 0.0075);
    m_pointLights.emplace_back(glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(0.6f), glm::vec3(-1.0f, 3.0f, 1.0f), 1.0, 0.045, 0.0075);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);

    return arrange();
}

int SceneManager::loadModels() {
    // TODO: maybe add some error checking in model class
    glm::mat4 model(1.0);
    model = glm::scale(model, glm::vec3(0.1));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // === CHESSBOARD ===
    m_chessBoard = Model("models/Stone_Chess_Board_v1_L3.123c4360d402-eec2-4a3a-854b-0ad9ae539388/Stone_Chess_Board_v1_L3.123c4360d402-eec2-4a3a-854b-0ad9ae539388/12951_Stone_Chess_Board_v1_L3.obj", RenderType::Static, model);
    m_chessBoard.modelMatrix = glm::translate(m_chessBoard.modelMatrix, glm::vec3(0.0f, 0.0f, 0.5f));

    // === WHITE KING ===
    m_whiteKing = Model("models/Stone_Chess_King_Side_A_v2_L1.123cb493df42-46f1-49ef-8c89-479187ab8a22/Stone_Chess_King_Side_A_v2_L1.123cb493df42-46f1-49ef-8c89-479187ab8a22/12939_Stone_Chess_King_Side_A_V2_l1.obj", RenderType::Static, model);
    m_whiteKing.modelMatrix = glm::translate(m_whiteKing.modelMatrix, glm::vec3(10.7f, -0.6f, 0.0f));

    // === WHITE QUEEN ===
    m_whiteQueen = Model("models/Stone_Chess_Queen_Side_A_v2_L1.123ca8f563ae-8402-4fcd-b919-9d6c85add86d/Stone_Chess_Queen_Side_A_v2_L1.123ca8f563ae-8402-4fcd-b919-9d6c85add86d/12940_Stone_Chess_Queen_Side_A_V2_l1.obj", RenderType::Static, model);
    m_whiteQueen.modelMatrix = glm::translate(m_whiteQueen.modelMatrix, glm::vec3(16.1f, 21.2f, 0.0f));

    // === WHITE ROOKS ===
    Model wr("models/Stone_Chess_Rook_Side_A_v2_L1.123c700a78d6-9c36-43d8-94ef-18b0f9bdbf3d/Stone_Chess_Rook_Side_A_v2_L1.123c700a78d6-9c36-43d8-94ef-18b0f9bdbf3d/12941_Stone_Chess_Rook_Side_A_V2_l1.obj", RenderType::Static, model);
    for (Model& m : m_whiteRooks) {
        m = wr;
    }
    m_whiteRooks[0].modelMatrix = glm::translate(m_whiteRooks[0].modelMatrix, glm::vec3(27.3f, -0.2f, 0.0f));
    m_whiteRooks[1].modelMatrix = glm::translate(m_whiteRooks[1].modelMatrix, glm::vec3(11.1f, 21.6f, 0.0f));

    // === WHITE PAWNS ===
    Model wp("models/Stone_Chess_Pawn_Side_A_v2_L3.123c0f81bc65-2846-45af-9512-6e41230dea09/Stone_Chess_Pawn_Side_A_v2_L3.123c0f81bc65-2846-45af-9512-6e41230dea09/12944_Stone_Chess_Pawn_Side_A_V2_L3.obj", RenderType::Static, model);
    for (Model& m : m_whitePawns) {
        m = wp;
    }
    m_whitePawns[0].modelMatrix = glm::translate(m_whitePawns[0].modelMatrix, glm::vec3(0.0f, -0.2f, 0.0f));
    m_whitePawns[1].modelMatrix = glm::translate(m_whitePawns[1].modelMatrix, glm::vec3(10.8f, -0.2f, 0.0f));
    m_whitePawns[2].modelMatrix = glm::translate(m_whitePawns[2].modelMatrix, glm::vec3(27.0f, -0.2f, 0.0f));
    m_whitePawns[3].modelMatrix = glm::translate(m_whitePawns[3].modelMatrix, glm::vec3(32.4f, -0.2f, 0.0f));
    m_whitePawns[4].modelMatrix = glm::translate(m_whitePawns[4].modelMatrix, glm::vec3(37.8f, -0.2f, 0.0f));

    // === BLACK KING ===
    m_blackKing = Model("models/Stone_Chess_King_Side_B_v2_L1.123c481d677b-5169-455e-bf04-675a07aaa9aa/Stone_Chess_King_Side_B_v2_L1.123c481d677b-5169-455e-bf04-675a07aaa9aa/12945_Stone_Chess_King_Side_B_v2_l1.obj", RenderType::Static, model);
    m_blackKing.modelMatrix = glm::translate(m_blackKing.modelMatrix, glm::vec3(10.7f, 0.0f, 0.0f));

    // === BLACK QUEEN ===
    m_blackQueen = Model("models/Stone_Chess_Queen_Side_B_v2_L1.123c4dd4d516-7f3e-4b9a-abc1-a2acb1d7ceff/Stone_Chess_Queen_Side_B_v2_L1.123c4dd4d516-7f3e-4b9a-abc1-a2acb1d7ceff/12946_Stone_Chess_Queen_Side_B_V2_l1.obj", RenderType::Dynamic, model);
    m_blackQueen.modelMatrix = glm::translate(m_blackQueen.modelMatrix, glm::vec3(-5.4f, -27.4f, 0.0f));

    // == BLACK ROOKS ===
    Model br("models/Stone_Chess_Rook_Side_B_v2_L1.123c00b55eba-db8e-49e1-8930-92b018c0ef95/Stone_Chess_Rook_Side_B_v2_L1.123c00b55eba-db8e-49e1-8930-92b018c0ef95/12947_Stone_Chess_Rook_Side_B_v2_l1.obj", RenderType::Static, model);
    for (Model& m : m_blackRooks) {
        m = br;
    }
    m_blackRooks[0].modelMatrix = glm::translate(m_blackRooks[0].modelMatrix, glm::vec3(-11.2f, -0.6f, 0.0f));
    m_blackRooks[1].modelMatrix = glm::translate(m_blackRooks[1].modelMatrix, glm::vec3(-0.3f, -27.5f, 0.0f));

    // === BLACK KNIGHT ===
    m_blackKnight = Model("models/Stone_Chess_Knight_Side_B_v2_L1.123c5e1267f2-c8bc-4f6e-ac1d-b15c2c660b6e/Stone_Chess_Knight_Side_B_v2_L1.123c5e1267f2-c8bc-4f6e-ac1d-b15c2c660b6e/12949_Stone_Chess_Knight_Side_B_V2_l1.obj", RenderType::Static, model);
    m_blackKnight.modelMatrix = glm::scale(m_blackKnight.modelMatrix, glm::vec3(0.8));
    m_blackKnight.modelMatrix = glm::translate(m_blackKnight.modelMatrix, glm::vec3(-17.0f, -22.5f, 0.0f));

    // === BLACK PAWNS ===
    Model bp("models/Stone_Chess_Pawn_Side_B_v2_L3.123c116cc629-4730-45fd-a530-a8d93427dc2f/Stone_Chess_Pawn_Side_B_v2_L3.123c116cc629-4730-45fd-a530-a8d93427dc2f/12950_Stone_Chess_Pawn_Side_B_v2_l3.obj", RenderType::Static, model);
    for (Model& m : m_blackPawns) {
        m = bp;
    }
    m_blackPawns[0].modelMatrix = glm::translate(m_blackPawns[0].modelMatrix, glm::vec3(-0.2f, -0.2f, 0.0f));
    m_blackPawns[1].modelMatrix = glm::translate(m_blackPawns[1].modelMatrix, glm::vec3(-5.6, -0.2f, 0.0f));
    m_blackPawns[2].modelMatrix = glm::translate(m_blackPawns[2].modelMatrix, glm::vec3(-16.4f, -5.6f, 0.0f));
    m_blackPawns[3].modelMatrix = glm::translate(m_blackPawns[3].modelMatrix, glm::vec3(-32.8f, -0.2f, 0.0f));
    m_blackPawns[4].modelMatrix = glm::translate(m_blackPawns[4].modelMatrix, glm::vec3(-38.2f, -0.2f, 0.0f));

    return 0;
}

void SceneManager::initShadowMapping()
{
    // init depth map array FBO
    glGenFramebuffers(1, &m_depthCubeMapArrayFBO);

    // === STATIC CUBE ARRAY ===

    // init cube depth map array
    glGenTextures(1, &m_depthCubeMapArrayStatic);

    // assign 2D texture to each face of the cube
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubeMapArrayStatic);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, m_pointLights.size() * 6);

    // texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // === FIANL CUBE ARRAY ===

    // init cube depth map array
    glGenTextures(1, &m_depthCubeMapArray);

    // assign 2D texture to each face of the cube
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubeMapArray);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, m_pointLights.size() * 6);

    // texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // === BIND FBO ===

    // bind depth cube to FBO as depth attatchment
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapArrayFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMapArrayStatic, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete: " << fboStatus << std::endl;

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

void SceneManager::renderUI()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 750));
    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::BeginGroup();
    ImGui::Text("Shading");
    if (ImGui::RadioButton("Flat", m_shadingType == ShadingType::Flat)) {
        m_shadingType = ShadingType::Flat;
    }
    if (ImGui::RadioButton("Gouraud", m_shadingType == ShadingType::Gouraud)) {
        m_shadingType = ShadingType::Gouraud;
    }
    if (ImGui::RadioButton("Phong", m_shadingType == ShadingType::Phong)) {
        m_shadingType = ShadingType::Phong;
    }
    ImGui::EndGroup();
    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::Text("Fog");
    ImGui::Checkbox("Active", &m_fogActive);
    ImGui::SliderFloat("Intensity", &m_fogIntensity, 0.0f, 1.0f);
    ImGui::SliderInt("Color", &m_fogColor, 0, 255);
    ImGui::EndGroup();

    ImGui::End();
}

int SceneManager::arrange() {
    // === SHADERS ===
    m_flatShader = Shader("shaders/flat.vert", "shaders/flat.frag");
    m_gouraudShader = Shader("shaders/gouraud.vert", "shaders/gouraud.frag");
    m_phongShader = Shader("shaders/phong.vert", "shaders/phong.frag");
    m_depthShader = Shader("shaders/depth.vert", "shaders/depth.frag");

    // === DATA ===
    if (loadModels()) {
        std::cerr << "Error loading models" << std::endl;
        return 1;
    }

    return 0;
}

int SceneManager::run() {
    // === STATIC POINT LIGHT SHADOWS ===
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const PointLight& light : m_pointLights) {
        renderPointLightDepthMap(light, RenderType::Static);
    }

    // === RENDER LOOP ===
    while (!glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
        m_accumTime += m_deltaTime;

        std::ostringstream ss;
        ss << "[";
        ss.precision(0);
        ss << std::fixed << ImGui::GetIO().Framerate; // TODO: add FPS
        ss << " FPS] " << m_title;
        glfwSetWindowTitle(m_window, ss.str().c_str());
        moveCamera();
        animateBlackQueen();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(m_backgroundColor.at(0), m_backgroundColor.at(1), m_backgroundColor.at(2), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCopyImageSubData(
            m_depthCubeMapArrayStatic, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0,
            m_depthCubeMapArray, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0,
            SHADOW_WIDTH, SHADOW_HEIGHT, 6 * m_pointLights.size());

        for (const PointLight& light : m_pointLights) {
            renderPointLightDepthMap(light, RenderType::Dynamic);
        }

        renderScene();
        renderUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    return terminate();
}

void SceneManager::renderPointLightDepthMap(const PointLight& light, RenderType renderMode)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapArrayFBO);

    glClear(GL_DEPTH_BUFFER_BIT);
    m_depthShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1));
    m_depthShader.setMat4("modelMatrix", model);
    m_depthShader.setVec3("lightPos", light.position);
    m_depthShader.setFloat("farPlane", FAR_PLANE_PL);

    for (int i = 0; i < 6; i++)
    {
        GLenum face = light.id * 6 + i;
        // static render
        if (renderMode == RenderType::Static) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMapArrayStatic, 0, face);
        }
        // normal render
        else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMapArray, 0, face);
        }
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
        m_depthShader.setMat4("viewMatrix", light.shadowTransformataions[i]);
        renderModels(m_depthShader, renderMode);
    }

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0, 0); // TODO: maybe better handling needed
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneManager::renderScene() {
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_width / (float)m_height, 0.1f, 100.0f);
    glm::mat4 view = m_camera.GetViewMatrix();

    Shader* shader = &m_phongShader;
    switch (m_shadingType)
    {
    case ShadingType::Flat:
        shader = &m_flatShader;
        break;
    case ShadingType::Gouraud:
        shader = &m_gouraudShader;
        break;
    case ShadingType::Phong:
        shader = &m_phongShader;
        break;
    default:
        break;
    }

    shader->use();
    shader->setMat4("projMatrix", projection);
    shader->setMat4("viewMatrix", view);
    shader->setVec3("cameraPos", m_camera.Position);

    // fog
    shader->setFloat("fogIntensity", m_fogIntensity);
    float col = 1.0 * m_fogColor / 255;
    shader->setVec3("fogColor", glm::vec3(col, col, col));
    shader->setBool("fogActive", m_fogActive);

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
        shader->setFloat(light + ".k0", m_pointLights[i].constant);
        shader->setFloat(light + ".k1", m_pointLights[i].linear);
        shader->setFloat(light + ".k2", m_pointLights[i].quadratic);
    }
    shader->setFloat("farPlane", FAR_PLANE_PL);
    shader->setInt("pointLightCount", static_cast<int>(m_pointLights.size()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->ID, "depthMap"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubeMapArray);

    // draw scene
    renderModels(*shader);
}

void SceneManager::renderModels(Shader& shader, RenderType renderMode)
{
    m_chessBoard.Draw(shader, renderMode);

    m_whiteKing.Draw(shader, renderMode);
    m_whiteQueen.Draw(shader, renderMode);
    for (Model& rook : m_whiteRooks) {
        rook.Draw(shader, renderMode);
    }
    for (Model& pawn : m_whitePawns) {
        pawn.Draw(shader, renderMode);
    }

    m_blackKing.Draw(shader, renderMode);
    m_blackQueen.Draw(shader, renderMode);
    for (Model& rook : m_blackRooks) {
        rook.Draw(shader, renderMode);
    }
    m_blackKnight.Draw(shader, renderMode);
    for (Model& pawn : m_blackPawns) {
        pawn.Draw(shader, renderMode);
    }
}

void SceneManager::animateBlackQueen()
{
    float speed = 0.5f;
    m_blackQueen.modelMatrix = glm::translate(m_blackQueen.modelMatrix, glm::vec3(0.5f * m_deltaTime, 0.0f, 0.0f));
}

int SceneManager::terminate() {
    glfwTerminate();

    return 0;
}