#include "Callbacks.h"

constexpr float thresholdOffsetDivisor = 50.0f;
constexpr float mouseMovementOffsetDivisor = 2.5f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    SceneManager* context = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));
    context->m_width = width;
    context->m_height = height;
}

void cursor_pos_callback(GLFWwindow* window, double xposIn, double yposIn) {
    SceneManager* context = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (context->m_firstMouse) {
        context->m_lastX = xpos;
        context->m_lastY = ypos;
        context->m_firstMouse = false;
    }

    if (context->m_pull) {
        float xoffset = (xpos - context->m_lastXPull) / mouseMovementOffsetDivisor;
        // reversed since y-coordinates go from bottom to top
        float yoffset = (context->m_lastYPull - ypos) / mouseMovementOffsetDivisor;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            context->m_camera.ProcessMouseMovementAndChangePosition(xoffset, yoffset, context->m_range);
        }
        else {
            context->m_camera.ProcessMouseMovement(xoffset, yoffset);
        }
    }

    if (!context->m_cursor) {
        float xoffset = xpos - context->m_lastX;
        // reversed since y-coordinates go from bottom to top
        float yoffset = context->m_lastY - ypos;
        context->m_camera.ProcessMouseMovement(xoffset, yoffset);
    }

    context->m_lastX = xpos;
    context->m_lastY = ypos;
    context->m_lastXPull = xpos;
    context->m_lastYPull = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    SceneManager* context = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button != GLFW_MOUSE_BUTTON_RIGHT) { return; }
    if (action == GLFW_PRESS) {
        context->m_lastXPull = static_cast<float>(xpos);
        context->m_lastYPull = static_cast<float>(ypos);
        context->m_pull = true;
    }
    else if (action == GLFW_RELEASE) {
        context->m_pull = false;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    SceneManager* context = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            context->m_camera.MovementSpeed = context->m_camera.MovementSpeed_Fast;
        }
        if (action == GLFW_RELEASE) {
            context->m_camera.MovementSpeed = context->m_camera.MovementSpeed_Slow;
        }
    }

    if (action != GLFW_PRESS) { return; }
    switch (key) {
    case GLFW_KEY_ESCAPE:
    {
        glfwSetWindowShouldClose(window, true);
        break;
    }
    case GLFW_KEY_C:
    {
        context->m_cursor = !context->m_cursor;
        if (context->m_cursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
    }
    case GLFW_KEY_TAB:
    {
        if (mods & GLFW_MOD_CONTROL) {
            const GLFWvidmode* mode = glfwGetVideoMode(context->m_monitor);
            if (context->m_mode == WindowMode::Fullscreen) {
                context->m_mode = WindowMode::Windowed;
                glfwSetWindowMonitor(window, nullptr, 200, 200,
                    mode->width / 2, mode->height / 2, GLFW_DONT_CARE);
            }
            else {
                context->m_mode = WindowMode::Fullscreen;
                glfwSetWindowMonitor(window, context->m_monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
        }
        break;
    }
    }
}

