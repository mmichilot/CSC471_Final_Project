#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>
#include <memory>

#include "Program.h"

using namespace std;

class Plane
{
    public:
        float groundX = 5.0f;
        float groundZ = 5.0f;
        float groundTex = 5.0f;

        unsigned int VBO;
        unsigned int VAO;

        glm::mat4 M = glm::mat4(1.0f);
        
        void init();
        void render(shared_ptr<Program> prog, bool useMaterials, int texture = -1);
};

#endif