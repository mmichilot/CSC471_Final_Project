#ifndef LIGHTINGSYSTEM_H
#define LIGHTINGSYSTEM_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Light.h"
#include "Program.h"

using namespace std;

struct LightSource {
    unsigned int id;
    bool enabled;
    shared_ptr<Light> light;
};

class LightingSystem
{
    public:

        vector<LightSource> lights;

        unsigned int spawnDirectLight(glm::vec3 color, 
                                      glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f));
        
        unsigned int spawnPointLight(glm::vec3 color, 
                                     glm::vec3 position = glm::vec3(0.0f));
       
        unsigned int spawnSpotLight(glm::vec3 color, 
                                    float inner_cutoff,
                                    float outer_cutoff,
                                    glm::vec3 position = glm::vec3(0.0f),
                                    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f));

        void renderLights(shared_ptr<Program> prog, glm::mat4 view) const;
        
        void setPosition(unsigned int id, glm::vec3 position);
        void setDirection(unsigned int id, glm::vec3 direction);
        void setColor(unsigned int id, glm::vec3 color);
    
        glm::vec3 getPosition(unsigned int id);
        glm::vec3 getDirection(unsigned int id);
        glm::vec3 getColor(unsigned int id);
        glm::mat4 getSpaceMatrix(unsigned int id, float aspect);
        
    private:
        shared_ptr<Light> search(unsigned int id);
};

#endif