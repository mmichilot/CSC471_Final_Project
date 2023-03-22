#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "Application.h"
#include "MatrixStack.h"

void Application::renderSkysphere(shared_ptr<Program> prog)
{
    // Compute model matrix
    glm::mat4 S_o = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 T_o = glm::translate(glm::mat4(1.0f), currCam->Position);
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

void Application::renderScene(shared_ptr<Program> prog, bool useMaterials) {
    stage.renderStage(prog, useMaterials);

    MatrixStack Model;
    Model.translate(stageCenter);
    
    Model.pushMatrix();
        Model.translate(glm::vec3(0.0f, stageHeight-0.8f, -stageDepth/2.0f));
        Model.rotate(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Model.multMatrix(spotlight->getTransformMat());
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
        spotlight->Draw(prog, useMaterials);
    Model.popMatrix();

    Model.pushMatrix();
        Model.translate(glm::vec3(-stageWidth/2.0f+1.0f, stageHeight-0.8f, -stageDepth/2.0f));
        Model.rotate(glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Model.rotate(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Model.multMatrix(spotlight->getTransformMat());
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
        spotlight->Draw(prog, useMaterials);
    Model.popMatrix();

    Model.pushMatrix();
        Model.translate(glm::vec3(stageWidth/2.0f-1.0f, stageHeight-0.8f, -stageDepth/2.0f));
        Model.rotate(glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Model.rotate(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Model.multMatrix(spotlight->getTransformMat());
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
        spotlight->Draw(prog, useMaterials);
    Model.popMatrix();
}

void Application::renderObjects(shared_ptr<Program> prog, bool useMaterials)
{
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(drum_set->getTransformMat()));
    drum_set->Draw(prog, useMaterials);
    
    dummies.renderDummies(prog, playGuitar, useMaterials);
    
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(amplifier1->getTransformMat()));
    amplifier1->Draw(prog, useMaterials);

    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(amplifier2->getTransformMat()));
    amplifier2->Draw(prog, useMaterials);

    // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(mic_stand->getTransformMat()));
    // mic_stand->Draw(prog, useMaterials);

    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(piano->getTransformMat()));
    piano->Draw(prog, useMaterials);

}


void Application::renderShadowMaps(float aspect)
{
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
        
        // Get light space matrix for a given light
        glm::mat4 LightSpace = lightingSystem.getSpaceMatrix(stageLights[i], aspect);
        glUniformMatrix4fv(shadowProg->getUniform("lightSpaceMatrix"), 1, GL_FALSE, value_ptr(LightSpace));

        // Setup shadow map
        glClear(GL_DEPTH_BUFFER_BIT);
       
        glCullFace(GL_FRONT);
        // Only render objects
        renderObjects(shadowProg, false);
        glCullFace(GL_BACK);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shadowProg->unbind();
}

void Application::render()
{
    // Frame timing
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    timePassed += deltaTime;

    // Get current frame buffer size and spect ratio
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width/(float)height;

    renderShadowMaps(aspect);

    /*
     * Render scene normally (skysphere, lighting, shadow mapping)
     */

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute view and perspective matrices
    glm::mat4 Projection =  glm::perspective(45.0f, aspect, 0.01f, 100.0f);
    glm::mat4 View = currCam->GetViewMatrix();

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
        glUniform3f(prog->getUniform("material.emissive"), 0.0f, 0.0f, 0.0f);
        glUniform1i(prog->getUniform("lightsEnabled"), 1);
        
        for (unsigned int i = 0; i < stageLights.size(); i++) {
            string index = "[" + to_string(i) + "]";
            glUniformMatrix4fv(prog->getUniform("lightSpaceMatrix"+index), 1, GL_FALSE, 
                value_ptr(lightingSystem.getSpaceMatrix(stageLights[i], aspect)));
        }
        

        if (playGuitar) {
            lightingSystem.setDirection(stageLights[2], glm::vec3(0.5f*cos(glfwGetTime()), -0.7f, 0.5f*sin(glfwGetTime())+0.5f));
            lightingSystem.setColor(stageLights[2], glm::vec3(cos(0.5f*glfwGetTime())+0.5f, sin(0.5f*glfwGetTime())+0.5f, 1.0f));
        }
        lightingSystem.renderLights(prog, View);
        
        // Bind shadow maps
        glActiveTexture(GL_TEXTURE0 + 100);
        glUniform1i(prog->getUniform("shadowMaps"), 100);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
        
        renderScene(prog);
        renderObjects(prog);

        // Setup spotlights' "lights"
        glUniform1i(prog->getUniform("lightsEnabled"), 0);
        MatrixStack Model;
        Model.pushMatrix();
            Model.translate(glm::vec3(0.0f, stageHeight-1.0f, -stageDepth+0.4f));
            Model.scale(0.14f);
            Model.multMatrix(skysphere->getTransformMat());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
            glUniform3fv(prog->getUniform("material.emissive"), 1, value_ptr(lightingSystem.getColor(stageLights[2])));
            skysphere->Draw(prog);
        Model.popMatrix();

        Model.pushMatrix();
            Model.translate(glm::vec3(-stageWidth/2.0f+1.05f, stageHeight-1.0f, -stageDepth+0.4f));
            Model.scale(0.14f);
            Model.multMatrix(skysphere->getTransformMat());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
            glUniform3fv(prog->getUniform("material.emissive"), 1, value_ptr(lightingSystem.getColor(stageLights[0])));
            skysphere->Draw(prog);
        Model.popMatrix();

        Model.pushMatrix();
            Model.translate(glm::vec3(stageWidth/2.0f-1.05f, stageHeight-1.0f, -stageDepth+0.4f));
            Model.scale(0.14f);
            Model.multMatrix(skysphere->getTransformMat());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
            glUniform3fv(prog->getUniform("material.emissive"), 1, value_ptr(lightingSystem.getColor(stageLights[1])));
            skysphere->Draw(prog);
        Model.popMatrix();
    
    prog->unbind();

    sceneLogic();
}