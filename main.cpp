#define PI 3.14159265359
#define WinX 100
#define WinY 40
#define KeyPressesSize 200
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <functional>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <shader.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <HelperClasses.hpp>
#include <RenderText.hpp>
#include <MazeGeneration.hpp>
#include <SkyBox.hpp>

int WinWidth = 1000, WinHeight = 1000;
const bool fullScreenActive = true;
void CreateGameStage();
Player player;
glm::mat4 projection;
glm::mat4 view;
bool won = false;
bool infiniteJumpMode = false;
GLuint fontVBO, fontVAO, fontProgramID;
std::list<Object> mazeObjects;
std::list<Object> monkeyIdolObjects;
int spawnedIdols = 0;
int gatheredIdols = 0;
Object cubeTemplate;
Object monkeyIdolTemplate;
Object floorObject;
LightsShader lightsShader;
GLuint monkeyVAO;
GLuint cubeVAO;
VBOs monkeyVBOs;
VBOs cubeVBOs;


void FillShader(Object& object, LightsShader& shader) {
	glUniformMatrix4fv(shader.ModelMatrixID, 1, GL_FALSE, &object.model[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(shader.ProjectionMatrixID, 1, GL_FALSE, &projection[0][0]);

	glUniform3f(shader.viewPos, player.camera.position.x, player.camera.position.y, player.camera.position.z);
	glUniform3f(shader.colorID, object.color.r, object.color.g, object.color.b);

	glUniform1i(shader.textureID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.textureID);
}

void StoreVAO(Object& object, GLuint VAO, VBOs& vbos) {

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, object.indexed_vertices.size() * sizeof(glm::vec3), &object.indexed_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindBuffer(GL_ARRAY_BUFFER, vbos.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, object.indexed_uvs.size() * sizeof(glm::vec2), &object.indexed_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindBuffer(GL_ARRAY_BUFFER, vbos.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, object.indexed_normals.size() * sizeof(glm::vec3), &object.indexed_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indices.size() * sizeof(unsigned short), &object.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void DrawObject(Object& object, LightsShader& shader, GLuint VAO) {

	FillShader(object, shader);

	glBindVertexArray(VAO);

	glDrawElements(
		GL_TRIANGLES,
		object.indices.size(),
		GL_UNSIGNED_SHORT,
		(void*)0
	);
}

void init()
{
	std::srand(std::time(0));

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glDepthFunc(GL_LEQUAL);

	glGenVertexArrays(1, &monkeyVAO);
	glBindVertexArray(monkeyVAO);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	lightsShader = LightsShader("lightShader2.vert", "lightShader2.frag");
	lightsShader.SetDirectionalLight();

	monkeyVBOs.GenerateBuffers();
	cubeVBOs.GenerateBuffers();

	cubeTemplate = Object("mandrill_cube.obj", "mandrill_cube_map.jpg", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	monkeyIdolTemplate = Object("suzanne.obj", glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	StoreVAO(cubeTemplate, cubeVAO, cubeVBOs);
	StoreVAO(monkeyIdolTemplate, monkeyVAO, monkeyVBOs);


	fontProgramID = SetupFontShader(WinWidth, WinHeight);
	LoadFont();
	InitTextObjects(fontVAO, fontVBO);

	PlayMP3("mandrill_maze_bgm.mp3");

	floorObject = Object(cubeTemplate, glm::vec3(100.0f, -2.0f, 100.0f), glm::vec3(200.0f, 1.0f, 200.0f));
	floorObject.color = glm::vec3(129.0f / 255.0f, 131.0f / 255.0f, 127.0f / 255.0f);
	GLuint defaultTextureID = LoadTexture("grass_texture.jpg");
	floorObject.textureID = defaultTextureID;

	InitSkyBox();

	CreateGameStage();

	view = glm::lookAt(
		player.camera.position,
		player.camera.position + player.camera.forward,
		player.camera.up
	);

}

float gravity = -0.00010f;
glm::vec3 gravityVelocity(0.0f, 0.0f, 0.0f);
bool canJump = false;

void OnJumpKeyPressed()
{
	if (canJump || infiniteJumpMode) {
		gravityVelocity.y = 0.05f;
		canJump = false;
	}
}

void CreateGameStage() {

	std::vector<std::vector<int>> maze = CreateMaze();
	float blockScale = 3.0f;
	float spaceBetweenBlocks = blockScale * 2.0f;
	float distanceModifier = 0.1f;
	int counter = 0;

	bool playerPositionFound = false;
	lightsShader.Use();

	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[i].size(); j++) {


			if (maze[i][j] == 1 || maze[i][j] == 2) {
				mazeObjects.emplace_back(cubeTemplate, glm::vec3(i * spaceBetweenBlocks, blockScale / 2.0f, j * spaceBetweenBlocks), glm::vec3(blockScale, blockScale, blockScale));
			}
			else if (maze[i][j] == 0) {
				if (!playerPositionFound) {

					player.ChangePosition(glm::vec3(i * spaceBetweenBlocks, 1.0f, j * spaceBetweenBlocks));
					playerPositionFound = true;

				}
				else if ((rand() / double(RAND_MAX) <= 0.1f)) {

					lightsShader.AddPointLight(glm::vec3(i * spaceBetweenBlocks, 0.5f, j * spaceBetweenBlocks), glm::vec3(0.15f, 0.15f, 0.15f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09, 0.032);
					monkeyIdolObjects.emplace_back(monkeyIdolTemplate, glm::vec3(i * spaceBetweenBlocks, 0.5f, j * spaceBetweenBlocks), glm::vec3(1.0f, 1.0f, 1.0f));
					monkeyIdolObjects.back().pointLightIndex = lightsShader.numPointLights - 1;
					spawnedIdols += 1;
				}
			}

		}
	}

}

void DestroyAllObjects() {
	mazeObjects.clear();
	monkeyIdolObjects.clear();
}

void ResetGameStage() {
	DestroyAllObjects();
	gatheredIdols = 0;
	spawnedIdols = 0;
	won = false;
	lightsShader.numPointLights = 0;
	CreateGameStage();

}

void WinCondition() {
	mazeObjects.clear();
	monkeyIdolObjects.clear();
	won = true;
}

bool onFloor;
glm::vec3 horzVerticalMovement;

void Update(float deltaTime) {

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 positionSum = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 newPosition;
	onFloor = false;
	bool positionChanged = false;

	lightsShader.Use();
	std::list<Object>::iterator it = mazeObjects.begin();

	for (; it != mazeObjects.end(); ++it) {
		DrawObject(*it, lightsShader, cubeVAO);
		if (PushBackIfIntersecting(player.collider, it->collider, newPosition, onFloor)) {

			player.ChangePosition2(player.position + newPosition);

		}
	}

	DrawObject(floorObject, lightsShader, cubeVAO);
	if (PushBackIfIntersecting(player.collider, floorObject.collider, newPosition, onFloor)) {
		player.ChangePosition2(player.position + newPosition);
	}


	it = monkeyIdolObjects.begin();

	while (it != monkeyIdolObjects.end() && !monkeyIdolObjects.empty()) {
		DrawObject(*it, lightsShader, monkeyVAO);
		it->RotateHorizontally(0.001 * deltaTime);
		if (Intersect(player.collider, it->collider)) {
			lightsShader.RemovePointLight(it->pointLightIndex);
			monkeyIdolObjects.erase(it++);
			gatheredIdols += 1;
			if (gatheredIdols >= spawnedIdols) WinCondition();
		}
		else {
			++it;
		}
	}

	if (!onFloor) {
		gravityVelocity.y += gravity * (float)deltaTime;
	}
	else {
		if (gravityVelocity.y < 0.0f)
			gravityVelocity.y = 0.0f;

		canJump = true;
	}


	view = glm::lookAt(
		player.camera.position,
		player.camera.position + player.camera.forward,
		player.camera.up
	);

	DrawSkyBox(view, projection);

	if (won) {
		RenderText(fontProgramID, fontVAO, fontVBO, "You defeated the Mandrill Maze! (Press R to reset)", 200.0f, 550.0f, 0.5f, glm::vec3(0.6f, 0.8f, 0.2f));
	}

	if (infiniteJumpMode) {
		RenderText(fontProgramID, fontVAO, fontVBO, "Infinite jump enabled.", 700.0f, 75.0f, 0.5f, glm::vec3(0.6f, 0.8f, 0.2f));
	}

	RenderText(fontProgramID, fontVAO, fontVBO, "Monkey idols gathered: " + std::to_string(gatheredIdols) + " / " + std::to_string(spawnedIdols), 15.0f, 75.0f, 0.5f, glm::vec3(0.6f, 0.8f, 0.2f));

}

bool mouseLocked = true;

void ToggleMouseLock() {
	mouseLocked = !mouseLocked;
	if (mouseLocked) {

		SDL_SetRelativeMouseMode(SDL_TRUE);

	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);

	}

}



int main(int ArgCount, char** Args) {

	uint32_t WindowFlags = SDL_WINDOW_OPENGL;

	SDL_Window* Window = SDL_CreateWindow("The Marvelous Mandrill Maze", WinX, WinY, WinWidth, WinHeight, WindowFlags);


	if (fullScreenActive) {
		WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		WinWidth = DM.w;
		WinHeight = DM.h;
		SDL_DestroyWindow(Window);
		Window = SDL_CreateWindow("The Marvelous Mandrill Maze", WinX, WinY, WinWidth, WinHeight, WindowFlags);
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);


	SDL_GetWindowSize(Window, &WinWidth, &WinHeight);

	projection = glm::perspective(
		glm::radians(90.0f),						// The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		(float)WinWidth / (float)WinHeight,			// Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,										// Near clipping plane. Keep as big as possible, or you'll get precision issues.
		400.0f										// Far clipping plane. Keep as little as possible.
	);

	//assert(Window);
	SDL_GLContext Context = SDL_GL_CreateContext(Window);

	std::srand(std::time(0));




	//glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "Glew Error: " << glewError << " : " <<
			glewGetErrorString(glewError) << std::endl;
		exit(EXIT_FAILURE);
	}

	init();

	bool keyPresses[KeyPressesSize];
	std::fill(std::begin(keyPresses), std::end(keyPresses), false);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	bool running = 1;

	float startTime = SDL_GetTicks();
	int frameCount = 0;
	int currentFPS = 0;

	std::cout << "GL VERSION: " << glGetString(GL_VERSION) << std::endl;




	std::cout << "START" << std::endl;

	while (running) {

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

		SDL_Event event;



		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {

				switch (event.key.keysym.sym) {

				case SDLK_p:
					ToggleMouseLock();
					break;
				case SDLK_ESCAPE:
					running = 0;
					std::cout << "EXIT" << std::endl;
					break;
				case SDLK_r:
					ResetGameStage();
					break;
				case SDLK_i:
					infiniteJumpMode = !infiniteJumpMode;
					break;
				case SDLK_m:
					ToggleMute();
				default:
					break;
				}
				if ((SDL_KeyCode)event.key.keysym.sym < KeyPressesSize)
					keyPresses[(SDL_KeyCode)event.key.keysym.sym] = true;

			}
			if (event.type == SDL_KEYUP) {
				if ((SDL_KeyCode)event.key.keysym.sym < KeyPressesSize)
					keyPresses[(SDL_KeyCode)event.key.keysym.sym] = false;
			}

			if (event.type == SDL_QUIT) {
				running = 0;
			}

			if (event.type == SDL_MOUSEMOTION) {
				player.camera.mouse(event.motion.xrel, event.motion.yrel);
			}


		}


		float verticalModifier = 0.0f;
		float horizontalModifier = 0.0f;
		float speedModifier = 0.010f * 1.5f;

		//continuous-response keys
		if (keyPresses[SDLK_w]) {
			verticalModifier = 1.0f;
		}
		if (keyPresses[SDLK_s]) {
			verticalModifier = -1.0f;
		}
		if (keyPresses[SDLK_d]) {
			horizontalModifier = 1.0f;
		}
		if (keyPresses[SDLK_a]) {
			horizontalModifier = -1.0f;
		}

		horzVerticalMovement = glm::vec3(0.0f, 0.0f, 0.0f);

		if (verticalModifier != 0.0f || horizontalModifier != 0.0f) {

			glm::vec3 normForward(0.0f, 0.0f, 0.0f);
			if (verticalModifier != 0.0f) {

				normForward = player.camera.forward;
				normForward.y = 0.0f;
				normForward = normalize(normForward);
			}

			horzVerticalMovement = normalize(normForward * verticalModifier + player.camera.right * horizontalModifier);

		}

		Update(deltaTime);

		player.ChangePosition(player.position + gravityVelocity + horzVerticalMovement * speedModifier * (float)deltaTime);

		if (keyPresses[SDLK_SPACE]) {
			OnJumpKeyPressed();
		}

		frameCount += 1;
		if (SDL_GetTicks() - startTime >= 1000) {
			startTime = SDL_GetTicks();
			currentFPS = frameCount;
			frameCount = 0;
		}
		RenderText(fontProgramID, fontVAO, fontVBO, "FPS: " + std::to_string(currentFPS), WinWidth * 0.85f, 75.0f, 0.5f, glm::vec3(0.6f, 0.8f, 0.2f));

		SDL_GL_SwapWindow(Window);
	}
	SDL_Quit();

	return 0;
}