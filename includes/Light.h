#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <memory>

#include "Program.h"

using namespace std;

enum LightType {
    DIRECT_LIGHT = 0,
    POINT_LIGHT,
    SPOT_LIGHT
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

class Light
{
    public:
        // Basic light properties
        glm::vec3 color = glm::vec3(1.0f);

        Light(glm::vec3 color) : color(color) {};

        void setColor(glm::vec3 color) { this->color = color; }
        virtual void activate(unsigned int i, const shared_ptr<Program> prog) const;

};

class DirectLight : virtual public Light
{
    public:
        // Directional light properties
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

        DirectLight(glm::vec3 color, glm::vec3 direction)
        : Light(color), direction(direction) {};

        void setDirection(glm::vec3 direction) { this->direction = direction; }
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;
};

class PointLight : virtual public Light
{
    public:
        // Point light properties
        glm::vec3 position = glm::vec3(0.0f);
        Attenuation attenuation = {1.0f, 0.045f, 0.0075f};

        PointLight(glm::vec3 color, glm::vec3 position)
        : Light(color), position(position) {};

        void setPosition(glm::vec3 position) { this->position = position; }
        void setAttenuation(float constant, float linear, float quadratic)
        {
            attenuation.constant  = constant;
            attenuation.linear    = linear;
            attenuation.quadratic = quadratic;
        }
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;
};

class SpotLight : public PointLight, public DirectLight
{
    public:
        // Spotlight properties
        float inner_cutoff = 0;
        float outer_cutoff = 0;

        SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, 
                  float inner, float outer)
        : Light(color), PointLight(color, position), DirectLight(color, direction), 
          inner_cutoff(inner), outer_cutoff(outer) {};

        void setCutoffs(float inner, float outer)
        {
            inner_cutoff = inner;
            outer_cutoff = outer;
        }
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;
};

#endif