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
uniform double x;
uniform double y;

void main() {
	gl_Position = position;
	fPos = position.xy * 2 * scale + vec2(x, y);
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
	auto xUniform = shader.getUniform("x");
	auto yUniform = shader.getUniform("y");

	float scale = 1;
	double centerX = 0, centerY = 0;

	double mouseX, mouseY;

	window.pointerMoved.connect([&](View::PointerArgument arg) {
		std::tie(mouseX, mouseY) = arg;
	});

	window.scroll.connect([&](View::ScrollArgument arg) {
		scale *= pow(2, -arg.y / 10);

		auto x = mouseX / window.width() * 2 - 1;
		auto y = -(mouseY / window.height() * 2 - 1);

		centerX += x * scale * .1 * arg.y;
		centerY += y * scale * .1 * arg.y;
	});

	window.frameUpdate.connect([&]() {
		vao.bind();

		shader.use();

		glUniform1f(scaleUniform, scale);
		glUniform1d(xUniform, centerX);
		glUniform1d(yUniform, centerY);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	});

	app.mainLoop();
}

