#include "MainGame.h"
#include "ImageLoader.h"
#include <iostream>
#include "ResourceManager.h"
#include "PapuEngine.h"
#include <random>
#include <algorithm>
#include <ctime>

using namespace std;

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

void MainGame::run() {
	init();
	update();
}

void MainGame::init() {
	_fps = 0;
	_previusTicks = 0;
	Papu::init();
	background = new Background("Images/naves/menu.png");
	_ship = new Ship(106, 79,glm::vec2(400,300), "Images/naves/Player.png");
	_enemies.push_back(new EnemyShip(132, 84, glm::vec2(100, 100),"Images/naves/spaceShips_005.png"));
	_enemies.push_back(new EnemyShip(136, 84, glm::vec2(200, 100), "Images/naves/spaceShips_005.png"));
	_enemies.push_back(new EnemyShip(136, 84, glm::vec2(200, 200), "Images/naves/spaceShips_005.png"));
	_window.create("Engine", _witdh, _height, 0);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	initLevel();
	initShaders();
}

void MainGame::initLevel() {
	_spriteBacth.init();
	_camera.setPosition(glm::vec2(400, 300));
}

void MainGame::initShaders() {
	_program.compileShaders("Shaders/colorShaderVert.txt",
		"Shaders/colorShaderFrag.txt");
	_program.addAtribute("vertexPosition");
	_program.addAtribute("vertexColor");
	_program.addAtribute("vertexUV");
	_program.linkShader();
}


void MainGame::draw() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_program.use();
	glActiveTexture(GL_TEXTURE0);

	GLuint pLocation =
		_program.getUniformLocation("P");

	glm::mat4 cameraMatrix = _camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1,GL_FALSE, &(cameraMatrix[0][0]));

	GLuint imageLocation = _program.getUniformLocation("myImage");
	glUniform1i(imageLocation, 0);

	_spriteBacth.begin();
	background->draw(_spriteBacth);
	_ship->draw(_spriteBacth);

	for (size_t i = 0; i < _enemies.size(); i++)
	{
		_enemies[i]->draw(_spriteBacth);
	}

	_spriteBacth.end();
	_spriteBacth.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);
	_program.unuse();
	_window.swapBuffer();
}

void MainGame::procesInput() {
	SDL_Event event;
	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			case SDL_MOUSEMOTION:
				_inputManager.setMouseCoords(event.motion.x,event.motion.y);
			break;
			case  SDL_KEYUP:
				_inputManager.releaseKey(event.key.keysym.sym);
				break;
			case  SDL_KEYDOWN:
				_inputManager.pressKey(event.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				_inputManager.pressKey(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				_inputManager.releaseKey(event.button.button);
				break;
		}
		if (_inputManager.isKeyPressed(SDLK_q)) {
			_camera.setScale(_camera.getScale() + SCALE_SPEED);
		}
		if (_inputManager.isKeyPressed(SDLK_e)) {
			_camera.setScale(_camera.getScale() - SCALE_SPEED);
		}
		if (_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoords =  _camera.convertScreenToWorl(_inputManager.getMouseCoords());
			glm::vec2 playerPosition(0, 0);
			glm::vec2 direction = mouseCoords - playerPosition;
			direction = glm::normalize(direction);
		}
	}
}

void MainGame::update() {

	while (_gameState != GameState::EXIT) {
		procesInput();
		draw();
		_camera.update();
		_time += 0.002f;
		gameLoop();
	}
}

void MainGame::gameLoop() {
	const float DESIRED_FPS = 60.0f;
	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMERATE = 
							MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;
	const int MAX_PHYSICS_STEP = 6;

	_fpsLimiter.setMaxFPS(60000.0f);
	_fpsLimiter.begin();
	float newTicks = SDL_GetTicks();
	float frameTime = newTicks - _previusTicks;
	_previusTicks = newTicks;
	float totalDeltime = frameTime / DESIRED_FRAMERATE;
	int i = 0;
	while (totalDeltime > 0.0f && i < MAX_PHYSICS_STEP)
	{
		float deltaTime = std::min(totalDeltime, 
									MAX_DELTA_TIME);
		totalDeltime -= deltaTime;
		updateAgents(deltaTime);
		i++;
	}
	_fps = _fpsLimiter.end();
}

void MainGame::updateAgents(float totalDeltime) {
}

MainGame::MainGame(): 
					  _witdh(800),
					  _height(600),
					  _gameState(GameState::PLAY),
					  _time(0)
{
	_camera.init(_witdh, _height);
}


MainGame::~MainGame()
{
}
