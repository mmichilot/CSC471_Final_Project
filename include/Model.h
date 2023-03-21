#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Program.h"

using namespace std;

class Model
{
    public:
        // Model data
        vector<Texture> textures_loaded;
        vector<Mesh> meshes;
        string directory;

        // Model metadata
        BoundingBox bb = {glm::vec3(INFINITY), glm::vec3(-INFINITY)};
        glm::mat4 M_o = glm::mat4(1.0f); // Composite matrix to scale+center model

        // Model transformations
        glm::mat4 T_w = glm::mat4(1.0f);
        glm::mat4 R_w = glm::mat4(1.0f);
        glm::mat4 S_w = glm::mat4(1.0f);

        Model(string path)
        {
            loadModel(path);
        }
        void Draw(const shared_ptr<Program> prog, bool drawMaterials=true) const;
        void normalize();

        // Model transformations
        void translate(glm::vec3 translate) { T_w = glm::translate(glm::mat4(1.0f), translate); }
        void rotate(glm::mat4 rotate) { R_w = rotate; };
        void scale(float scale) { S_w = glm::scale(glm::mat4(1.0f), glm::vec3(scale)); }
        void scale(glm::vec3 scale) { S_w = glm::scale(glm::mat4(1.0f), scale); }
        
        glm::mat4 getNormalizedMat() { return M_o; }
        glm::mat4 getTransformMat() { return T_w * R_w * S_w * M_o; }
        void updateBoundingBox(glm::mat4 M = glm::mat4(1.0f));

    private:
        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             string typeName);
};

#endif