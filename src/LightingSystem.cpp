#include <iostream>
#include <glm/glm.hpp>

#include "Light.h"
#include "LightingSystem.h"

using namespace std;


/*
 * Helper Functions
 */
shared_ptr<Light> LightingSystem::search(unsigned int id)
{
    for (auto light : lights) {
        if (light.id == id)
            return light.light;
    }

    cerr << "[LightingSystem] ID " << id << " not found!" << endl;
    return NULL;
}

/*
 *  Spawning Functions
 */
unsigned int LightingSystem::spawnDirectLight(glm::vec3 color, glm::vec3 direction)
{
    LightSource light;
    
    light.id = lights.size();
    light.enabled = true;
    light.light = make_shared<DirectLight>(color, direction);

    lights.push_back(light);
    return light.id;
}

unsigned int LightingSystem::spawnPointLight(glm::vec3 color, glm::vec3 position)
{
    LightSource light;
    
    light.id = lights.size();
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
    auto light = search(id);
    
    if (light == NULL) return;

    if (auto pointLight = dynamic_pointer_cast<PointLight>(light))
        pointLight->setPosition(position);
    if (auto spotLight = dynamic_pointer_cast<SpotLight>(light))
        spotLight->setPosition(position);
}

void LightingSystem::setDirection(unsigned int id, glm::vec3 direction)
{
    auto light = search(id);

    if (light == NULL) return;

    if (auto directLight = dynamic_pointer_cast<DirectLight>(light))
        directLight->setDirection(direction);
    if (auto spotLight = dynamic_pointer_cast<SpotLight>(light))
        spotLight->setDirection(direction);
}

void LightingSystem::setColor(unsigned int id, glm::vec3 color)
{
    auto light = search(id);

    if (light == NULL) return;

    light->setColor(color);
}

/* 
 * Getter Functions 
 */
glm::vec3 LightingSystem::getPosition(unsigned int id)
{
    auto light = search(id);

    if (light == NULL) return glm::vec3(0.0f);
    
    if (auto pointLight = dynamic_pointer_cast<PointLight>(light))
        return pointLight->getPosition();
    if (auto spotLight = dynamic_pointer_cast<SpotLight>(light))
        return spotLight->getPosition();
    
    return glm::vec3(0.0f);
}

glm::vec3 LightingSystem::getDirection(unsigned int id)
{
    auto light = search(id);

    if (light == NULL) return glm::vec3(0.0f);
    
    if (auto directLight = dynamic_pointer_cast<DirectLight>(light))
        return directLight->getDirection();
    if (auto spotLight = dynamic_pointer_cast<SpotLight>(light))
        return spotLight->getDirection();

    return glm::vec3(0.0f);
}

glm::mat4 LightingSystem::getSpaceMatrix(unsigned int id, float aspect)
{
    auto light = search(id);

    if (light == NULL) return glm::mat4(0.0f);

    if (auto spotLight = dynamic_pointer_cast<SpotLight>(light))
        return spotLight->getProjectionMatrix(aspect) * spotLight->getViewMatrix();

    return glm::mat4(0.0f);
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
            glUniform1i(prog->getUniform(light + ".valid"), GL_TRUE);

            // Render light color
            lights[i].light->activate(i, prog);

            if (auto directLight = dynamic_pointer_cast<DirectLight>(lights[i].light)) {
                glUniform1i(prog->getUniform(light + ".type"), 0);
                directLight->activate(i, prog, view);
            }

            if (auto pointLight = dynamic_pointer_cast<PointLight>(lights[i].light)) {
                glUniform1i(prog->getUniform(light + ".type"), 1);
                pointLight->activate(i, prog, view);
            }

            if (auto spotLight = dynamic_pointer_cast<SpotLight>(lights[i].light)) {
                glUniform1i(prog->getUniform(light + ".type"), 2);
                spotLight->activate(i, prog, view);
            }
        }
    }
}