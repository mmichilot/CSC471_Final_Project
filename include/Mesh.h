#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

#include "Program.h" 

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

class Mesh {
    public:
        // Mesh data
        vector<Vertex>       vertices;
        vector<unsigned int> indices;
        vector<Texture>      textures;
        Material             material;

        // Mesh metadata
        BoundingBox bb = {glm::vec3(INFINITY), glm::vec3(-INFINITY)};

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material);
        void Draw(const shared_ptr<Program> prog, bool drawMaterials=true) const;
        void measure();
        void measure(glm::mat4 M);

    private:
        // Render data
        unsigned int VAO, VBO, EBO;

        void setupMesh();
        void setMaterials(const shared_ptr<Program> prog) const;
};

#endif