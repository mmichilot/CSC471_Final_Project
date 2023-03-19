#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->material = material;

    setupMesh();
}

void Mesh::setupMesh()
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Setup array of structs (AoS)
    // Each Vertex struct has positions, normals, and texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Setup element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // Setup attribute pointers to point to their appropriate item within the AoS
    
    glEnableVertexAttribArray(0);   // vertex positions at location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);   // vertex normals at location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);   // vertex texture coords at location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::setMaterials(const shared_ptr<Program> prog) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    
    // By default, disable textures and use basic object material
    glUniform1i(prog->getUniform("material.texture_diffuse_enable"), 0);
    glUniform1i(prog->getUniform("material.texture_specular_enable"), 0);
    glUniform3fv(prog->getUniform("material.diffuse"), 1, value_ptr(material.diffuse));
    glUniform3fv(prog->getUniform("material.specular"), 1, value_ptr(material.specular));
    glUniform1f(prog->getUniform("material.shininess"), material.shininess);
    
    // If the mesh has diffuse and specular maps, use them instead
    if (textures.size() > 0) {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;

            if (name == "texture_diffuse") {
                glUniform1i(prog->getUniform("material.texture_diffuse_enable"), 1);
                number = to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                glUniform1i(prog->getUniform("material.texture_specular_enable"), 1);
                number = to_string(specularNr++);
            }
            
            glUniform1i(prog->getUniform(("material." + name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
    }
}

void Mesh::Draw(const shared_ptr<Program> prog, bool drawMaterials) const 
{
    // Draw mesh w/ its given materials, if desired
    if (drawMaterials)
        setMaterials(prog);
    
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::measure()
{
    // Determine bounding box for mesh
    for (auto &vertex : vertices)
    {
        bb.min = (glm::min)(vertex.Position, bb.min);
        bb.max = (glm::max)(vertex.Position, bb.max);
    }
}
