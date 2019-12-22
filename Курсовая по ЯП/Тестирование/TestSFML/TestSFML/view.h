#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void getPlayerCoordinateForView(float x, float y) {
	float tempX = x, tempY = y;

	if (x < 320) tempX = 320;
	if (y < 240) tempY = 240;
	if (y > 240) tempY = 240;
 
	view.setCenter(tempX, tempY);
};