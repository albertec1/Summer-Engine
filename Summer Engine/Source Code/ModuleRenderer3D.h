#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "MeshImporter.h"
#include <vector>
#include <string>

#define MAX_LIGHTS 8

class TextureInfo;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
//	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	MeshInfo* LoadModel(const std::string& filename);
	TextureInfo* CreateCheckerImage() const;

	void SetDepthBufferEnabled();
	bool GetVSync() const;
	void SetVSync(bool vsync);

	
	
public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext gl_context;

	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	std::vector<MeshInfo*> mesh_array;

	TextureInfo* CheckerTexture;

	bool depthEnabled = true;
	bool wireframeMode = false;
	bool vsync = true;

private:

	void LoadBuffer(MeshInfo* mesh, float* vertices, uint* indices);
	void LoadTexture(TextureInfo* tex);
	void DrawMesh(MeshInfo* mesh);
	void DrawAllMeshes();

 ////Testinng variables/////
	//float *positions;
	//unsigned int buffer;

};