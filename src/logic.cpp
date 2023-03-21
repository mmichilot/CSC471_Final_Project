#include "Application.h"
#include <glm/gtx/string_cast.hpp>

void Application::checkDrumInteraction()
{
    glm::vec2 playerPos = glm::vec2(camera.Position.x, camera.Position.z);

    // Determine whether player is within range to interact w/ drums
    float CP = (float) sqrt(pow(playerPos.x - useDrumsCenter.x, 2) + 
                            pow(playerPos.y - useDrumsCenter.y, 2));
    
    if ((CP <= useDrumsRadius) && !useDrums) {
        useDrums = true;
        freeRoam = false;
    }
    else if (useDrums) {
        useDrums = false;
        freeRoam = true;
        fixedCam = false;
    }
}

void Application::drumLogic()
{
    // Poll all keys
    snare.playLeft  = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_F) == GLFW_PRESS);
    snare.playRight = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_J) == GLFW_PRESS);
    hi_hat.playLeft  = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_D) == GLFW_PRESS);
    hi_hat.playRight = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_K) == GLFW_PRESS);
    kick.playLeft  = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS);
    kick.playRight = (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS);

    /* Snare */
    if (snare.playLeft && snare.leftFrameCount >= 18) {
        lightingSystem.setColor(stageLights[0], glm::vec3(random(), random(), random()));
        audioSystem.play(snare.source_id);
        snare.leftFrameCount = 0;
    }
    else
        snare.leftFrameCount++;

    if (snare.playRight && snare.rightFrameCount >= 18) {
        lightingSystem.setColor(stageLights[1], glm::vec3(random(), random(), random()));
        audioSystem.play(snare.source_id);
        snare.rightFrameCount = 0;
    }
    else
        snare.rightFrameCount++;

}

bool Application::checkPlayerCollisions()
{
    // Check if player is colliding w/ drums
    bool drumCollisionX = drum_set->bb.max.x >= playerBB.min.x &&
        playerBB.max.x >= drum_set->bb.min.x;
    bool drumCollisionZ = drum_set->bb.max.z >= playerBB.min.z &&
        playerBB.max.z >= drum_set->bb.min.z;
    
    if (drumCollisionX && drumCollisionZ) 
        return true;

    // Check if player is colliding w/ stage walls
    if (playerBB.max.x >= stageWidth/2.0f || playerBB.min.x <= -stageWidth/2.0f)
        return true;

    if (playerBB.max.z >= (stageCenter.z + stageDepth/2))
        return true;

    return false;
    
}

void Application::sceneLogic()
{
    // Update camera position during free roam
    if (freeRoam) {
        // Save previous player position and bounding box
        glm::vec3 prevPos = camera.Position;
        BoundingBox prevBB = playerBB;
    
        // Get next camera position
        if (pressedUp)    camera.move(FORWARD, deltaTime);
        if (pressedDown)  camera.move(BACKWARD, deltaTime);
        if (pressedLeft)  camera.move(LEFT, deltaTime);
        if (pressedRight) camera.move(RIGHT, deltaTime);
        camera.Position.y = playerHeight;
        
        // Compute new bounding box
        playerBB.min = glm::vec3(camera.Position.x-playerWidth/2.0f, 0.0f, camera.Position.z-0.2f);
        playerBB.max = glm::vec3(camera.Position.x+playerWidth/2.0f, playerHeight, camera.Position.z+0.2f);

        // If there is a collision, use previous position and bounding box
        if (checkPlayerCollisions()) {
            camera.Position = prevPos;
            playerBB = prevBB;
        }
    }

    if (useDrums)
        drumLogic();

    // Set correct camera
    if (useDrums) currCam = fixedCam ? &stageCam : &drumCam;
    else          currCam = &camera;
}