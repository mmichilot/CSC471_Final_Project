#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "WindowManager.h"
#include "Program.h"
#include "Model.h"
#include "LightingSystem.h"
#include "Camera.h"
#include "AudioSystem.h"
#include "common.h"

using namespace std;

#define WIDTH  1280
#define HEIGHT 720
#define SHADOW_WIDTH  4096
#define SHADOW_HEIGHT 4096

struct DrumPiece {
	unsigned int source_id;
	
	bool playLeft = false;
	unsigned int leftFrameCount = 0;
	
	bool playRight = false;
	unsigned int rightFrameCount = 0;
};

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
	shared_ptr<Model> amp;

	// Textures
	unsigned int skysphere_texture;
	unsigned int stage_texture;

	// Lights
	LightingSystem lightingSystem;
	vector<unsigned int> stageLights;

	// Shadow mapping
	unsigned int shadowFBO[10];
	unsigned int shadowMaps;

	// Audio
	AudioSystem audioSystem;

	// Drum Set audio
	DrumPiece snare;
	DrumPiece hi_hat;
	DrumPiece kick;

	// Stage
	glm::vec3 stageCenter = glm::vec3(0.0f, 0.0f, -4.0f);
	float stageWidth = 15.0f;
	float stageDepth = 10.0f;
	float stageHeight = 5.0f;

	// Camera
	Camera camera;
	Camera drumCam;
	Camera stageCam;
	Camera *currCam = &camera;
	float lastX = WIDTH/2.0f;
	float lastY = HEIGHT/2.0f;

	// Player
	bool freeRoam = true;
	bool fixedCam = false;
	bool useDrums = false;
	const float playerHeight = 0.6f;
	const float playerWidth  = 0.4f;
	BoundingBox playerBB = {
		glm::vec3( -playerWidth/2.0f, 0.0f, -playerWidth/2.0f),
		glm::vec3(  playerWidth/2.0f, playerHeight, playerWidth/2.0f)
	};

	// Drum Interaction
	glm::vec2 useDrumsCenter = glm::vec2(0.0f, -1.0f);
	float     useDrumsRadius = 0.8f;

	// Frame Timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Key States
	bool pressedUp    = false;
	bool pressedDown  = false;
	bool pressedLeft  = false;
	bool pressedRight = false;
	bool pressedUse   = false;

    /* Callbacks*/
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow *window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);

    /* Initialization */
    void init();
    void initShaders(const string shaderDirectory);
    void initGeometry(const string objectDirectory);
    void initTextures(const string textureDirectory);
	void initAudio(const string audioDirectory);
    void initLights();
	void initShadows();
	void initCameras();

	void render();

private:
	/* Rendering */
    void renderSkysphere(shared_ptr<Program> prog);
	void renderScene(shared_ptr<Program> prog, bool useMaterials = true);
	void renderStage(shared_ptr<Program> prog, bool useMaterials = true);
	void renderPlane(shared_ptr<Program> prog, bool useMaterials = true, int texture = -1);
	void renderShadowMaps(float aspect);
	
	/* Logic */
	void sceneLogic();
	void drumLogic();
	void checkDrumInteraction();
	bool checkPlayerCollisions();
};

#endif