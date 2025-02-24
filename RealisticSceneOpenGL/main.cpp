//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include "irrKlang.h"
using namespace irrklang;

#include <string>


// window
gps::Window myWindow;




glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

GLuint textureID;


gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 5.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;
bool pressedKeys[1024];

GLfloat angle;

/////////////////////////////////////////////////////////////////////Camera Tour
//coord punctelor prin care va trece camera pentru tur
glm::vec3 scenePoints[] = {
	glm::vec3(-29.6367f, 2.09901f , -0.803323f),
	glm::vec3(-19.1892, 4.6445  , -11.9296 ),
	glm::vec3(-19.1892 , 2.58049 , 16.2098 ),
	glm::vec3(-4.52156 , 1.49063 , 14.1539 ),
	glm::vec3(7.39384  ,  2.81615   , 9.43056 ),
	glm::vec3(14.9802 , 3.06928 , 26.315 ),
	glm::vec3(25.163 , 1.1742 , 27.3318 ),
	glm::vec3(25.163 ,  2.827 , 6.95603 ),
	glm::vec3(4.93861  , 2.42129   , -6.7918),
	glm::vec3(-24.3103    , 2.0154      , -8.57107 ),
	glm::vec3(-20.616 , 2.0154  , -25.1097),
	glm::vec3(9.65097  , 2.0154   , -25.1097),

};

// nr de puncte prin care trece camera
const int nrPoints = sizeof(scenePoints) / sizeof(scenePoints[0]);
float speed = 0.3f;  // viteza camerei pentru tur
float lastFrame = 0.0f; //  timpul ultimului cadru
bool isMoving = false;
float t = 0.0f;
int currentPoint = 0;

gps::Model3D scene;
gps::Model3D train;
gps::Model3D raindrop;
gps::Model3D helicopter;
gps::Model3D elice;


//////////////////////////////////////////sound
irrklang::ISoundEngine* engine;
irrklang::ISound* rainSound;
irrklang::ISound* helicopterSound;
irrklang::ISound* fogSound;

gps::Shader myCustomShader;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
std::vector<const GLchar*> faces;

bool firstMouse = true;
float lastX = 400, lastY = 300;  
float pitch = 0.0f, yaw = +180.0f;


//------------------------------ceata
int fogEnabled = 0;
//---------------------------lumina directionala 
int directionalLightEnabled = 0;
//--------------------------lumina punctiforma
int pointLightEnabled = 0;
//-------------------------lumina spot
int spotLightEnabled = 0;

////////////////////////////animatie tren
float trainPositionZ = 0.0f;
float trainSpeed = 0.05f;
bool movingForward = true;


glm::vec3 pointLightPositions[5] = {
	  glm::vec3(-3.0402f, 3.9017f, -5.3663f),			// Felinar1
	  glm::vec3( - 27.976f, 3.3483f, 1.6535f),          // Felinar2
	  glm::vec3(-16.07f, 3.3751f, -9.9887f),          // Felinar3
	  glm::vec3(9.7353f, 3.3588f, 1.6308f),          // Felinar4
	  glm::vec3(24.102f, 3.3516f, 14.356f)           // Felinar5
};



//---------animatieTren
bool moveForwTrain = false;
float deltaTrain = 0;

//-------------elicopter
float eliceAngle = 0.0f;
float helicopterPositionY = 3.9762f;
float elicePositionY = 5.7281;
bool takeOff = false;


//---------------------------Ploaie
int rainEnabled = 0;

const int NUM_PARTICLES = 500;

// vectori pentru pozitiile si vitezele picaturilor de ploaie
float positionsX[NUM_PARTICLES];
float positionsY[NUM_PARTICLES];
float positionsZ[NUM_PARTICLES];
float speeds[NUM_PARTICLES]; 




GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {

	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void initSkybox() {

	faces.push_back("skybox/cloudtop_rt.tga");
	faces.push_back("skybox/cloudtop_lf.tga");
	faces.push_back("skybox/cloudtop_up.tga");
	faces.push_back("skybox/cloudtop_dn.tga");
	faces.push_back("skybox/cloudtop_bk.tga");
	faces.push_back("skybox/cloudtop_ft.tga");
	mySkyBox.Load(faces);

}

void initNightSkybox() {
	faces.push_back("skybox/nightsky_rt.tga");
	faces.push_back("skybox/nightsky_lf.tga");
	faces.push_back("skybox/nightsky_up.tga");
	faces.push_back("skybox/nightsky_dn.tga");
	faces.push_back("skybox/nightsky_bk.tga");
	faces.push_back("skybox/nightsky_ft.tga");
	mySkyBox.Load(faces);


}

//////////////////////////////////////////////////////////////////////////Camera Tour

glm::vec3 getCameraPosition(glm::vec3 P0, glm::vec3 P1, float t) {
	return P0 * (1 - t) + P1 * t;
}

void setCamera(glm::vec3 position, glm::vec3 target) {
	glm::mat4 view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
}

void updateCamera(float deltaTime) {
	if (isMoving) {
		t += deltaTime * speed; 
		if (t > 1.0f) {
			t = 0.0f;
			currentPoint++;
			if (currentPoint >= nrPoints - 1) {
				currentPoint = nrPoints -1;
				isMoving = false;
			}
		}

		glm::vec3 position = getCameraPosition(scenePoints[currentPoint], scenePoints[currentPoint + 1], t);
		glm::vec3 target = getCameraPosition(scenePoints[currentPoint], scenePoints[currentPoint + 1], t + 0.1f);
		setCamera(position, target);
	}
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	
	//--------------Activarea cetii

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		fogEnabled = !fogEnabled;
		if (fogEnabled) {
			
			fogSound = engine->play2D("sounds/creepy-effect-255455.mp3", GL_TRUE, false, true);
		}
		else {
			if (fogSound) {
				fogSound->stop();
				fogSound->drop();
				fogSound = nullptr;
			}
		}
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogEnabled"), fogEnabled);
		
	}

	//------------------------Activarea luminii directionale
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		directionalLightEnabled = !directionalLightEnabled;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directionalLightEnabled"), directionalLightEnabled);
	}
	
	//----------------------------Activarea luminii punctiforme
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		pointLightEnabled = !pointLightEnabled;  
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "pointLightEnabled"), pointLightEnabled);
	}

	//------------------------------Activarea luminii spot
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		spotLightEnabled = !spotLightEnabled; 
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "spotLightEnabled"), spotLightEnabled);
	}

	//------------------------------Activarea ploii
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		rainEnabled = !rainEnabled;  
		if (rainEnabled) {
			// porneste sunetul doar daca nu este deja pornit
			if (!rainSound) {
				rainSound = engine->play2D("sounds/calming-rain-257596.mp3", GL_TRUE, false, true);
			}
		}
		else {
			// stop sunet dacă este activ
			if (rainSound) {
				rainSound->stop();
				rainSound->drop();
				rainSound = nullptr;
			}
		}
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS){
		//nightEnabled = !nightEnabled;
		myCustomShader.useShaderProgram();
		lightColor = glm::vec3(0.09f, 0.09f, 0.09f); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		initNightSkybox();
		

		
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		//nightEnabled = !nightEnabled;
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		faces.pop_back();
		initSkybox();
		myCustomShader.useShaderProgram();
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); 
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	}
	
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}


	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		takeOff = !takeOff;
		//helicopterSound = engine->play2D("sounds/helicopter-sound-41975.mp3", GL_TRUE, false, true);
		
		
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		if (!isMoving) { 
			isMoving = true;
			currentPoint = 0; //incepe de la primul punct
			t = 0.0f; 
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset; //rotatia camerei pe orizontala
	pitch += yoffset; //rotatia camerei pe verticala

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw); //actualizam pozitia si directia camerei
	myCustomShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); //trimitem matricea catre shader

}



void processMovement()
{

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);	
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);	
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);		
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);		
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

}

void initOpenGLWindow() {
	myWindow.Create(1200, 800, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}


void initOpenGLState()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	
	glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void initObjects() {

	scene.LoadModel("objects/scene1.obj");
	train.LoadModel("objects/train/trenulet.obj");
	raindrop.LoadModel("objects/raindrop/raindrop.obj");
	helicopter.LoadModel("objects/helicopter/helicopter.obj");
	elice.LoadModel("objects/elice/elice.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}


void initUniforms() {

	myCustomShader.useShaderProgram();


	// create model matrix for teapot
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		0.1f, 2000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	
	lightDir = glm::vec3(0.0f, 1.0f, 0.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));


	//////////////////////////////////////////////////////////Lumina Punctiforma


	for (int i = 0; i < 5; ++i) {
		std::string index = std::to_string(i);

		lightColor = glm::vec3(50.0f, 50.0f, 50.0f); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");

		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].position").c_str()), //transformam in sir de caractere
			pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);

		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].constant").c_str()), 1.0f);
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].linear").c_str()), 0.7f);
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].quadratic").c_str()), 1.8f);

		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].ambient").c_str()), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].diffuse").c_str()), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, ("pointLights[" + index + "].specular").c_str()), 1.7f, 1.7f, 1.7f);
	}


	//////////////////////////////SPOTLIGHT

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].position"), -17.365f, 1.3777f, -6.196f); 
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].direction"), 0.7f, 0.0f, -0.3f);  
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].cutOff"), glm::cos(glm::radians(16.5f))); 
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].outerCutoff"), glm::cos(glm::radians(37.0f))); 
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].linear"), 0.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].quadratic"), 1.8f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[0].specular"), 1.7f, 1.7f, 1.7f);

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].position"), -16.835f, 1.3777f, -5.291f);  
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].direction"), 0.7f, 0.0f, -0.3f);  
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].cutOff"), glm::cos(glm::radians(16.5f))); 
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].outerCutoff"), glm::cos(glm::radians(37.0f)));  
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].linear"), 0.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].quadratic"), 1.8f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLights[1].specular"), 1.7f, 1.7f, 1.7f);

}


void initSoundEngine()
{
	engine = irrklang::createIrrKlangDevice();
	if (!engine)
		return;
}


void initRainDrops() {
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		positionsX[i] = (rand() % 500 - 250) / 10.0f;  
		positionsY[i] = rand() % 50 / 10.0f + 5.0f;   
		positionsZ[i] = (rand() % 500 - 250) / 10.0f;
		speeds[i] = 0.2f + static_cast<float>(rand() % 10) / 500.0f;
	}
}

// actualizarea pozitiilor picaturlor de ploaie
void updateRainDrops() {
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		positionsY[i] -= speeds[i];  // picatura cade

		// daca ajunge la sol, resetam coordonatele
		if (positionsY[i] < 0.0f) {
			positionsY[i] = rand() % 50 / 10.0f + 5.0f;
			positionsX[i] = (rand() % 500 - 250) / 10.0f;  
			positionsZ[i] = (rand() % 500 - 250) / 10.0f;
		}
	}
}

void renderRain(gps::Shader shader) {
	if (!rainEnabled) {
		return;
	}
	shader.useShaderProgram();

	updateRainDrops();

	// desenam fiecare picatura de ploaie
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(positionsX[i], positionsY[i], positionsZ[i]));
		model = glm::scale(model, glm::vec3(0.04f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		raindrop.Draw(shader); 
	}
}


void renderTrain(gps::Shader shader)
{
	shader.useShaderProgram();

	if (movingForward) {
		trainPositionZ += trainSpeed;
		if (trainPositionZ > 15.0f) {  
			movingForward = false;  
		}
	}
	else {
		trainPositionZ -= trainSpeed;
		if (trainPositionZ < -15.0f){  
			movingForward = true;  
		}
	}

	glm::mat4 copyModel = model;

	model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(27.0f, 0.5f, trainPositionZ));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	train.Draw(shader);

	model = copyModel; 
}

void renderHelicopter(gps::Shader shader) {
	shader.useShaderProgram();

	glm::mat4 copyModel = model;

	if (takeOff) {
		helicopterPositionY += 0.01f; 
		if (helicopterPositionY > 10.0f) {  
			helicopterPositionY = 10.0f;
		}
	}
	else {
		if (helicopterPositionY > 3.8047f) {
			helicopterPositionY -= 0.01f;
		}
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(23.979f, helicopterPositionY, -3.8047f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	helicopter.Draw(shader);

	model = copyModel;
}

void renderRotor(gps::Shader shader) {
	shader.useShaderProgram();

	glm::mat4 copyModel = model;

	if (takeOff) {
		eliceAngle += 5.0f;
		if (eliceAngle >= 360.0f) {
			eliceAngle = 0.0f;
		}
		elicePositionY += 0.01f;  
		if (elicePositionY > 10.0f) { 
			elicePositionY = 10.0f;
		}
	}
	else {
		if (elicePositionY > 3.8047f) {
			elicePositionY -= 0.01f;
		}
		eliceAngle -= 7.0f;
		if (eliceAngle == 0.0f) {
			eliceAngle = 360.0f;
		}
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(23.972f, elicePositionY, -3.891f));
	model = glm::rotate(model, glm::radians(eliceAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	elice.Draw(shader);

	model = copyModel;
}

void renderScene() {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		myCustomShader.useShaderProgram();
		//send teapot model matrix data to shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//send teapot normal matrix data to shader
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		/// calculam parametrii necesari pentru turul automat
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		updateCamera(deltaTime);


		scene.Draw(myCustomShader);
		renderTrain(myCustomShader);
		mySkyBox.Draw(skyboxShader, view, projection);


		
	
	
}




void cleanup() {
	myWindow.Delete();
	
}




int main(int argc, const char * argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	initOpenGLState();
	initObjects();
	initSkybox();
	initShaders();
	initUniforms();
	setWindowCallbacks();
	initRainDrops();
	
	
	initSoundEngine();


	
	helicopterSound = engine->play2D("sounds/helicopter-sound-41975.mp3", GL_TRUE, false, true);
	

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		processMovement();
		renderScene();
		renderRain(myCustomShader);
		renderHelicopter(myCustomShader);
		renderRotor(myCustomShader);
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}
