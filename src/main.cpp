// Copyright Mattias Larsson Sköld 2019

#include "matgui/application.h"
#include "matgui/window.h"
#include "matgui/texture.h"

#include <iostream>
#include <vector>
#include <complex>

using namespace std;
using namespace MatGui;

int main(int argc, char **argv) {
	Application app(argc, argv);
	Application::ContinuousUpdates(false);

	Window window("Mandelbrot", 1000, 1000);

	window.style.fill.color(.1, 0, 0);
	window.updateStyle();

	Texture texture;

	auto mb = [](double x, double y) {
		complex<double> z = {};
		complex<double> c = {x-.5, y-.5};
		c *= 4;

		const int count = 20;

		for (int i = 0; i < count; ++i) {
			z = z * z + c;
			if (norm(z) > norm(2)) {
				double val = (double) i / count;
				return sqrt(val);
			}
		}
		return 0.;
	};

	const size_t width = 1000, height = 1000;

	vector<float> data(width * height);

	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			data.at(x + y * width) = mb(1. / width * x, 1. / height * y);
		}
	}

	texture.createGrayscale(data, width, height, "hej");

	window.frameUpdate.connect([&]() {
		drawTextureRect(
				{window.width() / 2, window.height() / 2},
				0,
				window.width(),
				window.height(),
				texture,
				DrawStyle::CenterOrigo);
	});

	app.mainLoop();

	return 0;
}

