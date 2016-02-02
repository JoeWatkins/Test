/*
Class:OGLRenderer
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/

#pragma comment(lib, "opengl32.lib")	//The actual OGL Library

#ifdef _DEBUG
#pragma comment(lib, "glew32sd.lib")		//GLEW static library bit, so we don't use the dll any more
#else 
#pragma comment(lib, "glew32s.lib")		//GLEW static library bit, so we don't use the dll any more
#endif

#pragma comment(lib, "SOIL.lib")		//SOIL, used to load in images

#include "OGLRenderer.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

int	OGLRenderer::width	= 0;	//Render area width (not quite the same as window width)
int	OGLRenderer::height = 0;	//Render area height (not quite the same as window height)

void OGLRenderer::BasicResizeFunc(int x, int y) {
	OGLRenderer::width	= x;
	OGLRenderer::height = y;
	glViewport(0, 0, OGLRenderer::width, OGLRenderer::height);
}

/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
OGLRenderer::OGLRenderer(Window &window)	{
	init					= false;

	HWND windowHandle = window.GetHandle();

	// Did We Get A Device Context?
	if (!(deviceContext=GetDC(windowHandle)))		{					
		std::cout << "OGLRenderer::OGLRenderer(): Failed to create window!" << std::endl;
		return;
	}
	
	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion		= 1; 
   	pfd.dwFlags			= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
   	pfd.iPixelType		= PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
   	pfd.cColorBits		= 32;				//4 channels of 8 bits each!
   	pfd.cDepthBits		= 24;				//24 bit depth buffer
	pfd.cStencilBits	= 8;				//plus an 8 bit stencil buffer
   	pfd.iLayerType		= PFD_MAIN_PLANE;

	GLuint		PixelFormat;
	if (!(PixelFormat=ChoosePixelFormat(deviceContext,&pfd)))		{	// Did Windows Find A Matching Pixel Format for our PFD?
		std::cout << "OGLRenderer::OGLRenderer(): Failed to choose a pixel format!" << std::endl;
		return;
	}

	if(!SetPixelFormat(deviceContext,PixelFormat,&pfd))			{		// Are We Able To Set The Pixel Format?
		std::cout << "OGLRenderer::OGLRenderer(): Failed to set a pixel format!" << std::endl;
		return;
	}

	HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext=wglCreateContext(deviceContext)))				{	// Are We Able To get the temporary context?
		std::cout << "OGLRenderer::OGLRenderer(): Cannot create a temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if(!wglMakeCurrent(deviceContext,tempContext))					{	// Try To Activate The Rendering Context
		std::cout << "OGLRenderer::OGLRenderer(): Cannot set temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	//Now we have a temporary context, we can find out if we support OGL 3.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "3.2.0" (or greater!)
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	if(major < 3) {					//Graphics hardware does not support OGL 3! Erk...
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 3.x!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if(major == 3 && minor < 2) {	//Graphics hardware does not support ENOUGH of OGL 3! Erk...
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 3.2!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	//We do support OGL 3! Let's set it up...

	int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,	//TODO: Maybe lock this to 3? We might actually get an OpenGL 4.x context...
        WGL_CONTEXT_MINOR_VERSION_ARB, minor, 
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,		//We want everything OpenGL 3.2 provides...
		0					//That's enough attributes...
    };

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext,0, attribs);

	// Check for the context, and try to make it the current rendering context
	if(!renderContext || !wglMakeCurrent(deviceContext,renderContext))		{			
		std::cout << "OGLRenderer::OGLRenderer(): Cannot set OpenGL 3 context!" << std::endl;	//It's all gone wrong!
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	glewExperimental = GL_TRUE;	//This forces GLEW to give us function pointers for everything (gets around GLEW using 'old fashioned' methods
								//for determining whether a OGL context supports a particular function or not
	
	if (glewInit() != GLEW_OK) {	//Try to initialise GLEW
		std::cout << "OGLRenderer::OGLRenderer(): Cannot initialise GLEW!" << std::endl;	//It's all gone wrong!
		return;
	}
	//If we get this far, everything's going well!

#ifdef OPENGL_DEBUGGING
	if (major >= 4 && minor >= 0) {
		glDebugMessageCallback((GLDEBUGPROC)&OGLRenderer::DebugCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}
#endif

	glClearColor(0.2f,0.2f,0.2f,1.0f);			//When we clear the screen, we want it to be dark grey

	window.RegisterResizeCallback(BasicResizeFunc);//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)		
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)	{
	wglDeleteContext(renderContext);
}

/*
Returns TRUE if everything in the constructor has gone to plan.
Check this to end the application if necessary...
*/
bool OGLRenderer::HasInitialised() const{
	return init;
}

/*
Resizes the rendering area. Should only be called by the Window class!
Does lower bounds checking on input values, so should be reasonably safe
to call.
*/
void OGLRenderer::Resize(int x, int y)	{
	width	= max(x,1);	
	height	= max(y,1);
	glViewport(0,0,width,height);
}

/*
Swaps the buffers, ready for the next frame's rendering. Should be called
every frame, at the end of RenderScene(), or whereever appropriate for
your application.
*/
void OGLRenderer::SwapBuffers() {
	//We call the windows OS SwapBuffers on win32. Wrapping it in this 
	//function keeps all the tutorial code 100% cross-platform (kinda).
	::SwapBuffers(deviceContext);
}
/*
Used by some later tutorials when we want to have framerate-independent
updates on certain datatypes. Really, OGLRenderer should have its own
timePeriodr, so it can just sit and look after itself (potentially even in
another thread...), but it's fine for the tutorials.

STUDENTS: Don't put your entity update routine in this, or anything like
that - it's just asking for trouble! Strictly speaking, even the camera
shouldn't be in here...(I'm lazy)
*/
void OGLRenderer::UpdateScene(float msec)	{

}

/*
Updates the uniform matrices of the current shader. Assumes that
the shader has uniform matrices called modelMatrix, viewMatrix,
projMatrix, and textureMatrix. Updates them with the relevant
matrix data. Sanity checks currentShader, so is always safe to
call.
*/
void OGLRenderer::UpdateShaderMatrices(GLuint program)	{
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix")  , 1, false, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix")   , 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix")   , 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "textureMatrix"), 1, false, (float*)&textureMatrix);


}


//void OGLRenderer::SetCurrentShader(Shader*s) {
//	currentShader = s;
//
//	glUseProgram(s->GetProgram());
//}

void	OGLRenderer::ClearBuffers() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


#ifdef OPENGL_DEBUGGING
void OGLRenderer::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, void* userParam)	{

		string sourceName;
		string typeName;
		string severityName;

		switch(source) {
			case GL_DEBUG_SOURCE_API_ARB			: sourceName = "Source(OpenGL)"			;break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB	: sourceName = "Source(Window System)"	;break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)";break;
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB	: sourceName = "Source(Third Party)"	;break;
			case GL_DEBUG_SOURCE_APPLICATION_ARB	: sourceName = "Source(Application)"	;break;
			case GL_DEBUG_SOURCE_OTHER_ARB			: sourceName = "Source(Other)"			;break;
		}

		switch(type) {
			case GL_DEBUG_TYPE_ERROR_ARB				: typeName = "Type(Error)"					;break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB	: typeName = "Type(Deprecated Behaviour)"	;break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB	: typeName = "Type(Undefined Behaviour)"	;break;
			case GL_DEBUG_TYPE_PORTABILITY_ARB			: typeName = "Type(Portability)"			;break;
			case GL_DEBUG_TYPE_PERFORMANCE_ARB			: typeName = "Type(Performance)"			;break;
			case GL_DEBUG_TYPE_OTHER_ARB				: typeName = "Type(Other)"					;break;
		}

		switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH_ARB		: severityName = "Priority(High)"		;break;
			case GL_DEBUG_SEVERITY_MEDIUM_ARB	: severityName = "Priority(Medium)"		;break;
			case GL_DEBUG_SEVERITY_LOW_ARB		: severityName = "Priority(Low)"		;break;
		}

		cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << endl;
}
#endif