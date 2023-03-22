#include <glm/gtc/type_ptr.hpp>

#include "Stage.h"
#include "MatrixStack.h"

// Save computation time by setting up the stage beforehand
void Stage::initStage()
{   
    MatrixStack Model;
    Model.translate(center);
    
    /* Stage planes */
    ground.init();
    back_wall.init();
    
    // Ground
    Model.pushMatrix();
        Model.translate(glm::vec3(0.0f, -1.0f, 0.0f));
        Model.scale(glm::vec3(width/10.0f + 0.1f, 1.0f, depth/10.0f + 0.1f));
        ground.M = Model.topMatrix();
    Model.popMatrix();

    // Back Wall
    Model.pushMatrix();
        Model.translate(glm::vec3(0.0f, -0.2f, depth/2.0f));
        Model.rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Model.scale(glm::vec3(width/10.0f-0.05f, 1.0f, 1.0f));
        back_wall.M = Model.topMatrix();
    Model.popMatrix();


    /* Stage trusses */
    float trussHeight = truss->bb.max.y - truss->bb.min.y;

    // Truss front right
    Model.pushMatrix();
        Model.translate(glm::vec3(width/2.0f, 0.0f, -depth/2.0f));
        for (int i = 0; i < 3; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss front left
    Model.pushMatrix();
        Model.translate(glm::vec3(-width/2.0f, 0.0f, -depth/2.0f));
        for (int i = 0; i < 3; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss back left
    Model.pushMatrix();
        Model.translate(glm::vec3(-width/2.0f, 0.0f, depth/2.0f));
        for (int i = 0; i < 3; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss back right
    Model.pushMatrix();
        Model.translate(glm::vec3(width/2.0f, 0.0f, depth/2.0f));
        for (int i = 0; i < 3; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss front top
    Model.pushMatrix();
        Model.translate(glm::vec3(width/2.0f, 5.0f, -depth/2.0f));
        Model.rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        for (int i = 0; i < width/trussHeight; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight + 1.0f, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss back top
    Model.pushMatrix();
        Model.translate(glm::vec3(width/2.0f, 5.0f, depth/2.0f));
        Model.rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        for (int i = 0; i < width/trussHeight; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight + 1.0f, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss right top
    Model.pushMatrix();
        Model.translate(glm::vec3(width/2.0f, 5.0f, depth/2.0f));
        Model.rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (int i = 0; i < depth/trussHeight; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight + 0.2f, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

    // Truss left top
    Model.pushMatrix();
        Model.translate(glm::vec3(-width/2.0f, 5.0f, depth/2.0f));
        Model.rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (int i = 0; i < depth/trussHeight; i++) {
            Model.pushMatrix();
                Model.translate(glm::vec3(0.0f, i*trussHeight + 0.2f, 0.0f));
                Model.multMatrix(truss->getTransformMat());
                trusses_M.push_back(Model.topMatrix());
            Model.popMatrix();
        }
    Model.popMatrix();

}

void Stage::renderStage(shared_ptr<Program> prog, bool useMaterials)
{
    /* Render planes */
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(ground.M));
    ground.render(prog, useMaterials, stage_texture);

    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(back_wall.M));
    back_wall.render(prog, useMaterials);

    /* Render trusses */
    for (auto M : trusses_M) {
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M));
        truss->Draw(prog, useMaterials);
    }
}