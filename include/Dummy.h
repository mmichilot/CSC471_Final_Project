#ifndef DUMMY_H
#define DUMMY_H

#include <memory>
#include <glm/glm.hpp>

#include "Model.h"
#include "Mesh.h"

using namespace std;

class Dummy
{
    public:
        shared_ptr<Model> model;
        shared_ptr<Model> guitar;

        // Meshes
        Mesh *head;
        Mesh *neck;
        Mesh *torso;
        Mesh *hip;
        Mesh *waist;
        
        Mesh *r_pelvic;
        Mesh *r_upper_leg;
        Mesh *r_knee;
        Mesh *r_lower_leg;
        Mesh *r_ankle;
        Mesh *r_foot;

        Mesh *r_shoulder;
        Mesh *r_upper_arm;
        Mesh *r_elbow;
        Mesh *r_forearm;
        Mesh *r_wrist;
        Mesh *r_hand;
        
        Mesh *l_pelvic;
        Mesh *l_upper_leg;
        Mesh *l_knee;
        Mesh *l_lower_leg;
        Mesh *l_ankle;
        Mesh *l_foot;

        Mesh *l_shoulder;
        Mesh *l_upper_arm;
        Mesh *l_elbow;
        Mesh *l_forearm;
        Mesh *l_wrist;
        Mesh *l_hand;

        // Dummy properities
        glm::vec3 guitaristPos = glm::vec3(-2.0f, 0.35f, -4.0f);
        BoundingBox guitaristBB = {
            glm::vec3(guitaristPos.x - 0.3f, 0.0f, guitaristPos.z - 0.3f),
            glm::vec3(guitaristPos.x + 0.3f, 0.0f, guitaristPos.z + 0.3f)
        };

        void init();
        void renderDummies(shared_ptr<Program> prog, bool playGuitar, bool useMaterials = true);
        void renderGuitarist(shared_ptr<Program> prog, bool playGuitar, bool useMaterials);
};

#endif