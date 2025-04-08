/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture Dado_Ocho;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model BuggattilLanta_M;
Model Buggatticapo_M;
Model Buggatti_M;
Model Lamp;
Model medal;
Model herc;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
//PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights1[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CrearDado_OCHO()
{
	// Definición de los índices (8 caras * 3 vértices = 24 índices)
	unsigned int octahedron_indices[] = {
		 0,  1,  2,   // Cara 1 (T1)
		 3,  4,  5,   // Cara 2 (T2)
		 6,  7,  8,   // Cara 3 (T3)
		 9, 10, 11,   // Cara 4 (T4)
		12, 13, 14,   // Cara 5 (T5)
		15, 16, 17,   // Cara 6 (T6)
		18, 19, 20,   // Cara 7 (T7)
		21, 22, 23    // Cara 8 (T8)
	};


	GLfloat octahedron_vertices[] = {

		// Cara 1
		 0.0f,   0.0f,  0.5f,    0.05f, 0.79f,   -1.0f,  -1.0f,  -1.0f,
		 0.5f,   0.0f,  0.0f,    0.23f, 0.79f,   -1.0f,  -1.0f,  -1.0f,
		 0.0f,   0.5f,  0.0f,    0.125f, 1.0f,   -1.0f,  -1.0f,  -1.0f,

		 // Cara 4
		  0.0f,   0.0f,  0.5f,    0.96f, 0.79f,    1.0f,  -1.0f,  -1.0f,
		  0.0f,   0.5f,  0.0f,    0.86f, 1.0f,     1.0f,  -1.0f,  -1.0f,
		 -0.5f,   0.0f,  0.0f,    0.75f, 0.79f,    1.0f,  -1.0f,  -1.0f,

		 // Cara 8
		  0.0f,   0.0f,  0.5f,    0.96f, 0.77f,    1.0f,   1.0f,  -1.0f,
		 -0.5f,   0.0f,  0.0f,    0.75f, 0.77f,    1.0f,   1.0f,  -1.0f,
		  0.0f,  -0.5f,  0.0f,    0.85f, 0.60f,    1.0f,   1.0f,  -1.0f,

		  // Cara 5
		   0.0f,   0.0f,  0.5f,    0.05f, 0.77f,   -1.0f,   1.0f,  -1.0f,
		   0.0f,  -0.5f,  0.0f,    0.125f, 0.60f,  -1.0f,   1.0f,  -1.0f,
		   0.5f,   0.0f,  0.0f,    0.23f, 0.77f,   -1.0f,   1.0f,  -1.0f,

		   // Cara 2
			0.0f,   0.0f, -0.5f,    0.47f, 0.79f,   -1.0f,  -1.0f,   1.0f,
			0.0f,   0.5f,  0.0f,    0.37f, 0.99f,   -1.0f,  -1.0f,   1.0f,
			0.5f,   0.0f,  0.0f,    0.28f, 0.79f,   -1.0f,  -1.0f,   1.0f,

			// Cara 3
			 0.0f,   0.0f, -0.5f,    0.51f, 0.79f,    1.0f,  -1.0f,   1.0f,
			-0.5f,   0.0f,  0.0f,    0.69f, 0.79f,    1.0f,  -1.0f,   1.0f,
			 0.0f,   0.5f,  0.0f,    0.615f, 1.0f,    1.0f,  -1.0f,   1.0f,

			 // Cara 7
			  0.0f,   0.0f, -0.5f,    0.51f, 0.78f,    1.0f,   1.0f,   1.0f,
			  0.0f,  -0.5f,  0.0f,    0.615f, 0.60f,   1.0f,   1.0f,   1.0f,
			 -0.5f,   0.0f,  0.0f,    0.69f, 0.78f,    1.0f,   1.0f,   1.0f,

			 // Cara 6
			  0.0f,   0.0f, -0.5f,    0.45f, 0.78f,   -1.0f,   1.0f,   1.0f,
			  0.5f,   0.0f,  0.0f,    0.28f, 0.78f,   -1.0f,   1.0f,   1.0f,
			  0.0f,  -0.5f,  0.0f,    0.37f, 0.60f,   -1.0f,   1.0f,   1.0f,


	};

	Mesh* dado_OCHO = new Mesh();
	dado_OCHO->CreateMesh(octahedron_vertices, octahedron_indices, 192, 24);
	meshList.push_back(dado_OCHO);

	calcAverageNormals(octahedron_indices, 24, octahedron_vertices, 192, 8, 5);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	CrearDado_OCHO();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	Dado_Ocho = Texture("Textures/cubocompleto.tga");
	Dado_Ocho.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	BuggattilLanta_M = Model();
	BuggattilLanta_M.LoadModel("Models/llantaOrigen.obj");
	Buggatticapo_M = Model();
	Buggatticapo_M.LoadModel("Models/cofretex.obj");
	Buggatti_M = Model();
	Buggatti_M.LoadModel("Models/panic1.obj");
	Lamp = Model();
	Lamp.LoadModel("Models/lamp_calle.obj");
	medal = Model();
	medal.LoadModel("Models/olympia.obj");
	herc = Model();
	herc.LoadModel("Models/Hercstatue.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	//unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
/*	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount++;*/

	// SE AGREGAN LOS ARREGLOS DE LUCES, CAA UNO CON SU RESPECTIVO COUNT 

	unsigned int pointLightCount_ARRAY1 = 0;

	pointLights1[0] = PointLight(1.0f, 0.843f, 0.0f,
		0.3f, 0.5f,
		7.70f, 2.3f, 6.0f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY1++;

	pointLights1[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.25f, 2.0f,
		7.70f, 11.70f, 7.800f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY1++;

	unsigned int pointLightCount_ARRAY2 = 0;

	pointLights2[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.25f, 2.0f,
		7.70f, 11.70f, 7.800f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;

	pointLights2[1] = PointLight(1.0f, 0.843f, 0.0f,
		0.3f, 0.5f,
		7.70f, 2.3f, 6.0f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;



	/*pointLights1[1] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 10.0f,
		-11.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY1++;

	pointLights1[2] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 10.0f,
		-16.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY1++;

	pointLights1[3] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 10.0f,
		-24.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY1++;

	unsigned int pointLightCount_ARRAY2 = 0;

	// array2
	pointLights2[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 10.0f,
		-6.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;

	pointLights2[1] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 10.0f,
		-11.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;

	pointLights2[2] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 10.0f,
		-16.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;

	pointLights2[3] = PointLight(1.0f, 0.0f, 0.0f,
		1.0f, 10.0f,
		-24.0f, 1.5f, 1.5f,
		0.0f, 0.1f, 0.5f);
	pointLightCount_ARRAY2++;*/


	unsigned int spotLightCount = 0;
	unsigned int spotLightCount2 = 0;

	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	spotLights2[0] = spotLights[0]; // Copia la luz de la linterna al segundo arreglo
	spotLightCount2++;

	//helicoptero
	spotLights[1] = SpotLight(0.8f, 0.8f, 0.0f, // color    // Color amarillo chillón (RGB)
		0.5f, 0.1f,           // aIntensity y dIntensity
		0.0f, 35.0f, 0.0f,     // Posición del helicóptero en el centro y a 5 unidades de altura
		0.0f, -10.0f, 0.0f,    // La luz apunta hacia abajo (eje Y negativo)
		1.0f, 0.0f, 0.00f,
		30.0f);
	spotLightCount++;
	spotLights2[1] = spotLights[1]; // Copia la luz del helicóptero al segundo arreglo
	spotLightCount2++;
	//luz de la estatua
	spotLights[2] = SpotLight(1.0f, 0.85f, 0.65f, // color
		0.5f, 2.0f,
		0.0f, 1.5f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	spotLights2[2] = spotLights[2];
	spotLightCount2++;

	//auto
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f, // color
		0.5f, 2.0f,
		2.1f, 0.6f, 3.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights2[3] = SpotLight(0.0f, 0.0f, 1.0f, // color
		0.5f, 2.0f,
		2.1f, 0.6f, -3.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount2++;

	spotLights[4] = SpotLight(0.0f, 0.0f, 1.0f, // color
		0.5f, 2.0f,
		2.1f, 0.6f, -3.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights2[4] = SpotLight(0.0f, 0.0f, 1.0f, // color
		0.5f, 2.0f,
		2.1f, 0.6f, 3.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount2++;





	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a a cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		spotLights2[0].SetFlash(lowerLight, camera.getCameraDirection());


		float a1 = mainWindow.getarticulacion1();
		float a2 = mainWindow.getarticulacion2();
		//glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, 0.1f) * (a1 + a2);

		if (mainWindow.getvalor3() == 1.0f)
		{
			glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, 0.1f) * (a1 + a2);
			spotLights[3].SetFlash(lightPosition, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (mainWindow.getvalor3() == 0.0f)
		{
			glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, 0.1f) * (a1 + a2);
			spotLights2[3].SetFlash(lightPosition, glm::vec3(0.0f, 0.0f, -1.0f));
		}
		

		float a3 = mainWindow.getarticulacion3();
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(a3), glm::vec3(0.1f, 0.0f, 0.0f));
		glm::vec3 lightPosition3 = glm::vec3(0.0f, 0.0f, 1.0f) + glm::vec3(0.0f, 0.0f, 0.1f) * (a1 + a2);
		glm::vec3 lightDirection3 = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 0.1f, 0.0f)));
		spotLights[2].SetFlash(lightPosition3, lightDirection3);
		spotLights2[2].SetFlash(lightPosition3, lightDirection3);

		float a4 = mainWindow.getarticulacion4(), a5 = mainWindow.getarticulacion5();
		glm::vec3 lightPosition2 = glm::vec3(0.0f, 0.0f, 1.0f) + glm::vec3(0.0f, 0.0f, 1.0f) * (a4 + a5);
		spotLights[1].SetFlash(lightPosition2, glm::vec3(0.0f, -5.0f, 0.0f));
		spotLights2[1].SetFlash(lightPosition2, glm::vec3(0.0f, -5.0f, 0.0f));

	
		shaderList[0].SetDirectionalLight(&mainLight);


		if (mainWindow.getvalor3() == 1.0f)
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount-1);
		}
		else if (mainWindow.getvalor3() == 0.0f)
		{
			shaderList[0].SetSpotLights(spotLights2, spotLightCount2-1);
		}
		else
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}








		int state = 0;

		if (mainWindow.getvalor() == 0.0f && mainWindow.getvalor2() == 0.0f)
			state = 0;
		else if (mainWindow.getvalor() == 1.0f && mainWindow.getvalor2() == 0.0f)
			state = 1;
		else if (mainWindow.getvalor() == 0.0f && mainWindow.getvalor2() == 1.0f)
			state = 2;
		else if (mainWindow.getvalor() == 1.0f && mainWindow.getvalor2() == 1.0f)
			state = 3;

		switch (state)
		{
		case 0:
			// Situación inicial: ambas luces apagadas (o en estado base)
			shaderList[0].SetPointLights(pointLights1, pointLightCount_ARRAY1);
			break;
		case 1:
			// Sólo la medalla encendida
			shaderList[0].SetPointLights(pointLights1, pointLightCount_ARRAY1 - 1);
			break;
		case 2:
			// Sólo la luz del faro encendida
			shaderList[0].SetPointLights(pointLights2, pointLightCount_ARRAY2 - 1);
			break;
		case 3:
			// Ambas luces encendidas / o apagadas según la lógica que desees
				shaderList[0].SetPointLights(pointLights1, pointLightCount_ARRAY1 - 2);
			break;
		default:
			break;
		}



		// PARA NO PRENDER UNA LUZ LO UNICO QUE SE PUEDE HACER ES NO MANDARLA AL SHADER 
		// SHADER -1 -2 -3
		// TENDRIA UN TOTAL DE 16 COMBINACIONES DON CUATRO ARREGLOS 
		// HAY QUE CREAR HASTA DIECISEIS ARREGLOS POSIBLES

		/*if (mainWindow.getvalor() == 0.0f)
		{//h
			shaderList[0].SetPointLights(pointLights1, pointLightCount_ARRAY1);
		}
		else
		{
			if (mainWindow.getvalor() == 1.0f)
			{//h
				shaderList[0].SetPointLights(pointLights2, pointLightCount_ARRAY2);
			};

		}
		shaderList[0].SetSpotLights(spotLights, spotLightCount);*/

		




		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();
		//carroceria del automovil

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f) * mainWindow.getarticulacion1()); //traslacion del cuerpo adelante
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f) * mainWindow.getarticulacion2()); //traslacion del cuerpo hacia atras
	


		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Buggatti_M.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(2.5f, 0.7f, 2.825f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuggattilLanta_M.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-2.5f, 0.7f, 2.825f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuggattilLanta_M.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-2.5f, 0.7f, -4.975f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuggattilLanta_M.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(2.5f, 0.7f, -4.975f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BuggattilLanta_M.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.3f, 1.6f, 2.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.1f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Buggatticapo_M.RenderModel();
	

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.0f, -0.7f,8.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lamp.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.0f, 5.0f,6.5f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		medal.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 20.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		herc.RenderModel();



		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Dado_Ocho.UseTexture();
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f) * mainWindow.getarticulacion4()); //traslacion del cuerpo adelante
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f) * mainWindow.getarticulacion5()); //traslacion del cuerpo hacia atras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
