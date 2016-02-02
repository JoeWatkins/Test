#pragma once
#include "../nclgl/OGLRenderer.h"

#include "RenderObject.h"

#include <vector>

using std::vector;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	~Renderer(void);

	virtual void	RenderScene();

	virtual void	Render(const RenderObject &o);

	virtual void	UpdateScene(float msec);

	void	AddRenderObject(RenderObject &r) {
		renderObjects.push_back(&r);
	}

	float value = 1.0f;

protected:

	vector<RenderObject*> renderObjects;
	GLuint loadTexture(const char* textIn);
	GLuint smileyTexture;
	GLuint noiseTexture;
	float timePeriod;
	float scale;
	float change = 1.0f;
	


};

