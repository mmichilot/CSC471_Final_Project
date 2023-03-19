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
    prog->addUniform("lightSpaceMatrix");
    prog->addUniform("shadowMap");
    
    for (unsigned int i = 0; i < 10; i++) {
        string light = ("light[" + to_string(i) + "]");
        prog->addUniform(light + ".valid");
        prog->addUniform(light + ".type");
        prog->addUniform(light + ".position");
        prog->addUniform(light + ".direction");
        prog->addUniform(light + ".inner_cutoff");
        prog->addUniform(light + ".outer_cutoff");
        prog->addUniform(light + ".ambient");
        prog->addUniform(light + ".diffuse");
        prog->addUniform(light + ".specular");
        prog->addUniform(light + ".constant");
        prog->addUniform(light + ".linear");
        prog->addUniform(light + ".quadratic");
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
    shadowProg->addUniform("lightSpaceMatrix");
    shadowProg->addUniform("M");
    shadowProg->addAttribute("vertPos");
}

void Application::initGeometry(const string objectDirectory)
{	
    // Import models
    skysphere = make_shared<Model>((objectDirectory + "/skysphere.obj"));
    skysphere->normalize();
    
    drum_set = make_shared<Model>((objectDirectory + "/drum_set/drum_set.obj"));
    drum_set->normalize();

    // Setup user-created objects
    initPlane(&planeVAO, &planeVBO);
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
    stage_lights[0].position = glm::vec3(0.0f, 5.0f, -6.0f);
    stage_lights[0].id = lightingSystem.spawnSpotLight(glm::vec3(1.0f), 20, 25, 
        stage_lights[0].position, glm::vec3(0.0f, -0.7f, 1.0f));
    // stage_lights[1] = lightingSystem.spawnSpotLight(glm::vec3(0.0f, 1.0f, 0.0f), 
    // 	20, 25, glm::vec3( 1.0f, 5.0f, -1.0f));
    // stage_lights[2] = lightingSystem.spawnSpotLight(glm::vec3(0.0f, 0.0f, 1.0f), 
    // 	20, 25, glm::vec3( 0.0f, 5.0f,  1.0f));
    // stage_lights[3] = lightingSystem.spawnPointLight(glm::vec3(1.0f), 
    // 	glm::vec3( 1.0f, 2.0f,  -1.0f));

}

void Application::initShadowMap()
{
    // Setup shadow map texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Setup shadow map FBO
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}