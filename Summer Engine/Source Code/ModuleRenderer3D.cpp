#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
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
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	//temporary till i figure ou where to put it
	//LoadModel("Assets/Models/BakerHouse.fbx");
	CreateCheckerImage();

// Modern OpenGL square ////////////////////

	//float positions[] = {
	//	-0.5f, -0.5f,
	//	 0.5f, -0.5f,
	//	 0.5f,  0.5f,
	//	//0.5f,  0.5f, 3RD VERTEX REPEATED
	//	-0.5f,  0.5f,
	//	//-0.5f, -0.5f 4TH VERTEX REPEATED
	//};

	//uint indices[] = {
	//	0, 1, 2,
	//	2, 3, 0
	//};

	//uint buffer;
	//glGenBuffers(1, &buffer);
	//glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, positions, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	//
	//uint ibo; //index buffer object
	//glGenBuffers(1, &ibo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, indices, GL_STATIC_DRAW);


	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	//recalculate projection matrix and set up the viewport if the window resizes?

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

	//DIRECT MODE TRIANGLE EXAMPLE//
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);
	
	glTexCoord2f(0.0f, 0.f);
	glVertex3f(-5.f, 1.f, 0.f);

	glTexCoord2f(1.f, 0.f);
	glVertex3f(5.f, 1.f, 0.f);
	
	glTexCoord2f(0.5f, 1.f);
	glVertex3f(0.f, 6.f, 0.f);
	
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
	
	//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	/*GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOG("Error Drawimg Elements! %s\n", gluErrorString(error));
	}*/


	DrawAllMeshes();
	//ImGui Render
	//App->menu->Render();
	
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

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

MeshInfo* ModuleRenderer3D::LoadModel(const std::string& filename)
{
	MeshImporter imp;
	MeshInfo* temp = imp.LoadScene(filename);
	LoadBuffer(temp, temp->vertices, temp->indices);
	mesh_array.push_back(temp);
	return temp;
}

void ModuleRenderer3D::LoadBuffer(MeshInfo* mesh, float* vertices, uint* indices)
{
	//Create a vertex array object which will hold all buffer objects
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, (GLuint*)&mesh->b_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->b_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 3, vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, (GLuint*)&mesh->b_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->b_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer((uint)BufferIndex::VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray((uint)BufferIndex::VERTICES);

	if (mesh->num_normals > 0)
	{
		glGenBuffers(1, (GLuint*)&mesh->b_normals);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->b_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);
	
		glVertexAttribPointer((uint)BufferIndex::NORMALS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray((uint)BufferIndex::NORMALS);
	}

	if (mesh->num_tex_coords > 0) // tex_coords ///////////
	{
		glGenBuffers(1, (GLuint*)&mesh->b_texture_coords);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->b_texture_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_tex_coords * 2, mesh->texture_coords, GL_STATIC_DRAW);
	
		glVertexAttribPointer((uint)BufferIndex::TEX_COORDINATES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);
	}
	
	glBindVertexArray(0);

/*	glGenBuffers(1, (GLuint*)&mesh->b_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->b_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 8, &vertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const GLvoid*)12);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float), (const GLvoid*)20);
*/
	
}

void ModuleRenderer3D::DrawMesh(MeshInfo* mesh)
{	
	glEnableVertexAttribArray((uint)BufferIndex::VERTICES);
	glEnableVertexAttribArray((uint)BufferIndex::NORMALS);
	glEnableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);

	glBindVertexArray(mesh->VAO);	
	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOG("Error Drawimg Elements! %s\n", gluErrorString(error));
	}
	glBindVertexArray(0);

	glDisableVertexAttribArray((uint)BufferIndex::VERTICES);
	glDisableVertexAttribArray((uint)BufferIndex::NORMALS);
	glDisableVertexAttribArray((uint)BufferIndex::TEX_COORDINATES);

}

void ModuleRenderer3D::DrawAllMeshes()
{	
	for (int m = 0; m < mesh_array.size(); m++)
	{
		DrawMesh(mesh_array[m]);
	}
}
           
void ModuleRenderer3D::LoadTexture(TextureInfo* tex)
{
	
}

TextureInfo* ModuleRenderer3D::CreateCheckerImage() const
{
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	TextureInfo* tex = new TextureInfo;
	tex->path = "Checker";
	tex->tex_width = CHECKERS_WIDTH;
	tex->tex_height = CHECKERS_HEIGHT;

	//Load Texture info and parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex->tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex->tex_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Send checkerImage to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->tex_width, tex->tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	return tex;
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