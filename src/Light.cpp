#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"
#include "Program.h"

using namespace std;

void Light::activate(unsigned int i, const shared_ptr<Program> prog) const
{
    // Compute light colors
    glm::vec3 ambient = color * glm::vec3(0.5f);
    glm::vec3 diffuse = color * glm::vec3(1.0f);
    
    string light = ("light[" + to_string(i) + "]");
    glUniform3fv(prog->getUniform(light + ".ambient"), 1, value_ptr(ambient));
    glUniform3fv(prog->getUniform(light + ".diffuse"), 1, value_ptr(diffuse));
    glUniform3f(prog->getUniform(light + ".specular"), 1.0f, 1.0f, 1.0f);
    
}

void DirectLight::activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const
{
    // Need to compute light direction from view to world
    // Remember that the view matrix changes the coordinate system, warping vectors, such
    // as light direction. Need to "unwarp" it, to get the correct light direction in the 
    // world space.
    glm::vec3 w_direction = glm::mat3(glm::transpose(glm::inverse(view))) * direction;

    string light = ("light[" + to_string(i) + "]");
    glUniform3fv(prog->getUniform(light + ".direction"), 1, value_ptr(w_direction));
}

void PointLight::activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const
{
    // Need to compute light position from world to view
    glm::vec3 v_position = glm::vec3(view * glm::vec4(position, 1.0f));

    string light = ("light[" + to_string(i) + "]");
    glUniform3fv(prog->getUniform(light + ".position"), 1, value_ptr(v_position));

    // Apply attenuation
    glUniform1f(prog->getUniform(light + ".constant"),  attenuation.constant);
    glUniform1f(prog->getUniform(light + ".linear"),    attenuation.linear);
    glUniform1f(prog->getUniform(light + ".quadratic"), attenuation.quadratic);
}

void SpotLight::activate(unsigned int i, const shared_ptr<Program> prog, glm::mat4 view) const
{
    PointLight::activate(i, prog, view);
    DirectLight::activate(i, prog, view);
    
    // Apply light cutoffs
    float inner = glm::cos(glm::radians(inner_cutoff));
    float outer = glm::cos(glm::radians(outer_cutoff));

    string light = ("light[" + to_string(i) + "]");
    glUniform1f(prog->getUniform(light + ".inner_cutoff"), inner);
    glUniform1f(prog->getUniform(light + ".outer_cutoff"), outer);
}