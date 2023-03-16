/*
 * Example two meshes and two shaders (could also be used for Program 2)
 * includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "Object.h"
#include "MatrixStack.h"
#include "WindowManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks {

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	shared_ptr<Program> prog;

	// Our shader program
	shared_ptr<Program> solidColorProg;

	// Unique Objects
	shared_ptr<Object> studio_light;
	shared_ptr<Object> cube;
	shared_ptr<Object> drum_set;
	shared_ptr<Object> elec_drum_set;
	shared_ptr<Object> drum_throne;
	shared_ptr<Object> drumstick;
	shared_ptr<Object> eighth_note;
	shared_ptr<Object> quarter_note;
	shared_ptr<Object> cat;
	

	// Copies
	shared_ptr<Object> drumstick_R;
	shared_ptr<Object> drumstick_L;
	shared_ptr<Object> hh_eighth_note;
	shared_ptr<Object> snare_quarter_note;
	shared_ptr<Object> kick_quarter_note;
	shared_ptr<Object> light_1;
	shared_ptr<Object> light_2;

	// camera data
	float cameraMove = -2.5;
	vec2 cameraRot = vec2(0);

	//animation data
	float catTheta = 0;
	float hhTheta = 0;
	float sTheta = 0;
	float kTheta = 0;
	float lstickTheta = 0;
	float rstickTheta = 0;

	float hhAlpha = 0;
	float sAlpha = 0;
	float kAlpha = 0;

	float eighth_tempo = 12;
	float quarter_tempo = eighth_tempo/2;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Move camera
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			cameraMove += 0.2;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			cameraMove -= 0.2;
		}

		// Rotate camera along Y
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			cameraRot.y -= 0.1;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			cameraRot.y += 0.1;
		}

		// Rotate camera along X
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			cameraRot.x -= 0.1;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			cameraRot.x += 0.1;
		}
		
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Enable alpha transparency
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");

		// Initialize the GLSL program.
		solidColorProg = make_shared<Program>();
		solidColorProg->setVerbose(true);
		solidColorProg->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/solid_frag.glsl");
		solidColorProg->init();
		solidColorProg->addUniform("P");
		solidColorProg->addUniform("V");
		solidColorProg->addUniform("M");
		solidColorProg->addUniform("solidColor");
		solidColorProg->addAttribute("vertPos");
		solidColorProg->addAttribute("vertNor");
	}

	shared_ptr<Object> loadObject(const char *obj_file, int index=-1) {
		string errStr;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;

		cout << "\nLoading \"" << obj_file << "\"" << endl;

		bool rc = tinyobj::LoadObj(shapes, materials, errStr, obj_file);
		cout << "# of shapes: " << shapes.size() << endl;

		if (!rc) {
			cerr << errStr << endl;
			exit(2);
		}

		if (index != -1)
			return make_shared<Object>(shapes, index);
		else
			return make_shared<Object>(shapes);

	}

	void initGeom(const std::string& resourceDirectory)
	{	
		// Load base objects
		cube = loadObject((resourceDirectory + "/cube.obj").c_str());
		studio_light = loadObject((resourceDirectory + "/studio_light.obj").c_str());
		drum_set = loadObject((resourceDirectory + "/drum.obj").c_str());
		elec_drum_set = loadObject((resourceDirectory + "/edrum.obj").c_str());
		drum_throne = loadObject((resourceDirectory + "/drum_throne.obj").c_str());
		drumstick = loadObject((resourceDirectory + "/drumstick.obj").c_str());
		eighth_note = loadObject((resourceDirectory + "/MusicNotes.obj").c_str(), 1);
		quarter_note = loadObject((resourceDirectory + "/MusicNotes2.obj").c_str(), 1);
		cat = loadObject((resourceDirectory + "/cat.obj").c_str());
		

		// Setup static object attributes
		light_1 = make_shared<Object>(*studio_light);
		light_1->setUniformScale(1.2);
		light_1->setRotation(0, 250, 0);
		light_1->setPosition(-2, 1.4, -3);
		
		light_2 = make_shared<Object>(*studio_light);
		light_2->setUniformScale(1.2);
		light_2->setRotation(0, 180, 0);
		light_2->setPosition(2, 1.4, -3);

		cube->setPosition(0, -0.55, 0);
		cube->setScale(100, 1, 100);
		
		drum_throne->setPosition(-0.1, 0.75, -0.3);
		drum_throne->setUniformScale(0.3);
		
		drum_set->setPosition(0, 1, -1);

		elec_drum_set->setUniformScale(1.1);
		elec_drum_set->setRotation(0, -120, 0);
		elec_drum_set->setPosition(-1.3, 1.1, 0.1);

		drumstick_L = make_shared<Object>(*drumstick);
		drumstick_L->setUniformScale(0.25);
		drumstick_L->setPivot(0.15, 0, 0);
		drumstick_L->setPosition(-0.3, 1.25, -0.5);
		
		drumstick_R = make_shared<Object>(*drumstick);
		drumstick_R->setUniformScale(0.25);
		drumstick_R->setPivot(0.15, 0, 0);
		drumstick_R->setPosition(-0.2, 1.17, -0.95);

		hh_eighth_note = make_shared<Object>(*eighth_note);
		hh_eighth_note->setUniformScale(0.1);

		snare_quarter_note = make_shared<Object>(*quarter_note);
		snare_quarter_note->setUniformScale(0.1);

		kick_quarter_note = make_shared<Object>(*quarter_note);
		kick_quarter_note->setUniformScale(0.1);

		cat->setUniformScale(0.15);
		cat->setPivot(0, 1.2, 0);
		cat->setPosition(-0.05, 0, -0.3);
	}

	void render() {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View moved by keys
		View->pushMatrix();
			View->loadIdentity();
			View->translate(vec3(0, -1.5, cameraMove));
			View->rotate(cameraRot.y, vec3(0, 1, 0));
			View->rotate(cameraRot.x, vec3(1, 0, 0));

		// Draw using solid color fragment shader
		solidColorProg->bind();
			glUniformMatrix4fv(solidColorProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(solidColorProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));

			// Draw floor
			glUniform4f(solidColorProg->getUniform("solidColor"), 0.5, 0.3, 0.2, 1);
			cube->draw(solidColorProg);

			// Draw lights
			glUniform4f(solidColorProg->getUniform("solidColor"), 0.2, 0.2, 0.2, 1);
			light_1->draw(solidColorProg);
			light_2->draw(solidColorProg);
			
			// Draw hi-hat eighth note
			hh_eighth_note->setPosition(-0.6, 1.35+hhTheta, -0.5);
			if (lstickTheta > 0) {
				glUniform4f(solidColorProg->getUniform("solidColor"), 0, 0, 0, hhAlpha);
				hh_eighth_note->draw(solidColorProg);
			}

			// Draw snare quarter note
			snare_quarter_note->setPosition(-0.5, 1.3+sTheta, -1);
			if (rstickTheta > 0) {
				glUniform4f(solidColorProg->getUniform("solidColor"), 0, 0, 0, sAlpha);
				snare_quarter_note->draw(solidColorProg);
			}

			// Draw kick quarter note
			glUniform4f(solidColorProg->getUniform("solidColor"), 0, 0, 0, 1);
			
			kick_quarter_note->setPosition(0.1, 0.8+kTheta, -1+kTheta);
			if (kTheta > 0) {
				glUniform4f(solidColorProg->getUniform("solidColor"), 0, 0, 0, kAlpha);
				kick_quarter_note->draw(solidColorProg);
			}

		solidColorProg->unbind();

		// Draw using regular shader
		prog->bind();
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));

			cat->setRotation(0, 45, catTheta);
			cat->draw(prog);
			
			drum_set->draw(prog);
			elec_drum_set->draw(prog);
			drum_throne->draw(prog);

			drumstick_L->setRotation(0, -120, lstickTheta);
			drumstick_L->draw(prog);

			drumstick_R->setRotation(0, -130, rstickTheta);
			drumstick_R->draw(prog);
		prog->unbind();

		//animation updates
		catTheta = (180/M_PI)*0.05*sin((quarter_tempo/2)*glfwGetTime());

		hhTheta = 0.05*sin(eighth_tempo*glfwGetTime());
		hhAlpha = sin(eighth_tempo*glfwGetTime());

		sTheta = 0.05*cos(quarter_tempo*glfwGetTime());
		sAlpha = cos(quarter_tempo*glfwGetTime()); 
		
		kTheta = 0.05*sin(quarter_tempo*glfwGetTime()+M_PI);
		kAlpha = sin(quarter_tempo*glfwGetTime()+M_PI);

		lstickTheta = (180/M_PI)*0.1*sin(eighth_tempo*glfwGetTime());
		rstickTheta = (180/M_PI)*0.1*cos(quarter_tempo*glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
