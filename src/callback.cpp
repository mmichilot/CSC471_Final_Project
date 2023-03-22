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

    // Camera
    if (key == GLFW_KEY_W)    pressedUp    = updateKeyState(action);
    if (key == GLFW_KEY_S)  pressedDown  = updateKeyState(action);
    if (key == GLFW_KEY_A)  pressedLeft  = updateKeyState(action);
    if (key == GLFW_KEY_D) pressedRight = updateKeyState(action);
    
    // Drum Interaction
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        checkDrumInteraction();
        checkGuitaristInteraction();
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS && useDrums) {
        fixedCam = !fixedCam;
    }
    
}

void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - (float) ypos;

    lastX = (float) xpos;
    lastY = (float) ypos; 
    
    currCam->look(xoffset, yoffset);
}

void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}