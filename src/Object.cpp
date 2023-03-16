#include "Object.h"
#include <iostream>
#include <assert.h>

using namespace std;
using namespace glm;

/* Helper Functions */

// Determine max value within a vec3
float max3(vec3 a) {
		return glm::max(glm::max(a.x, a.y), a.z);
}

// Computes composite matrix to center and scale object
// given its bounding box 
mat4 setupObject(BoundingBox bb) {
    vec3 extents = bb.max - bb.min;
    vec3 center = bb.min + 0.5f * extents;
    mat4 T_o = translate(mat4(1.0f), -1.0f * center);
    mat4 S_o = scale(mat4(1.0f), vec3(2/max3(extents)));
    
    return S_o * T_o;
}

/* Methods */

Object::Object(vector<tinyobj::shape_t> shapes) {
    // load object
    for (auto shape : shapes) {
        shared_ptr<Shape> s = make_shared<Shape>(false);
        s->createShape(shape);

        // Compute object's bounding box
        s->measure();
        bb.min = min(bb.min, s->min);
        bb.max = max(bb.max, s->max);

        s->init();
        this->shapes.push_back(s);  
    }

    M_o = setupObject(bb);
}

Object::Object(vector<tinyobj::shape_t> shapes, int index) {
    cout << "Loading shape: " << index << endl;
    assert (index < shapes.size() && index >= 0);
    
    // load single shape from object
    shared_ptr<Shape> s = make_shared<Shape>(false);
    s->createShape(shapes[index]);

    // Compute object's bounding box
    s->measure();
    bb.min = min(bb.min, s->min);
    bb.max = max(bb.max, s->max);

    s->init();
    this->shapes.push_back(s);  

    M_o = setupObject(bb);
}

Object::Object(const Object& obj) {
    for (auto shape : obj.shapes)
        shapes.push_back(shape);

    M_o = obj.M_o;
    bb = obj.bb;

    T_w = mat4(1.0f);
    R_w = mat4(1.0f);
    S_w = mat4(1.0f);

    M = mat4(1.0f);

    compFunc = [](mat4 T, mat4 R, mat4 P, mat4 S) -> mat4 {
        return T*R*P*S;
    };

}

void Object::setPivot(float x, float y, float z) {
    P = translate(mat4(1.0f), vec3(x, y, z));

    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setPosition(float x, float y, float z) {
    T_w = translate(mat4(1.0f), vec3(x, y, z));

    // Update composite matrix
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setRotation(float x, float y, float z) {
    mat4 rotX = rotate(mat4(1.0f), x * (float) (M_PI/180), vec3(1, 0, 0));
    mat4 rotY = rotate(mat4(1.0f), y * (float) (M_PI/180), vec3(0, 1, 0));
    mat4 rotZ = rotate(mat4(1.0f), z * (float) (M_PI/180), vec3(0, 0, 1));

    R_w = rotZ * rotY * rotX;
    
    // Update composite matrix
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setMultiRotation(bool x, bool y, bool z, float angle) {
    R_w = rotate(mat4(1.0f), angle * (float) (M_PI/180), vec3(x, y, z));
    
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setScale(float x, float y, float z) {
    S_w = scale(mat4(1.0f), vec3(x, y, z));

    // Update composite matrix
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setUniformScale(float sc) {
    S_w = scale(mat4(1.0f), vec3(sc));

    // Update composite matrix
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::setCompFunc(function<mat4(mat4 T, mat4 R, mat4 P, mat4 S)> func) {
    compFunc = func;

    // Update composite matrix
    M = compFunc(T_w, R_w, P, S_w) * M_o;
}

void Object::draw(const shared_ptr<Program> prog) const {
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M));
    for (auto shape : shapes)
        shape->draw(prog);
}