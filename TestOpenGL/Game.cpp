#include "Game.h"

//General Functions

//Private functions
void Game::initGLFW()
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initWindow(const char* title, bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);

	if (this->window == nullptr)
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	glfwSetFramebufferSizeCallback(this->window, Game::framebuffer_resize_callback);

	//glViewport(0, 0, framebufferWidth, framebufferHeight);

	glfwMakeContextCurrent(this->window); //IMPORTANT!!
}

void Game::initGLEW()
{
	//INIT GLEW (NEEDS WINDOW AND OPENGL CONTEXT)
	glewExperimental = GL_TRUE;

	//Error Check
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initOpenGLOptions()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // change if you want to draw only outline GL_LINE

	//Input
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::initMatrices(bool isPerspective)
{


	this->ViewMatrix = glm::mat4(1.0f);
	this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);

	this->ProjectionMatrix = glm::mat4(1.0f);

	this->isPerspective = isPerspective;

	if(this->isPerspective)
		this->ProjectionMatrix = glm::perspective(glm::radians(this->fov), static_cast<float>(this->framebufferWidth) / this->framebufferHeight, this->nearPlane, this->farPlane);
	else
		this->ProjectionMatrix = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, this->nearPlane, this->farPlane);
}

void Game::initShaders()
{
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR, "vertex_shader.glsl", "fragment_shader.glsl"));
}

void Game::initInstancingShaders()
{
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR, "instancing_vertex_shader.glsl", "instancing_fragment_shader.glsl"));
}

void Game::initTextures()
{
	//Texture0
	this->textures.push_back(new Texture("Images/bic.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("Images/bic.png", GL_TEXTURE_2D));

	//Texture1
	this->textures.push_back(new Texture("Images/container.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("Images/container.png", GL_TEXTURE_2D));

	this->textures.push_back(new Texture("Images/background.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("Images/background.png", GL_TEXTURE_2D));
}

void Game::initMaterials()
{
	this->materials.push_back(new Material(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(2.0f), 0, 1));
}

void Game::initOBJModels()
{

}

void Game::initBricks(int rows, int cols, int width, int height, int horDist, int verDist)
{
	int idCounter = 0;
	for (size_t y = 0; y < rows; y++)
		for (size_t x = 0; x < cols; x++)
		{
			if (y <= 2)
			{
				this->bricks.push_back(new Brick(
					glm::vec3(x * (width + horDist), y * (height + verDist), -1.0f),
					width,
					height,
					this->materials[0],
					this->textures[TEX_BLAZARK],
					TWO_HIT_BRICK)
				);

				this->bricks[idCounter]->brick_id = idCounter;
			}
			if (y > 2)
			{
				this->bricks.push_back(new Brick(
					glm::vec3(x * (width + horDist), y * (height + verDist), -1.0f),
					width,
					height,
					this->materials[0],
					this->textures[TEX_CONTAINER],
					ONE_HIT_BRICK)
				);

				this->bricks[idCounter]->brick_id = idCounter;
			}
			idCounter++;
		}
}

void Game::initModels()
{
	std::vector<Mesh*>meshes;

	
	int width = this->bricks[0]->width;
	int height = this->bricks[0]->height;

	
	meshes.push_back(new Mesh(&Quad(width,height), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
	
	meshes.push_back(new Mesh(&Quad(38, 38), glm::vec3(12.f, 0.f, 0.0f), glm::vec3(12.f, 2.f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));

	//bricks to models
	for(int i = 0; i < bricks.size(); i++)
	{
		//cube 1
		this->models.push_back(new Model(
			bricks[i]->position,
			bricks[i]->material,
			bricks[i]->texture,
			&bricks[i]->texture[TEX_CONTAINER_SPECULAR],
			meshes[0],
			bricks[i]->brick_id)
		);
	}
		


	/*this->meshes.push_back(new Mesh(&Quad(), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));*/

	


	////Background
	this->models.push_back(new Model(
		glm::vec3(0.0f, 0.0f, -1.0f),	
		this->materials[0],
		this->textures[4],
		this->textures[4],
		meshes[1],
		0
		)
	);	

	//this->models.push_back(new Model(
	//	glm::vec3(0.0f, 2.0f, -1.0f),
	//	this->materials[0],
	//	this->textures[0],
	//	this->textures[1],
	//	meshes
	//)
	//);

	//this->models.push_back(new Model(
	//	glm::vec3(4.0f, 2.0f, 4.0f),
	//	this->materials[0],
	//	this->textures[0],
	//	this->textures[1],
	//	"OBJFiles/monkey.obj"
	//)
	//);

	/*this->models.push_back(new Model(
		glm::vec3(2.0f, 0.0f, 2.0f),
		this->materials[0],
		this->textures[TEX_CONTAINER],
		this->textures[TEX_CONTAINER_SPECULAR],
		meshes
		)
	);*/

	for (auto*& i : meshes)
		delete i;
}
  
void Game::initLights()
{
	this->lights.push_back(new glm::vec3(12.0f, 3.0f, 1.0f));
}

void Game::initUniforms()
{
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ProjectionMatrix, "ProjectionMatrix");

	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(*this->lights[0], "lightPos0");
	
}

void Game::updateUniforms()
{
	//Update view matrix (camera)
	this->ViewMatrix = this->camera.getViewMatrix();

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(this->camera.getPosition(), "cameraPos");

	//Update frame buffer size and projection matrix
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);	

	if(this->isPerspective)
		this->ProjectionMatrix = glm::perspective(glm::radians(this->fov), static_cast<float>(this->framebufferWidth) / this->framebufferHeight, this->nearPlane, this->farPlane);
	else
		this->ProjectionMatrix = glm::ortho(-17.0f, 17.0f, -17.0f, 17.0f, this->nearPlane, this->farPlane);
	
	this->camera.setPosition(glm::vec3(12.0f, 0.f, 0.f));

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
}

//Constructors/destructors
Game::Game(const char* title, const int WINDOW_WIDTH, const int WINDOW_HEIGHT, const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR, bool resizable)
	: WINDOW_WIDTH(WINDOW_WIDTH), WINDOW_HEIGHT(WINDOW_HEIGHT), GL_VERSION_MAJOR(GL_VERSION_MAJOR), GL_VERSION_MINOR(GL_VERSION_MINOR), camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
{
	//Init variables
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	this->camPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->camFront = glm::vec3(0.0f, 0.0f, -1.0f);

	this->fov = 90.0f;
	this->nearPlane = 0.1f;
	this->farPlane = 1000.0f;

	this->dt = 0.0f;
	this->curTime = 0.0f;
	this->lastTime = 0.0f;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->firstMouse = true;

	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLEW();
	this->initOpenGLOptions();

	this->initMatrices(false); //false for ortho camera
	this->initShaders();
	this->initTextures();
	this->initMaterials();
	this->initBricks(5, 5, 5, 2, 1, 1);
	this->initOBJModels();
	this->initModels();
	this->initLights();
	this->initUniforms();
}


Game::~Game()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (size_t i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];

	for (size_t i = 0; i < this->textures.size(); i++)
		delete this->textures[i];

	for (size_t i = 0; i < this->materials.size(); i++)
		delete this->materials[i];

	for (auto*& i : this->models)
		delete i;

	for (size_t i = 0; i < this->lights.size(); i++)
		delete this->lights[i];
	
}

//Accessor
int Game::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//Modifier
void Game::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}



//Functions
void Game::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void Game::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	//Calc offset
	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->lastMouseY - this->mouseY;

	//Set last X and Y
	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;
}

void Game::updateKeyboardInput()
{
	//Program
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);
	}

	//Camera
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->camera.move(this->dt, FORWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->camera.move(this->dt, BACKWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->camera.move(this->dt, LEFT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->camera.move(this->dt, RIGHT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_C) == GLFW_PRESS)
	{
		this->camPosition.y -= 0.05f;
	}
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		this->camPosition.y += 0.05f;
	}

	if (glfwGetKey(this->window, GLFW_KEY_B) == GLFW_PRESS)
	{
		

		/*Texture* textureTemp = new Texture("Images/container.png", GL_TEXTURE_2D);
		this->bricks[0]->texture = textureTemp;
		this->models[0]->setTexture(this->bricks[0]->texture);*/
	}
	if (glfwGetKey(this->window, GLFW_KEY_N) == GLFW_PRESS)
	{
		Texture* textureTemp = new Texture("Images/bic.png", GL_TEXTURE_2D);
		this->bricks[0]->texture = textureTemp;
		models[0]->setTexture(this->bricks[0]->texture);
	}
}

void Game::updateGamepadInput()
{
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	//std::cout << "Joystick/Gamepad 1 is connected: " << present << std::endl;

	if (present == 1)
	{
		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		/*std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "Left stick X Axis: " << axes[0] << std::endl;
		std::cout << "Left stick Y Axis: " << axes[1] << std::endl;

		std::cout << "Right stick X Axis: " << axes[2] << std::endl;
		std::cout << "Right stick Y Axis: " << axes[3] << std::endl;

		std::cout << "Left Trigger: " << axes[4] << std::endl;
		std::cout << "Right Trigger: " << axes[5] << std::endl;*/

		int buttonCount;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		if (GLFW_PRESS == buttons[0])
		{
			std::cout << "A button pressed: " << std::endl;
		}

		if (GLFW_PRESS == buttons[1])
		{
			std::cout << "B button pressed: " << std::endl;
		}

		if (GLFW_PRESS == buttons[2])
		{
			std::cout << "X button pressed: " << std::endl;
		}

		if (GLFW_PRESS == buttons[3])
		{
			std::cout << "Y button pressed: " << std::endl;
		}


	}
}

void Game::updateInput()
{
	glfwPollEvents();

	this->updateKeyboardInput();
	this->updateMouseInput();
	this->updateGamepadInput();
	//this->camera.updateInput(dt, -1, this->mouseOffsetX, this->mouseOffsetY);
}

void Game::updateModels()
{
	//std::vector<Mesh*>meshes;


	//int width = this->bricks[0]->width;
	//int height = this->bricks[0]->height;


	//meshes.push_back(new Mesh(&Quad(width, height), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));

	//for (int i = 0; i < bricks.size(); i++)
	//{
	//	//cube 1
	//	this->models.push_back(new Model(
	//		bricks[i]->position,
	//		bricks[i]->material,
	//		bricks[i]->texture,
	//		&bricks[i]->texture[TEX_CONTAINER_SPECULAR],
	//		meshes,
	//		bricks[i]->brick_id)
	//	);
	//}
}

void Game::update()
{
	//UPDATE INPUT ---
	this->updateDt();
	this->updateInput();
	

	//this->models[0]->rotate(glm::vec3(0.0f, 2.0f, 0.0f));
	//this->models[1]->rotate(glm::vec3(0.0f, 1.0f, 0.0f));
	//this->models[2]->rotate(glm::vec3(0.0f, 1.0f, 0.0f));
	/*std::cout << "DT: " << this->dt << "\n" << "Mouse offsetX: " << this->mouseOffsetX << " Mouse offsetY: " << this->mouseOffsetY << "\n";*/
}

void Game::render()
{

	//UPDATE ---
	//updateInput(window);

	//DRAW ---
	//CLEAR
	glClearColor(0.3f, 0.1f, 0.7f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Update the uniforms
	this->updateUniforms();

	//Render models

	for (auto& i : this->models)
	{	
		i->render(this->shaders[SHADER_CORE_PROGRAM]);
	}

	//END DRAW
	glfwSwapBuffers(window);
	glFlush();

	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Static functions
void Game::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};
