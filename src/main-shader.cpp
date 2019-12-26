// Copyright Mattias Larsson Sköld
#include "matgui/window.h"
#include "matgui/application.h"
#include "matgui/matgl.h"
#include <vector>
#include <cmath>

using namespace std;
using namespace GL;
using namespace MatGui;

namespace plainShader {

const string vertex =
		R"_(
#version 330 core

in vec4 position;
out vec2 fPos;
uniform float scale;

void main() {
	gl_Position = position;
//	fPos = vec2(position.x / 2 + .5, position.y / 2 + .5);
	fPos = position.xy * 2 * scale;
}

)_";

const string fragment =
		R"_(
#version 330 core

#define product(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)

in vec2 fPos;
out vec4 fragColor;

void main() {
	vec2 c = fPos;
	float value = 0;
	vec2 z = vec2(0, 0);

	for (int i = 0; i < 100; ++i) {
		z = product(z, z) + c;
		if (dot(z, z) > 4) {
			value = float(i) / 100;
		}
	}
	
//	fragColor = vec4(fPos.xy, 1, 1);
	fragColor = vec4(value, sqrt(value), 0, 1);
}

)_";

} //Namespace


vector<float> objectVertices = {
		-1, -1, 0,
		 1, -1, 0,
		-1,  1, 0,
		 1,  1, 0,
};

vector<GLuint> indices = {
		0, 1, 2,
		1, 2, 3
};

int main(int argc, char **argv) {
	int width = 800, height = 600;
	Application app(argc, argv);
	Window window("Mandelbrot - drawn with shaders", width, height);

	ShaderProgram shader(plainShader::vertex, plainShader::fragment);
	GL::VertexArrayObject vao;
	GL::VertexBufferObject ObjectVertexBuffer(objectVertices, 0, 3);
	GL::VertexBufferObject ObjectElementBuffer(indices);

	vao.unbind();

	auto scaleUniform = shader.getUniform("scale");

	float scale = 1;

	window.scroll.connect([&scale](View::ScrollArgument arg) {
		scale *= pow(2, -arg.y / 10);
	});

	window.frameUpdate.connect([&]() {
		vao.bind();

		shader.use();

		glUniform1f(scaleUniform, scale);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	});

	app.mainLoop();
}

