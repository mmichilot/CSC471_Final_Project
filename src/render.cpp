#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "Application.h"
#include "MatrixStack.h"


void Application::renderSkysphere(shared_ptr<Program> prog)
{
    // Compute model matrix
    glm::mat4 S_o = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 T_o = glm::translate(glm::mat4(1.0f), camera.Position);
    glm::mat4 Model = T_o * S_o;
    
    // Setup texture
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(prog->getUniform("tex"), 0);
    glBindTexture(GL_TEXTURE_2D, skysphere_texture);

    // Render skysphere
    glDisable(GL_DEPTH_TEST);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model));
    skysphere->Draw(prog, false);
    glEnable(GL_DEPTH_TEST);
}

void Application::renderPlane(shared_ptr<Program> prog, bool useMaterials, int texture)
{
    // By default, disable texture use
    if (useMaterials) {
        glUniform1i(prog->getUniform("material.texture_diffuse_enable"), GL_FALSE);
        glUniform1i(prog->getUniform("material.texture_specular_enable"), GL_FALSE);

        // Enable diffuse texturing, if available
        if (texture != -1)
        {
            glUniform1i(prog->getUniform("material.texture_diffuse_enable"), GL_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(prog->getUniform("material.texture_diffuse1"), 0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        else
            glUniform3f(prog->getUniform("material.diffuse"), 0.5f, 0.5f, 0.5f);

        
        // Set specular properties
        glUniform3f(prog->getUniform("material.specular"), 0.1f, 0.1f, 0.1f);
        glUniform1f(prog->getUniform("material.shininess"), 32.0f);
    }

    // Render
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Application::renderStage(shared_ptr<Program> prog, bool useMaterials)
{
    auto Model = make_shared<MatrixStack>();
    
    // Stage transformations
    Model->translate(glm::vec3(0.0f, 0.0f, -3.0f));
    
    // Ground
    Model->pushMatrix();
        Model->translate(glm::vec3(0.0f, -1.0f, 0.0f));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        renderPlane(prog, useMaterials, stage_texture);
    Model->popMatrix();

    // // Ceiling
    // Model->pushMatrix();
    //     Model->translate(glm::vec3(0.0f, 5.0f, 0.0f));
    //     Model->rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //     glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
    //     renderPlane(prog, useMaterials);
    // Model->popMatrix();

    // Left Wall
    Model->pushMatrix();
        Model->translate(glm::vec3(-5.0f, 0.0f, 0.0f));
        Model->rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        renderPlane(prog, useMaterials);
    Model->popMatrix();

    // Right Wall
    Model->pushMatrix();
        Model->translate(glm::vec3(5.0f, 0.0f, 0.0f));
        Model->rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        renderPlane(prog, useMaterials);
    Model->popMatrix();

    // Back Wall
    Model->pushMatrix();
        Model->translate(glm::vec3(0.0f, 0.0f, 5.0f));
        Model->rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        renderPlane(prog, useMaterials);
    Model->popMatrix();

}

void Application::renderScene(shared_ptr<Program> prog, bool useMaterials) {
    // Create the matrix stacks
    auto Model = make_shared<MatrixStack>();
        
    renderStage(prog, useMaterials);
    
    // Render drum set
    Model->pushMatrix();
        Model->loadIdentity();
        Model->translate(glm::vec3(0.0f, -0.4f, -2.0f));
        Model->rotate(glm::radians((float) 220), glm::vec3(0.0f, 1.0f, 0.0f));
        Model->multMatrix(drum_set->getNormalizedMat());
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        drum_set->Draw(prog, useMaterials);
    Model->popMatrix();
}

void Application::render()
{
    // Frame timing
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Update camera position
    if (pressedW) camera.move(FORWARD, deltaTime);
    if (pressedS) camera.move(BACKWARD, deltaTime);
    if (pressedA) camera.move(LEFT, deltaTime);
    if (pressedD) camera.move(RIGHT, deltaTime);
    camera.Position.y = 0.6f;	// Set player height

    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width/(float)height;


    /*
     * Render depth map for each light
     */
    
    shadowProg->bind();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    
    for (unsigned int i = 0; i < stageLights.size(); i++)
    { 
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        // Get ligt space matrix for a given light
        glm::mat4 LightSpace = lightingSystem.getSpaceMatrix(stageLights[i], aspect);
        glUniformMatrix4fv(shadowProg->getUniform("lightSpaceMatrix"), 1, GL_FALSE, value_ptr(LightSpace));

        // Setup shadow map
        glClear(GL_DEPTH_BUFFER_BIT);
       
        glCullFace(GL_FRONT);
        renderScene(shadowProg, false);
        glCullFace(GL_BACK);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shadowProg->unbind();


    /*
     * Render scene normally (skysphere, lighting, shadow mapping)
     */

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute view and perspective matrices
    glm::mat4 Projection = glm::perspective(45.0f, aspect, 0.01f, 100.0f);
    glm::mat4 View = camera.GetViewMatrix();

    // Render skysphere
    skyProg->bind();
        glUniformMatrix4fv(skyProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
        glUniformMatrix4fv(skyProg->getUniform("V"), 1, GL_FALSE, value_ptr(View));
        renderSkysphere(skyProg);
    skyProg->unbind();

    // Render scene w/ lighting
    prog->bind();
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View));
        
        for (unsigned int i = 0; i < stageLights.size(); i++) {
            string index = "[" + to_string(i) + "]";
            glUniformMatrix4fv(prog->getUniform("lightSpaceMatrix"+index), 1, GL_FALSE, 
                value_ptr(lightingSystem.getSpaceMatrix(stageLights[i], aspect)));
        }
        

        // lightingSystem.setPosition(stageLights[2], glm::vec3(3*glm::cos(glfwGetTime()), 0.0f, -6.0f));
        // lightingSystem.setPosition(stageLights[1], glm::vec3(2*glm::cos(glfwGetTime()), 0.0f, -6.0f));
        // lightingSystem.setColor(stageLights[1], glm::vec3(0.5*glm::cos(glfwGetTime())+0.5));
        // lightingSystem.setDirection(stageLights[3], glm::vec3(glm::cos(glfwGetTime()), -1.0f, 1.0f));

        lightingSystem.renderLights(prog, View);
        
        // Bind shadow maps
        glActiveTexture(GL_TEXTURE0 + 100);
        glUniform1i(prog->getUniform("shadowMaps"), 100);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
        
        renderScene(prog);
    prog->unbind();
}