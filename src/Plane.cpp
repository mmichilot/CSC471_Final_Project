#include "Plane.h"
#include <iostream>

void Plane::init()
{
    float vertices[] = {
        // positions         	      // normals       	 // texture coords
        -groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,       0.0f,  groundTex,
         groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,  groundTex,  groundTex,
         groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,  groundTex,       0.0f,
         groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,  groundTex,       0.0f,
        -groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,       0.0f,       0.0f,
        -groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,       0.0f,  groundTex,
    };

    // Generate ground VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Send vertices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup attribute pointers
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Plane::render(shared_ptr<Program> prog, bool useMaterials, int texture)
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
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
