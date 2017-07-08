#pragma once

#include <Vector.h>
#include <Matrix.h>

#define HAVE_OPENGL 1
#define HAVE_VULKAN 0
#define HAVE_GL_GLCOREARB_H 1

#include "Rendering/Rendering.h"

class Scene;

class RendererWrapper
{
public:

	//
	// Constructor
	RendererWrapper		(void);
	~RendererWrapper	(void);

	//
	// ...
	bool init			(void);
	bool release		(void);

	//
	// ...
	void	onReady					(void);
	void	onResize				(int width, int height);
	void	onUpdate				(const mat4x4 & mView, const vec4 & clearColor);

	//
	// ...
	void	initQueue				(const char * szFilename);
	void	releaseQueue			(void);

	//
	// ...
	Scene & getScene(void);

	//
	// ...
	unsigned int /*GLuint*/ GetRenderTexture(const char * name) const;

	void SetDefaultFramebuffer(unsigned int /*GLuint*/ framebuffer);
	void SetPickBufferFramebuffer(unsigned int /*GLuint*/ framebuffer);

	//
	// ...
	const mat4x4 & getProjection(void) const;

};
