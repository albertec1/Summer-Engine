#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleMenu.h"

#include "MeshImporter.h"
#include "TextureImporter.h"

#include <iostream>

#include "ImGui.h"
#include "OpenGl.h"
#include "Assimp.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	gl_context = SDL_GL_CreateContext(App->window->window);
	if(gl_context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	GLenum error = glewInit();

	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}

	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
		LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error..1
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error..2
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error..3
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		lights[0].Active(true);
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Stream log messages to Debug window
	//struct aiLogStream stream;
	//stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	//aiAttachLogStream(&stream);

	//temporary till i figure out
	//  where to put it	
	//LoadModel("Assets/Models/BakerHouse.fbx");
	
	// Modern OpenGL cube [indices]////
	PrimitiveCube();
	//Create checker texture
	CreateCheckerImage();
	
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	//DIRECT MODE TRIANGLE EXAMPLE//  //TODO: move into a separate method
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);
	
	glTexCoord2f(0.0f, 0.f);	glVertex3f(-2.f, 1.f, 0.f);
	glTexCoord2f(1.f, 0.f);		glVertex3f(2.f, 1.f, 0.f);
	glTexCoord2f(0.f, 1.f);		glVertex3f(-2.f, 4.f, 0.f);
	
	glTexCoord2f(0.f, 1.f);		glVertex3f(-2.f, 4.f, 0.f);
	glTexCoord2f(1.f, 0.f);		glVertex3f(2.f, 1.f, 0.f);
	glTexCoord2f(1.f, 1.f);		glVertex3f(2.f, 4.f, 0.f);
	
	glEnd();
	glFlush();
	
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	
	// Modern OpenGL square render////////////////////
	// Render using indices///
	glBindTexture(GL_TEXTURE_2D, TEX);
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOG("Error Drawimg Elements! %s\n", gluErrorString(error));
	}
	glBindVertexArray(0);
	//DrawAllMeshes();
	
	//ImGui Render
	App->menu->Render();
	
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	// detach log stream
	//aiDetachAllLogStreams();

	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(gl_context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//MeshInfo* ModuleRenderer3D::LoadModel(const std::string& filename)
//{
//	MeshImporter imp;
//	MeshInfo* temp = imp.LoadScene(filename);
//	/*LoadBuffer(temp, temp->vertices, temp->indices);
//	mesh_array.push_back(temp);*/
//	return temp;
//}

//void ModuleRenderer3D::LoadBuffer(MeshInfo* mesh, float* vertices, uint* indices)
//{
//	//Create a vertex array object which will hold all buffer objects
//	glGenVertexArrays(1, &mesh->VAO);
//	glBindVertexArray(mesh->VAO);
//
//	glGenBuffers(1, (GLuint*)&mesh->b_vertices);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->b_vertices);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 3, vertices, GL_STATIC_DRAW);
//	
//	glGenBuffers(1, (GLuint*)&mesh->b_indices);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->b_indices);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, indices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer((uint)BufferIndex::VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray((uint)BufferIndex::VERTICES);
//
//	if (mesh->num_normals > 0)
//	{
//		glGenBuffers(1, (GLuint*)&mesh->b_normals);
//		glBindBuffer(GL_ARRAY_BUFFER, mesh->b_normals);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);
//	
//		glVertexAttribPointer((uint)BufferIndex::NORMALS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray((uint)BufferIndex::NORMALS);
//	}
//
//	if (mesh->num_tex_coords > 0) // tex_coords ///////////
//	{
//		glGenBuffers(1, (GLuint*)&mesh->b_texture_coords);
//		glBindBuffer(GL_ARRAY_BUFFER, mesh->b_texture_coords);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_tex_coords * 2, mesh->texture_coords, GL_STATIC_DRAW);
//	
//		glVertexAttribPointer((uint)BufferIndex::TEX_COORDINATES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);
//	}
//	
//	glBindVertexArray(0);
//
///*	glGenBuffers(1, (GLuint*)&mesh->b_vertices);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->b_vertices);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 8, &vertices, GL_STATIC_DRAW);
//	
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const GLvoid*)12);
//	
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float), (const GLvoid*)20);
//*/
//	
//}

//void ModuleRenderer3D::DrawMesh(MeshInfo* mesh)
//{	
//	glEnableVertexAttribArray((uint)BufferIndex::VERTICES);
//	glEnableVertexAttribArray((uint)BufferIndex::NORMALS);
//	glEnableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);
//
//	glBindVertexArray(mesh->VAO);	
//	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
//	GLenum error = glGetError();
//	if (error != GL_NO_ERROR)
//	{
//		LOG("Error Drawimg Elements! %s\n", gluErrorString(error));
//	}
//	glBindVertexArray(0);
//
//	glDisableVertexAttribArray((uint)BufferIndex::VERTICES);
//	glDisableVertexAttribArray((uint)BufferIndex::NORMALS);
//	glDisableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);
//
//}
//
//void ModuleRenderer3D::DrawAllMeshes()
//{	
//	for (int m = 0; m < mesh_array.size(); m++)
//	{
//		DrawMesh(mesh_array[m]);
//	}
//}
//           
//void ModuleRenderer3D::LoadTexture(TextureInfo* tex)
//{
//	
//}

void ModuleRenderer3D::CreateCheckerImage()
{
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255; //debug this line to see what it does
			checkerImage[i][j][0] = (GLubyte)c;		//R
			checkerImage[i][j][1] = (GLubyte)c;		//G
			checkerImage[i][j][2] = (GLubyte)c;		//B
			checkerImage[i][j][3] = (GLubyte)255;	//A
		}
	}

	/*TextureInfo* tex = new TextureInfo;
	tex->path = "Checker";
	tex->tex_width = CHECKERS_WIDTH;
	tex->tex_height = CHECKERS_HEIGHT;*/

	//Load Texture info and parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_2D, TEX);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Send checkerImage to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer3D::PrimitiveCube()
{
	float vertices[] = {

		/*-0.5f, -0.5f, -0.5f,	
		 0.5f, -0.5f, -0.5f,	
		 0.5f,  0.5f, -0.5f,	
		-0.5f,  0.5f, -0.5f,	*/
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f, //0
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,	//1	
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,	//2
		-0.5f, -0.5f,  0.5f, 	0.0f, 0.0f  //3
	};

	//float texCoords[] = {
	//	1.0f, 0.0f, //0
	//	1.0f, 1.0f,	//1
	//	0.0f, 0.0f, //3
	//	
	//	1.0f, 1.0f,	//1
	//	0.0f, 1.0f,	//2
	//	0.0f, 0.0f  //3
	//};

	uint indices[] = {
		//0, 2, 1,	//BACK FACE
		//0, 3, 2,
		//1, 2, 4,	//RIGHT FACE
		//4, 2, 5,
		//2, 6, 5,	//TOP FACE
		//2, 3, 6,
		//1, 7, 0,	//BOTTOM FACE
		//1, 4, 7,
		//6, 3, 0,	//LEFT FACE
		//7, 6, 0,
		//4, 5, 7,	//FRONT FACE
		//5, 6, 7,
		0, 1, 3,
		1, 2, 3,
	};
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);	//stores calls to glGenBuffers when the target is GL_ELEMENT_ARRAY_BUFFER

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*glGenBuffers(1, &TCoordsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, TCoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);*/

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/*stackoverflow.com/questions/4983532/what-are-the-texture-coordinates-for-a-cube-in-opengl*/
}

void ModuleRenderer3D::SetDepthBufferEnabled()
{
	if (depthEnabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

bool ModuleRenderer3D::GetVSync() const
{
	return vsync;
}

void ModuleRenderer3D::SetVSync(bool vsync)
{
	if (this->vsync != vsync)
	{
		this->vsync = vsync;
		if (SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}
}