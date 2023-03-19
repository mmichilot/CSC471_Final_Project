#include "Application.h"

bool updateKeyState(int action) {
    if (action == GLFW_PRESS)   return true;
    if (action == GLFW_RELEASE) return false;
    return true;
}

void Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W) pressedW = updateKeyState(action);
    if (key == GLFW_KEY_S) pressedS = updateKeyState(action);
    if (key == GLFW_KEY_A) pressedA = updateKeyState(action);
    if (key == GLFW_KEY_D) pressedD = updateKeyState(action);
}

void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - (float) ypos;

    lastX = (float) xpos;
    lastY = (float) ypos; 
    
    camera.look(xoffset, yoffset);
}

void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}