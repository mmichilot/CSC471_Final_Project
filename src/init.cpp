#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "Application.h"
#include "GLSL.h"
#include "common.h"

using namespace std;

void initPlane(unsigned int *VAO, unsigned int *VBO);

void Application::init()
{
    GLSL::checkVersion();

    // Set background color.
    glClearColor(.12f, .34f, .56f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);

    // Enable alpha transparency
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Application::initShaders(const string shaderDirectory)
{
    // Initialize the GLSL programs

    prog = make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(shaderDirectory + "/vert.glsl", shaderDirectory + "/frag.glsl");
    prog->init();
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("shadowMaps");

    
    for (unsigned int i = 0; i < 10; i++) {
        string index = ("[" + to_string(i) + "]");
        prog->addUniform("light" + index + ".valid");
        prog->addUniform("light" + index + ".type");
        prog->addUniform("light" + index + ".position");
        prog->addUniform("light" + index + ".direction");
        prog->addUniform("light" + index + ".inner_cutoff");
        prog->addUniform("light" + index + ".outer_cutoff");
        prog->addUniform("light" + index + ".ambient");
        prog->addUniform("light" + index + ".diffuse");
        prog->addUniform("light" + index + ".specular");
        prog->addUniform("light" + index + ".constant");
        prog->addUniform("light" + index + ".linear");
        prog->addUniform("light" + index + ".quadratic");
        prog->addUniform("lightSpaceMatrix" + index);
    }

    prog->addUniform("material.diffuse");
    prog->addUniform("material.specular");
    prog->addUniform("material.texture_diffuse_enable");
    prog->addUniform("material.texture_diffuse1");
    prog->addUniform("material.texture_specular_enable");
    prog->addUniform("material.texture_specular1");
    prog->addUniform("material.shininess");
    
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");

    skyProg = make_shared<Program>();
    skyProg->setVerbose(true);
    skyProg->setShaderNames(shaderDirectory + "/sky_vert.glsl", shaderDirectory + "/sky_frag.glsl");
    skyProg->init();
    skyProg->addUniform("P");
    skyProg->addUniform("M");
    skyProg->addUniform("V");
    skyProg->addUniform("tex");
    skyProg->addAttribute("vertPos");
    skyProg->addAttribute("vertTex");

    shadowProg = make_shared<Program>();
    shadowProg->setVerbose(true);
    shadowProg->setShaderNames(shaderDirectory + "/shadow_vert.glsl", shaderDirectory + "/shadow_frag.glsl");
    shadowProg->init();
    shadowProg->addUniform("M");
    shadowProg->addUniform("lightSpaceMatrix");
    shadowProg->addAttribute("vertPos");
}

void Application::initGeometry(const string objectDirectory)
{	
    // Import models
    skysphere = make_shared<Model>((objectDirectory + "/skysphere.obj"));
    skysphere->normalize();
    
    drum_set = make_shared<Model>((objectDirectory + "/drum_set/drum_set.obj"));
    drum_set->normalize();

    // amp = make_shared<Model>(objectDirectory + "/amp/GuitarAmp.x");
    // amp->normalize();

    // Setup user-created objects
    initPlane(&planeVAO, &planeVBO);

    // Setup initial transforms
    drum_set->translate(glm::vec3(0.0f, -0.4f, -2.0f));
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(220.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    drum_set->rotate(rotation);
    drum_set->updateBoundingBox();
}

void Application::initTextures(const string textureDirectory)
{
    // Load textures

    string texFile = "/nightSky.png";
    skysphere_texture = TextureFromFile(texFile.c_str(), textureDirectory);
    glBindTexture(GL_TEXTURE_2D, skysphere_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    texFile = "/stage_floor.jpg";
    stage_texture = TextureFromFile(texFile.c_str(), textureDirectory);
    glBindTexture(GL_TEXTURE_2D, stage_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
}

void initPlane(unsigned int *VAO, unsigned int *VBO)
{
    float groundX = 5.0f;
    float groundZ = 5.0f;
    float groundTex = 5.0f;

    float vertices[] = {
        // positions         	      // normals       	 // texture coords
        -groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,       0.0f,  groundTex,
         groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,  groundTex,  groundTex,
         groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,  groundTex,       0.0f,
         groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,  groundTex,       0.0f,
        -groundX, 0.0f,  groundZ,  0.0f, 1.0f,  0.0f,       0.0f,       0.0f,
        -groundX, 0.0f, -groundZ,  0.0f, 1.0f,  0.0f,       0.0f,  groundTex,
    };

    // Generate ground VAO and VBO
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    // Send vertices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup attribute pointers
    glBindVertexArray(*VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Application::initLights()
{
    // Setup lights
    unsigned int light0 = lightingSystem.spawnSpotLight(glm::vec3(1.0f, 0.0f, 0.0f), 20, 25, 
        glm::vec3(-4.0f, 0.0f, -6.0f), glm::vec3(0.7f, 0.0f, 1.0f));
    stageLights.push_back(light0);

    unsigned int light1 = lightingSystem.spawnSpotLight(glm::vec3(0.0f, 1.0f, 0.0f), 20, 25, 
        glm::vec3(4.0f, 0.0f, -6.0f), glm::vec3(-0.7f, 0.0f, 1.0f));
    stageLights.push_back(light1);

    // unsigned int light2 = lightingSystem.spawnSpotLight(glm::vec3(0.0f, 0.0f, 1.0f), 20, 25, 
    //     glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // stageLights.push_back(light2);

    // unsigned int light3 = lightingSystem.spawnSpotLight(glm::vec3(0.5f), 20, 25, 
    //     glm::vec3(0.0f, 5.0f, -6.0f), glm::vec3(0.0f, -1.0f, 1.0f));
    // stageLights.push_back(light3);
}

void Application::initShadows()
{
    // Setup an array of shadow maps
    glGenTextures(1, &shadowMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 10, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);

    glGenFramebuffers(10, &shadowFBO[0]);

    // Attach each shadow map layer to an FBO
    for (unsigned int i = 0; i < 10; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps, 0, i);
    }

}

void Application::initAudio(const string audioDirectory)
{   
    unsigned int buffer, source;
    
    // Setup snare
    buffer = audioSystem.loadFile(audioDirectory + "/snare.wav");
    source = audioSystem.createSource(0.0f, 0.0f, 0.0f);
    audioSystem.bind(source, buffer);
    snare.source_id = source;

    // Setup hi-hat
    buffer = audioSystem.loadFile(audioDirectory + "/hi_hat.wav");
    source = audioSystem.createSource(0.0f, 0.0f, 0.0f);
    audioSystem.bind(source, buffer);
    hi_hat.source_id = source;

    // Setup kick
    buffer = audioSystem.loadFile(audioDirectory + "/kick.wav");
    source = audioSystem.createSource(0.0f, 0.0f, 0.0f);
    audioSystem.bind(source, buffer);
    kick.source_id = source;
}

void Application::initCameras()
{
    camera.Position = glm::vec3(0.0f, playerHeight, 0.0f);
    drumCam.Position = glm::vec3(0.0f, 0.4f, -1.5f);
    
    stageCam.Position = glm::vec3(0.0f, stageHeight/2.0f, stageCenter.z - stageDepth/2.0f);
    stageCam.Front    = glm::vec3(0.0f, 0.0f, 1.0f);
    stageCam.Yaw      = 90.0f;
}