#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <memory>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Program.h"

using namespace std;

class Model
{
    public:
        Model(string path)
        {
            loadModel(path);
        }
        void Draw(const shared_ptr<Program> prog, bool drawMaterials=true) const;
        void normalize();
        glm::mat4 getNormalizedMat() { return M_o; }

    private:
        // Model data
        vector<Texture> textures_loaded;
        vector<Mesh> meshes;
        string directory;

        // Model metadata
        BoundingBox bb = {glm::vec3(INFINITY), glm::vec3(-INFINITY)};
        glm::mat4 M_o = glm::mat4(1.0f); // Composite matrix to scale+center model

        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             string typeName);
};

#endif