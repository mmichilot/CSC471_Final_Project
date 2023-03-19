#include <iostream>
#include <glm/glm.hpp>

#include "Light.h"
#include "LightingSystem.h"

using namespace std;


/*
 * Helper Functions
 */
LightSource* LightingSystem::search(unsigned int id)
{
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i].id == id)
            return &lights[i];
    }

    cerr << "[LightingSystem] ID " << id << " not found!" <<endl;
    return NULL;
}

/*
 *  Spawning Functions
 */
unsigned int LightingSystem::spawnDirectLight(glm::vec3 color, glm::vec3 direction)
{
    LightSource light;
    
    light.id = lights.size();
    light.type = DIRECT_LIGHT;
    light.enabled = true;
    light.light = make_shared<DirectLight>(color, direction);

    lights.push_back(light);
    return light.id;
}

unsigned int LightingSystem::spawnPointLight(glm::vec3 color, glm::vec3 position)
{
    LightSource light;
    
    light.id = lights.size();
    light.type = POINT_LIGHT;
    light.enabled = true;
    light.light = make_shared<PointLight>(color, position);

    lights.push_back(light);
    return light.id;
}

unsigned int LightingSystem::spawnSpotLight(glm::vec3 color, float inner_cutoff,
    float outer_cutoff, glm::vec3 position, glm::vec3 direction)
{
    LightSource light;
    
    light.id = lights.size();
    light.type = SPOT_LIGHT;
    light.enabled = true;
    light.light = make_shared<SpotLight>(color, position, direction, 
        inner_cutoff, outer_cutoff);

    lights.push_back(light);
    return light.id;
}

/*
 * Setter Functions
 */
void LightingSystem::setPosition(unsigned int id, glm::vec3 position)
{
    
    LightSource *l = search(id);
    
    if (l == NULL)
        return;

    if (l->type == DIRECT_LIGHT) {
        cerr << "Directional light doesn't have a position!" << endl;
        return;
    }

    dynamic_pointer_cast<PointLight>(l->light)->setPosition(position);
}

void LightingSystem::setDirection(unsigned int id, glm::vec3 direction)
{
    LightSource *l = search(id);

    if (l == NULL)
        return;

    if (l->type == POINT_LIGHT) {
        cerr << "Point light doesn't have a direction!" << endl;
        return;
    }

    dynamic_pointer_cast<DirectLight>(l->light)->setDirection(direction);
}

/* 
 * Getter Functions 
 */
glm::vec3 LightingSystem::getPosition(unsigned int id)
{
    LightSource *l = search(id);

    if (l == NULL)
        return glm::vec3(0.0f);
    
    if (l->type == DIRECT_LIGHT) {
        cerr << "Directional light doesn't have a position!" << endl;
        return glm::vec3(0.0f);
    }

    return dynamic_pointer_cast<PointLight>(l->light)->position;
}

glm::vec3 LightingSystem::getDirection(unsigned int id)
{
    LightSource *l = search(id);

    if (l == NULL)
        return glm::vec3(0.0f);
    
    if (l->type == POINT_LIGHT) {
        cerr << "Point light doesn't have a direction!" << endl;
        return glm::vec3(0.0f);
    }

    return dynamic_pointer_cast<DirectLight>(l->light)->direction;
}

/*
 * Rendering
 */
void LightingSystem::renderLights(shared_ptr<Program> prog, glm::mat4 view) const
{
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        string light = ("light[" + to_string(i) + "]");
        glUniform1i(prog->getUniform(light + ".valid"), GL_FALSE);


        if (lights[i].enabled) {

            // Set light source type
            glUniform1i(prog->getUniform(light + ".valid"), GL_TRUE);
            glUniform1i(prog->getUniform(light + ".type"), lights[i].type);
            
            // Render light color
            lights[i].light->activate(i, prog);

            // Renderlight specific properties
            if (lights[i].type == DIRECT_LIGHT)
                dynamic_pointer_cast<DirectLight>(lights[i].light)->activate(i, prog, view);
            else if (lights[i].type == POINT_LIGHT)
                dynamic_pointer_cast<PointLight>(lights[i].light)->activate(i, prog, view);
            else if (lights[i].type == SPOT_LIGHT)
                dynamic_pointer_cast<SpotLight>(lights[i].light)->activate(i, prog, view);

        }
    }
}