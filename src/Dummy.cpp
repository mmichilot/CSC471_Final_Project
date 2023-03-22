#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "Dummy.h"
#include "MatrixStack.h"
#include "Application.h"
#include "common.h"

using namespace std;

void Dummy::init()
{
    // Get pointers to body part
    head  = &model->meshes[2];
    neck = &model->meshes[16];
    torso = &model->meshes[1];
    hip   = &model->meshes[0];
    waist = &model->meshes[3];
    
    r_pelvic = &model->meshes[5];
    r_upper_leg = &model->meshes[14];
    r_knee = &model->meshes[12];
    r_lower_leg = &model->meshes[13];
    r_ankle = &model->meshes[4];
    r_foot = &model->meshes[9];

    r_shoulder = &model->meshes[7];
    r_upper_arm = &model->meshes[6];
    r_elbow = &model->meshes[8];
    r_forearm = &model->meshes[10];
    r_wrist = &model->meshes[15];
    r_hand = &model->meshes[11];
    
    l_pelvic = &model->meshes[18];
    l_upper_leg = &model->meshes[27];
    l_knee = &model->meshes[25];
    l_lower_leg = &model->meshes[26];
    l_ankle = &model->meshes[17];
    l_foot = &model->meshes[22];

    l_shoulder = &model->meshes[20];
    l_upper_arm = &model->meshes[19];
    l_elbow = &model->meshes[21];
    l_forearm = &model->meshes[23];
    l_wrist = &model->meshes[28];
    l_hand = &model->meshes[24];

    cout << "Number of meshes for dummy: " << model->meshes.size() << endl;
}

void Dummy::renderDummies(shared_ptr<Program> prog, bool playGuitar, bool useMaterials)
{
    renderGuitarist(prog, playGuitar, useMaterials);
}

void Dummy::renderGuitarist(shared_ptr<Program> prog, bool playGuitar, bool useMaterials)
{
    MatrixStack Model;
    Model.translate(guitaristPos);
    Model.rotate(glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Render guitarist
    Model.pushMatrix();
        Model.scale(0.01f);

        // Torso
        Model.pushMatrix();

            Model.translate(torso->moveToZero());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
            torso->Draw(prog, useMaterials);

            // Neck
            Model.pushMatrix();
                Model.translate(-1.0f * neck->moveToZero());
                if (playGuitar)
                    Model.rotate(0.5f*sin(3.0f*glfwGetTime()) - 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
                Model.translate(neck->moveToZero());
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                neck->Draw(prog, useMaterials);
                head->Draw(prog, useMaterials);
            Model.popMatrix();

            // Left arm
            Model.pushMatrix();
                // Left upper arm
                Model.translate(-1.0f * l_shoulder->moveToZero());
                Model.rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.3f, 0.0f));
                Model.translate(l_shoulder->moveToZero());
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                l_shoulder->Draw(prog, useMaterials);
                l_upper_arm->Draw(prog, useMaterials);

                // Left lower arm
                Model.pushMatrix();
                    Model.translate(-1.0f * l_elbow->moveToZero());
                    Model.rotate(glm::radians(120.0f), glm::vec3(1.0f, -0.7f, 0.0f));
                    Model.translate(l_elbow->moveToZero());
                    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                    l_elbow->Draw(prog, useMaterials);
                    l_forearm->Draw(prog, useMaterials);
                    
                    // Left hand
                    Model.pushMatrix();
                        Model.translate(-1.0f * l_wrist->moveToZero());
                        Model.rotate(glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                        Model.rotate(glm::radians(160.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                        Model.translate(l_wrist->moveToZero());
                        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                        l_wrist->Draw(prog, useMaterials);
                        l_hand->Draw(prog, useMaterials);
                    Model.popMatrix();
                Model.popMatrix();
            Model.popMatrix();
        
            // Right arm
            Model.pushMatrix();
                // Right upper arm
                Model.translate(-1.0f * r_shoulder->moveToZero());
                Model.rotate(glm::radians(65.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                Model.rotate(glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                Model.translate(r_shoulder->moveToZero());
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                r_shoulder->Draw(prog, useMaterials);
                r_upper_arm->Draw(prog, useMaterials);

                // Right lower arm
                Model.pushMatrix();
                    Model.translate(-1.0f * r_elbow->moveToZero());
                    
                    if (playGuitar)
                        Model.rotate(0.5f * sin(5.0f*glfwGetTime()) - 0.3f, glm::vec3(0.0f, 1.0f, 1.0f));
                    
                    Model.rotate(glm::radians(110.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    Model.translate(r_elbow->moveToZero());
                    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                    r_elbow->Draw(prog, useMaterials);
                    r_forearm->Draw(prog, useMaterials);
                    
                    // Right hand
                    Model.pushMatrix();
                        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
                        r_wrist->Draw(prog, useMaterials);
                        r_hand->Draw(prog, useMaterials);
                    Model.popMatrix();
                Model.popMatrix();
            Model.popMatrix();

             glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
            hip->Draw(prog, useMaterials);
            waist->Draw(prog, useMaterials);
            
            r_pelvic->Draw(prog, useMaterials);
            r_upper_leg->Draw(prog, useMaterials);
            r_knee->Draw(prog, useMaterials);
            r_lower_leg->Draw(prog, useMaterials);
            r_ankle->Draw(prog, useMaterials);
            r_foot->Draw(prog, useMaterials);

            l_pelvic->Draw(prog, useMaterials);
            l_upper_leg->Draw(prog, useMaterials);
            l_knee->Draw(prog, useMaterials);
            l_lower_leg->Draw(prog, useMaterials);
            l_ankle->Draw(prog, useMaterials);
            l_foot->Draw(prog, useMaterials);

        Model.popMatrix();

    Model.popMatrix();

    // Render guitar
    Model.pushMatrix();
        Model.translate(glm::vec3(0.15f, -0.01f, -0.25f));
        Model.scale(0.45f);
        Model.rotate(glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Model.rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Model.multMatrix(guitar->getNormalizedMat());
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model.topMatrix()));
        guitar->Draw(prog, useMaterials);
    Model.popMatrix();
}
