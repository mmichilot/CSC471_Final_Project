#ifndef LIGHT_H
#define LIGHT_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Program.h"

using namespace std;

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

class Light
{
    public:
        Light(glm::vec3 color) : color(color) {};

        /* Setters */
        void setColor(glm::vec3 color) { this->color = color; }

        /* Getters */
        glm::vec3 getColor() { return color; }

        /* Setup light for rendering */
        virtual void activate(unsigned int i, const shared_ptr<Program> prog) const;

    private:
        /* Properties */
        glm::vec3 color = glm::vec3(1.0f);
};


class DirectLight : public Light
{
    public:
        DirectLight(glm::vec3 color, glm::vec3 direction)
        : Light(color), direction(direction) {};

        /* Setters */
        void setDirection(glm::vec3 direction) { this->direction = direction; }

        /* Getters */
        glm::vec3 getDirection() { return direction; }

        /* Setup light for rendering */
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;

    private:
        /* Properties */
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
};  


class PointLight : public Light
{
    public:
        PointLight(glm::vec3 color, glm::vec3 position)
        : Light(color), position(position) {};

        /* Setters */
        void setPosition(glm::vec3 position) { this->position = position; }
        void setAttenuation(float constant, float linear, float quadratic)
        {
            attenuation.constant  = constant;
            attenuation.linear    = linear;
            attenuation.quadratic = quadratic;
        }

        /* Getters */
        glm::vec3 getPosition() { return position; }

        /* Setup light for rendering */
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;

    private:
        /* Properties */
        glm::vec3 position = glm::vec3(0.0f);
        Attenuation attenuation = {1.0f, 0.045f, 0.0075f};
};


class SpotLight : public Light
{
    public:
        SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, 
                  float inner, float outer)
        : Light(color), position(position), direction(direction), 
        inner_cutoff(inner), outer_cutoff(outer) 
        {
            front = glm::normalize(direction);
            right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
            up    = glm::normalize(glm::cross(right, front));
        };

        /* Setters */
        void setPosition(glm::vec3 position) { this->position = position; }
        
        void setDirection(glm::vec3 direction) {
            front = glm::normalize(direction);
            right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
            up    = glm::normalize(glm::cross(right, front)); 
            this->direction = direction; 
        }
        
        void setAttenuation(float constant, float linear, float quadratic)
        {
            attenuation.constant  = constant;
            attenuation.linear    = linear;
            attenuation.quadratic = quadratic;
        }
        
        void setCutoffs(float inner, float outer)
        {
            inner_cutoff = inner;
            outer_cutoff = outer;
        }

        /* Getters */
        glm::vec3 getPosition() { return position; }
        glm::vec3 getDirection() { return direction; }
        
        glm::mat4 getProjectionMatrix(float aspect) { 
            return glm::perspective(45.0f, aspect, 0.01f, 100.0f);
        }
        
        glm::mat4 getViewMatrix() {
            return glm::lookAt(position, position + front, up);
        }

        /* Setup light for rendering */
        void activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const;
    
    private:
        /* Properties */
        glm::vec3 position  = glm::vec3(0.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
        Attenuation attenuation = {1.0f, 0.045f, 0.0075f}; 
        float inner_cutoff = 0;
        float outer_cutoff = 0;
        
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;
};

#endif