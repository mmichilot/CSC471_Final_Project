#ifndef STAGE_H
#define STAGE_H

#include <glm/glm.hpp>
#include "Model.h"
#include "Program.h"
#include "Plane.h"

using namespace std;

class Stage
{
    public:
        // Stage size
        glm::vec3 center;
        float width;
        float depth;
        float height;

        // Stage models
        shared_ptr<Model> truss;

        // Stage textures
        unsigned int stage_texture;

        // Stage planes
        Plane ground;
        Plane back_wall;

        // Vertical stage trusses
        vector<glm::mat4> trusses_M;

        Stage(glm::vec3 center, float width, float depth, float height) :
        center(center), width(width), depth(depth), height(height) {};
        

        void initStage();
        void renderStage(shared_ptr<Program> prog, bool useMaterials = true);
};

#endif