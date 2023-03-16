#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <glad/glad.h>
#include <functional>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

typedef struct {
    vec3 min;
    vec3 max;
} BoundingBox;

typedef struct {
    mat4 x;
    mat4 y;
    mat4 z;
} Rotations;

class Object
{
public:
    // Load all shapes within a multi-shape object
    Object(vector<tinyobj::shape_t> shapes);
    // Load a given shape within a multi-shape object
    Object(vector<tinyobj::shape_t> shapes, int index);
    
    // Copy Constructors
    Object(const Object& obj);

    void setPivot(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);
    void setMultiRotation(bool x, bool y, bool z, float angle);
    void setScale(float x, float y, float z);
    void setUniformScale(float sc);
    void setCompFunc(function<mat4(mat4 T, mat4 R, mat4 P, mat4 S)> func);

    void draw(const shared_ptr<Program> prog) const;

private:
    vector<shared_ptr<Shape>> shapes;

    // Matrix to scale and center object
    mat4 M_o;

    // Matrices to translate, rotate, and scale object
    mat4 T_w = mat4(1.0f);
    mat4 R_w = mat4(1.0f);
    mat4 S_w = mat4(1.0f);

    mat4 P = mat4(1.0f);

    // Composite matrix for all operations
    mat4 M = mat4(1.0f);

    BoundingBox bb = {vec3(INFINITY), vec3(-INFINITY)};

    // Function handle for composite matrix computation
    function<mat4(mat4 T, mat4 R, mat4 P, mat4 S)> compFunc = [](mat4 T, mat4 R, mat4 P, mat4 S) -> mat4 {
        return T*R*P*S;
    };
};

#endif