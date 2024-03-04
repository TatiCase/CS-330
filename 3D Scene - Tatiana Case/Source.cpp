#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpengl/camera.h> // Camera class

// include the provided basic shape meshes code
#include "meshes.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "3D Scene - Tatiana Case"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nVertices;   // Number of vertices of the mesh
		GLuint nIndices;    // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	// Shader program
	GLuint gProgramId;
	// camera
	Camera gCamera(glm::vec3(0.0f, 2.0f, 15.0f));
	float gLastX = WINDOW_WIDTH / 10.0f;
	float gLastY = WINDOW_HEIGHT / 10.0f;
	bool gFirstMouse = true;
	bool isPerspective = true;
	// Texture Ids
	GLuint gPlane;
	GLuint gTeddy;
	GLuint gBlack;
	GLuint gWhite;
	GLuint gBead;
	GLuint gDonut;
	GLuint gDonut2;
	GLuint gHotDog;
	GLuint gHotDogBun;
	GLuint gTeddyBody;
	GLuint gHotDogBunEnd;
	GLuint gHotDogEnd;
	//Shape Meshes from Professor Brian
	Meshes meshes;
	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////// 
/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 vertexPosition; // VAP position 0 for vertex position data
layout(location = 1) in vec3 vertexNormal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexFragmentNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0f); // Transforms vertices into clip coordinates

	vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

	vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal; // get normal vectors in world space only and exclude normal translation properties
	vertexTextureCoordinate = textureCoordinate;
}
);

const GLchar* fragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec4 objectColor;

uniform vec3 ambientColor;

uniform vec3 light1Color;
uniform vec3 light1Position;

uniform vec3 light2Color;
uniform vec3 light2Position;

uniform vec3 light3Color;
uniform vec3 light3Position;

uniform vec3 light4Color;
uniform vec3 light4Position;

uniform vec3 viewPosition;

uniform sampler2D uTexture; // Useful when working with multiple textures

uniform bool ubHasTexture;

uniform float ambientStrength = 0.1f; // Set ambient or global lighting strength

uniform float specularIntensity1 = 0.8f;
uniform float highlightSize1 = 16.0f;

uniform float specularIntensity2 = 0.8f;
uniform float highlightSize2 = 16.0f;

uniform float specularIntensity3 = 0.8f;
uniform float highlightSize3 = 16.0f;

uniform float specularIntensity4 = 0.8f;
uniform float highlightSize4 = 16.0f;

void main()
{
	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	//Calculate Ambient lighting
	vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color

	//**Calculate Diffuse lighting**
	vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit

	vec3 light1Direction = normalize(light1Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments
	float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse1 = impact1 * light1Color; // Generate diffuse light color

	vec3 light2Direction = normalize(light2Position - vertexFragmentPos);
	float impact2 = max(dot(norm, light2Direction), 0.0);
	vec3 diffuse2 = impact2 * light2Color;

	vec3 light3Direction = normalize(light3Position - vertexFragmentPos);
	float impact3 = max(dot(norm, light3Direction), 0.0);
	vec3 diffuse3 = impact3 * light3Color;

	vec3 light4Direction = normalize(light4Position - vertexFragmentPos);
	float impact4 = max(dot(norm, light4Direction), 0.0);
	vec3 diffuse4 = impact4 * light4Color;

	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction

	vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize1);
	vec3 specular1 = specularIntensity1 * specularComponent1 * light1Color;

	vec3 reflectDir2 = reflect(-light2Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
	vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;

	vec3 reflectDir3 = reflect(-light3Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent3 = pow(max(dot(viewDir, reflectDir3), 0.0), highlightSize3);
	vec3 specular3 = specularIntensity3 * specularComponent3 * light3Color;

	vec3 reflectDir4 = reflect(-light4Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent4 = pow(max(dot(viewDir, reflectDir4), 0.0), highlightSize4);
	vec3 specular4 = specularIntensity4 * specularComponent4 * light4Color;

	//**Calculate phong result**
	//Texture holds the color to be used for all three components
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate);
	vec3 phong1;
	vec3 phong2;
	vec3 phong3;
	vec3 phong4;

	if (ubHasTexture == true)
	{
		phong1 = (ambient + diffuse1 + specular1) * textureColor.xyz;
		phong2 = (ambient + diffuse2 + specular2) * textureColor.xyz;
		phong3 = (ambient + diffuse3 + specular3) * textureColor.xyz;
		phong4 = (ambient + diffuse4 + specular4) * textureColor.xyz;
	}
	else
	{
		phong1 = (ambient + diffuse1 + specular1) * objectColor.xyz;
		phong2 = (ambient + diffuse2 + specular2) * objectColor.xyz;
		phong3 = (ambient + diffuse3 + specular3) * objectColor.xyz;
		phong4 = (ambient + diffuse4 + specular4) * objectColor.xyz;
	}

	fragmentColor = vec4(phong1 + phong2 + phong3 + phong4, 1.0); // Send lighting results to GPU
}
);
///////////////////////////////////////////////////////////////////////////////////////

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

// Main Function: Entry point to OpenGL program
int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the basic shape meshes for use
	meshes.CreateMeshes();

	// Create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	// Load textures
	const char* texFilename = "../../resources/textures/plane.png";
	if (!UCreateTexture(texFilename, gPlane))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/VampireTeddy.png";
	if (!UCreateTexture(texFilename, gTeddy))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/black.png";
	if (!UCreateTexture(texFilename, gBlack))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/white.png";
	if (!UCreateTexture(texFilename, gWhite))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/bead.png";
	if (!UCreateTexture(texFilename, gBead))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/donut.png";
	if (!UCreateTexture(texFilename, gDonut))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/donut2.png";
	if (!UCreateTexture(texFilename, gDonut2))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/hotdog.png";
	if (!UCreateTexture(texFilename, gHotDog))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/hotdogbun.png";
	if (!UCreateTexture(texFilename, gHotDogBun))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/hotdogbunend.png";
	if (!UCreateTexture(texFilename, gHotDogBunEnd))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/teddybody.png";
	if (!UCreateTexture(texFilename, gTeddyBody))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../../resources/textures/hotdogend.png";
	if (!UCreateTexture(texFilename, gHotDogEnd))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPlane);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gTeddy);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBlack);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gWhite);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gBead);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gDonut);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gDonut2);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, gHotDog);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, gHotDogBun);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, gTeddyBody);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, gHotDogBunEnd);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, gHotDogEnd);

	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	gCamera.Position = glm::vec3(0.0f, 4.25f, 16.0f);
	gCamera.Front = glm::vec3(0.0, 1.75, 0.0f);
	gCamera.Up = glm::vec3(0.0, 2.0, 0.0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(gWindow))
	{
		// per-frame timing
		// --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	meshes.DestroyMeshes();

	// Release shader program
	UDestroyShaderProgram(gProgramId);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	// ---------------------
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW: initialize
	// ----------------
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	// Add UP/DOWN Camera Movement
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	// toggle normal and orthographic projection when user presses "P"
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		isPerspective = GL_FALSE;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		isPerspective = GL_TRUE;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}



// Functioned called to render a frame
void URender()
{
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;
	GLint viewPosLoc;

	GLint ambStrLoc;
	GLint ambColLoc;

	GLint light1ColLoc;
	GLint light1PosLoc;

	GLint light2ColLoc;
	GLint light2PosLoc;

	GLint light3ColLoc;
	GLint light3PosLoc;

	GLint light4ColLoc;
	GLint light4PosLoc;

	GLint objColLoc;

	GLint specInt1Loc;
	GLint highlghtSz1Loc;

	GLint specInt2Loc;
	GLint highlghtSz2Loc;

	GLint specInt3Loc;
	GLint highlghtSz3Loc;

	GLint specInt4Loc;
	GLint highlghtSz4Loc;

	GLint uHasTextureLoc;

	bool ubHasTextureVal;

	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 rotation1;
	glm::mat4 rotation2;
	glm::mat4 translation;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera/view transformation
	view = gCamera.GetViewMatrix();

	// Creates a projection THEN create perspective or orthographic view
	projection;
	// NOW check if 'isPerspective' and THEN set the perspective
	if (isPerspective) {
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		float scale = 90;
		projection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 0.1f, 100.0f);
	}

	// Set the shader to be used
	glUseProgram(gProgramId);

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	viewPosLoc = glGetUniformLocation(gProgramId, "viewPosition");

	ambStrLoc = glGetUniformLocation(gProgramId, "ambientStrength");
	ambColLoc = glGetUniformLocation(gProgramId, "ambientColor");

	light1ColLoc = glGetUniformLocation(gProgramId, "light1Color");
	light1PosLoc = glGetUniformLocation(gProgramId, "light1Position");

	light2ColLoc = glGetUniformLocation(gProgramId, "light2Color");
	light2PosLoc = glGetUniformLocation(gProgramId, "light2Position");

	light3ColLoc = glGetUniformLocation(gProgramId, "light3Color");
	light3PosLoc = glGetUniformLocation(gProgramId, "light3Position");

	light4ColLoc = glGetUniformLocation(gProgramId, "light4Color");
	light4PosLoc = glGetUniformLocation(gProgramId, "light4Position");

	objColLoc = glGetUniformLocation(gProgramId, "objectColor");

	specInt1Loc = glGetUniformLocation(gProgramId, "specularIntensity1");
	highlghtSz1Loc = glGetUniformLocation(gProgramId, "highlightSize1");

	specInt2Loc = glGetUniformLocation(gProgramId, "specularIntensity2");
	highlghtSz2Loc = glGetUniformLocation(gProgramId, "highlightSize2");

	specInt3Loc = glGetUniformLocation(gProgramId, "specularIntensity3");
	highlghtSz3Loc = glGetUniformLocation(gProgramId, "highlightSize3");

	specInt4Loc = glGetUniformLocation(gProgramId, "specularIntensity4");
	highlghtSz4Loc = glGetUniformLocation(gProgramId, "highlightSize4");

	uHasTextureLoc = glGetUniformLocation(gProgramId, "ubHasTexture");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the camera view location
	glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);
	//set ambient lighting strength
	glUniform1f(ambStrLoc, 0.3f);
	//set ambient color
	glUniform3f(ambColLoc, 0.2f, 0.2f, 0.2f);

	ubHasTextureVal = true;
	glUniform1i(uHasTextureLoc, ubHasTextureVal);

	///////////////////////////////////SHAPES////////////////////////////////////////

	/*Start Plane*/
	///-------Transform and draw the bottom plane --------
		// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPlaneMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(6.0f, 1.0f, 6.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

	// Light 1 is Diffuse
	glUniform3f(light1ColLoc, 0.25f, 0.25f, 0.25f);
	glUniform3f(light1PosLoc, 2.0f, 5.0f, 7.0f);

	glUniform3f(light2ColLoc, 0.60f, 0.60f, 0.60f);
	glUniform3f(light2PosLoc, 10.0f, 5.0f, 10.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, 0.6f);
	glUniform1f(specInt2Loc, 0.8f);
	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 2.0f);
	glUniform1f(highlghtSz2Loc, 8.0f);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*End Plane*/

	/*Start Vampire Teddy*/
	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Left Leg
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-0.32f, 0.05f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 2);

	// Light 1 and light 3 are diffuse
	glUniform3f(light1ColLoc, 0.25f, 0.25f, 0.25f);
	glUniform3f(light1PosLoc, 2.0f, 5.0f, 7.0f);

	glUniform3f(light2ColLoc, 0.50f, 0.50f, 0.50f);
	glUniform3f(light2PosLoc, 8.0f, 5.0f, 15.0f);

	glUniform3f(light3ColLoc, 0.25f, 0.25f, 0.25f);
	glUniform3f(light3PosLoc, 2.0f, 5.0f, -7.0f);

	glUniform3f(light4ColLoc, 0.50f, 0.50f, 0.50f);
	glUniform3f(light4PosLoc, -8.0f, 5.0f, -15.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, 0.6f);
	glUniform1f(specInt2Loc, 0.8f);
	glUniform1f(specInt3Loc, 0.6f);
	glUniform1f(specInt4Loc, 0.8f);

	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 2.0f);
	glUniform1f(highlghtSz2Loc, 8.0f);
	glUniform1f(highlghtSz3Loc, 2.0f);
	glUniform1f(highlghtSz4Loc, 8.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Left Arm
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.35f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(1.65f, glm::vec3(0.0, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.60f, 0.90f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Right Arm
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.35f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(1.35f, glm::vec3(0.0, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-0.30f, 0.90f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Right Leg
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.32f, 0.05f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);


	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Right Ear
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.40f, 0.02f, 0.40f));
	// 2. Rotate the object
	rotation = glm::rotate(1.50f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(1.0f, 2.65f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Left Ear
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.40f, 0.02f, 0.40f));
	// 2. Rotate the object
	rotation = glm::rotate(1.50f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-.90f, 2.65f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Right Foot
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.11f, 0.11f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.32f, 0.00f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 3);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Left Foot
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.11f, 0.11));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-0.32f, 0.00f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Left Hand
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.62f, 0.89f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Right Hand
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-0.66f, 0.97f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the tapered cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Body
	glBindVertexArray(meshes.gTaperedCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5f, 1.0f, 0.5f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.0f, 0.15f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 9);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	// Head
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.0f, 2.0f, 1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 1);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*End Vampire Teddy*/

	/*Start Bead*/
	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.30f, 0.30f, 0.30f));
	// 2. Rotate the object
	rotation = glm::rotate(1.59f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.15f, 0.31f, 2.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 4);

	// Light 1 and light 3 are
	glUniform3f(light1ColLoc, 0.25f, 0.25f, 0.25f);
	glUniform3f(light1PosLoc, 2.0f, 5.0f, 7.0f);

	glUniform3f(light2ColLoc, 0.54f, 0.45f, 0.35f);
	glUniform3f(light2PosLoc, 8.0f, 5.0f, 15.0f);

	glUniform3f(light3ColLoc, 0.25f, 0.25f, 0.25f);
	glUniform3f(light3PosLoc, 2.0f, 5.0f, -7.0f);

	glUniform3f(light4ColLoc, 0.54f, 0.45f, 0.35f);
	glUniform3f(light4PosLoc, -8.0f, 5.0f, -15.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, 0.6f);
	glUniform1f(specInt2Loc, 0.8f);
	glUniform1f(specInt3Loc, 0.6f);
	glUniform1f(specInt4Loc, 0.8f);

	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 2.0f);
	glUniform1f(highlghtSz2Loc, 8.0f);
	glUniform1f(highlghtSz3Loc, 2.0f);
	glUniform1f(highlghtSz4Loc, 8.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*End Bead*/

	/*Start Donut*/
	///-------Transform and draw the torus mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gTorusMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.20f, 1.20f, 1.20f));
	// 2. Rotate the object
	rotation = glm::rotate(-0.30f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(2.5f, 1.90f, -2.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 8);

	// Light 1 and light 3 are diffuse
	glUniform3f(light1ColLoc, 0.75f, 0.75f, 0.75f);
	glUniform3f(light1PosLoc, 2.0f, 5.0f, 7.0f);

	glUniform3f(light2ColLoc, 0.4f, 0.4f, 0.2f);
	glUniform3f(light2PosLoc, 10.0f, 5.0f, 15.0f);

	glUniform3f(light3ColLoc, 0.75f, 0.75f, 0.75f);
	glUniform3f(light3PosLoc, 2.0f, 5.0f, -7.0f);

	glUniform3f(light4ColLoc, 0.4f, 0.4f, 0.2f);
	glUniform3f(light4PosLoc, -10.0f, 4.0f, -17.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, 0.2f);
	glUniform1f(specInt2Loc, 0.4f);
	glUniform1f(specInt3Loc, 0.2f);
	glUniform1f(specInt4Loc, 0.4f);

	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 2.0f);
	glUniform1f(highlghtSz2Loc, 8.0f);
	glUniform1f(highlghtSz3Loc, 2.0f);
	glUniform1f(highlghtSz4Loc, 8.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	glBindVertexArray(meshes.gTorusMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.10f, 1.10f, 1.10f));
	// 2. Rotate the object
	rotation = glm::rotate(-0.30f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(2.5f, 1.95f, -1.80f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 6);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*End Donut*/

	/*Start Hot Dog Bun*/
	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.0f, 4.0f, 1.0f));
	// 2. Rotate the object on x axis
	rotation = glm::rotate(1.57f, glm::vec3(1.0, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.0f, 1.00f, -1.75f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 8);


	// // Light 1 and light 3 are diffuse
	glUniform3f(light1ColLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(light1PosLoc, -2.0f, 6.0f, 6.0f);

	glUniform3f(light2ColLoc, 0.4f, 0.4f, 0.2f);
	glUniform3f(light2PosLoc, 10.0f, 8.0f, 10.0f);

	glUniform3f(light3ColLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(light3PosLoc, -2.0f, 6.0f, -6.0f);

	glUniform3f(light4ColLoc, 0.4f, 0.4f, 0.2f);
	glUniform3f(light4PosLoc, -10.0f, 8.0f, -12.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, 0.2f);
	glUniform1f(specInt2Loc, 0.4f);
	glUniform1f(specInt3Loc, 0.2f);
	glUniform1f(specInt4Loc, 0.4f);

	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 2.0f);
	glUniform1f(highlghtSz2Loc, 8.0f);
	glUniform1f(highlghtSz3Loc, 2.0f);
	glUniform1f(highlghtSz4Loc, 8.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotate the object
	rotation = glm::rotate(1.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.0f, 0.97f, -1.75f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 10);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.01f, 1.01f, 1.01f));
	// 2. Rotate the object
	rotation = glm::rotate(1.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-4.81f, 0.98f, 1.05f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*End Hot Dog Bun*/

	/*Start Hot Dog*/
	///-------Transform and draw the cylinder mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.60f, 4.15f, 0.60f));
	// 2. Rotate the object on x axis
	rotation = glm::rotate(1.60f, glm::vec3(1.0, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.0f, 1.80f, -1.75f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 7);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.60f, 0.60f, 0.60f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.0f, 1.80f, -1.75f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 11);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	///-------Transform and draw the sphere mesh --------
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.60f, 0.60f, 0.60f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-4.85f, 1.69f, 1.10f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*End Hot Dog*/
	///////////////////////////////////END SHAPES////////////////////////////

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}


void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}