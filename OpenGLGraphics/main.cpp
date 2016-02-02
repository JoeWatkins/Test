#include "Renderer.h"
#include "RenderObject.h"

#pragma comment(lib, "nclgl.lib")

void main(void) {

	Window w = Window(800, 600);
	Renderer r(w);

	Mesh*	m	= Mesh::LoadMeshFile("cube.asciimesh");
	Shader* s	= new Shader("basicvert.glsl", "basicfrag.glsl");

	if (s->UsingDefaultShader()) {
		cout << "Warning: Using default shader! Your shader probably hasn't worked..." << endl;
		cout << "Press any key to continue." << endl;
		std::cin.get();
	}


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	RenderObject o(m,s);
	o.SetModelMatrix(Matrix4::Translation(Vector3(0,0,-10)) * Matrix4::Scale(Vector3(1,1,1)) * Matrix4::Rotation(45 , Vector3(1,0,0)));
	r.AddRenderObject(o);

	r.SetProjectionMatrix(Matrix4::Perspective(1, 100, 1.33f, 45.0f));

	r.SetViewMatrix(Matrix4::BuildViewMatrix(Vector3(0, 0, 0), Vector3(0, 0, -10)));

	while(w.UpdateWindow()) {
		float msec = w.GetTimer()->GetTimedMS();

		o.SetModelMatrix(o.GetModelMatrix() * Matrix4::Rotation(0.1f * msec,Vector3(0,0,1)));
		if (Keyboard::KeyDown(KEY_1)) {
			delete s;
			s = new Shader("scale_vert.glsl","basicfrag.glsl");

			o = RenderObject (m, s);
			o.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -10)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(45, Vector3(1, 0, 0)));
			r.value = 1.0;
		}
		if (Keyboard::KeyDown(KEY_2)) {
			delete s;
			s = new Shader("basicvert.glsl", "texture_frag.glsl");
			 o = RenderObject (m, s);
			 o.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -10)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(45, Vector3(1, 0, 0)));
			r.value = 1.0;
		}
		if (Keyboard::KeyDown(KEY_3)) {
			delete s;
			s = new Shader("basicvert.glsl", "alpha_frag.glsl");
			o = RenderObject (m, s);
			o.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -10)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(45, Vector3(1, 0, 0)));
			r.value = 1.0;
		}
		if (Keyboard::KeyDown(KEY_4)) {
			delete s;
			s = new Shader("basicvert.glsl",  "basicfrag.glsl", "geom_vert.glsl");
			o = RenderObject(m, s);
			o.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -10)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(45, Vector3(1, 0, 0)));
			r.value = 1.0;
		}



		r.UpdateScene(msec);
		r.ClearBuffers();
		r.RenderScene();
		r.SwapBuffers();
	}
	delete m;
	delete s;
}