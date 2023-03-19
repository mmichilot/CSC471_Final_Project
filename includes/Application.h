#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "WindowManager.h"
#include "Program.h"
#include "Model.h"
#include "LightingSystem.h"
#include "Camera.h"

using namespace std;

#define WIDTH  1280
#define HEIGHT 720

class Application : public EventCallbacks
{
public:
    WindowManager *windowManager = nullptr;
            
    // Our shader programs
	shared_ptr<Program> prog;
	shared_ptr<Program> skyProg;
	shared_ptr<Program> shadowProg;

	// Models
	 unsigned int planeVAO, planeVBO;

	// Imported Models
	shared_ptr<Model> drum_set;
	shared_ptr<Model> skysphere;

	// Textures
	unsigned int skysphere_texture;
	unsigned int stage_texture;

	// Lights
	LightingSystem lightingSystem;
	struct {
		unsigned int id;
		glm::vec3 position;
	} stage_lights[10];

	// Shadow mapping
	unsigned int depthMapFBO;
	unsigned int depthMap;
	const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

	// Camera
	Camera camera;
	float lastX = WIDTH/2.0f;
	float lastY = HEIGHT/2.0f;

	// Frame Timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Key States
	bool pressedW = false;
	bool pressedS = false;
	bool pressedA = false;
	bool pressedD = false;

    /* Callbacks*/
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow *window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);

    /* Initialization */
    void init();
    void initShaders(const string shaderDirectory);
    void initGeometry(const string objectDirectory);
    void initTextures(const string textureDirectory);
    void initLights();
    void initShadowMap();

	void render();

private:
    void renderSkysphere(shared_ptr<Program> prog);
	void renderScene(shared_ptr<Program> prog, bool useMaterials = true);
	void renderStage(shared_ptr<Program> prog, bool useMaterials = true);
	void renderPlane(shared_ptr<Program> prog, bool useMaterials = true, int texture = -1);

	
};

#endif