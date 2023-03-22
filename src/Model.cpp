#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "common.h"
#include "Model.h"
#include "Program.h"

using namespace std;

void Model::Draw(const shared_ptr<Program> prog, bool drawMaterials) const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(prog, drawMaterials);
}

void Model::loadModel(string path)
{
    cout << "\nLoading Model: " << path << endl;
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material mat = {glm::vec3(0.0f), glm::vec3(0.0f), 0.0f};

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        // Process vertices
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Process normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        // Process texture coordinates (if available)
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // Process faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        cout << "Loading Materials..." << endl; 
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        
        // Load diffuse texture map, if available
        if (material->GetTextureCount(aiTextureType_DIFFUSE) >= 1)
        {
            cout << "Loading Diffuse Texture Map(s)..." << endl;
            vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                            aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }
        else 
        {
            // Load basic diffuse material
            cout << "Loading basic diffuse material..." << endl;
            aiColor3D color(0.f, 0.f, 0.f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            mat.diffuse.r = color.r;
            mat.diffuse.g = color.g;
            mat.diffuse.b = color.b;
            cout << "[Diffuse] r: " << color.r << " g: " << color.g << " b: " << color.b << endl;
        }

        // Load specular texture map, if available
        if (material->GetTextureCount(aiTextureType_SPECULAR) >= 1) {
            cout << "Loading Specular Texture Map(s)..." << endl;
            vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        else
        {
            // Load basic specular material
            cout << "Loading basic specular material..." << endl;
            aiColor3D color(0.f, 0.f, 0.f);
            material->Get(AI_MATKEY_COLOR_SPECULAR, color);
            mat.specular.r = color.r;
            mat.specular.g = color.g;
            mat.specular.b = color.b;
            cout << "[Specular] r: " << color.r << " g: " << color.g << " b: " << color.b << endl;
        }

        // Load material shininess
        material->Get(AI_MATKEY_SHININESS, mat.shininess);
        cout << "[Shininess] f: " << mat.shininess << endl;
    }

    return Mesh(vertices, indices, textures, mat);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        // Check if texture has already been loaded
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

// Determine max value within a vec3
float max3(glm::vec3 a) {
		return (glm::max)((glm::max)(a.x, a.y), a.z);
}

void Model::normalize()
{
    // Determine bounding box for entire model
    for (auto &mesh : meshes)
    {
        mesh.measure();
        bb.min = (glm::min)(mesh.bb.min, bb.min);
        bb.max = (glm::max)(mesh.bb.max, bb.max);
    }

    // Compute composite matrix
    glm::vec3 extents = bb.max - bb.min;
    glm::vec3 center = bb.min + (0.5f * extents);
    glm::mat4 T_o = glm::translate(glm::mat4(1.0f), -1.0f * center);
    glm::mat4 S_o = glm::scale(glm::mat4(1.0f), glm::vec3(2/max3(extents)));

    M_o = S_o * T_o;

    // Update the bounding box
    bb.min = glm::vec3(M_o * glm::vec4(bb.min, 1.0f));
    bb.max = glm::vec3(M_o * glm::vec4(bb.max, 1.0f));
}

void Model::updateBoundingBox(glm::mat4 M)
{   
    glm::mat4 M_tot = T_w * R_w * S_w * M_o;
    
    bb.min = glm::vec3(INFINITY);
    bb.max = glm::vec3(-INFINITY);

    for (auto &mesh : meshes)
    {
        mesh.measure(M_tot);
        bb.min = (glm::min)(mesh.bb.min, bb.min);
        bb.max = (glm::max)(mesh.bb.max, bb.max);
    }
}