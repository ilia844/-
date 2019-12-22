#include <SFML/Graphics.hpp>
using namespace sf;

View view;

void setPlayerCoordinateForView(float x, float y, int winX, int winY, int winW, int winH) { //функция для считывания координат игрока
	//if (((winX < winW / 4) || (winX > winW * 3 / 4)) && ((winY < winH / 3) || (winY > winH * 2 / 3))) { //Как определять координаты спрайта относительно окна

		float tempX = x, tempY = y;

		if (x < 400) tempX = 400;
		if (x > 1200) tempX = 1200;
		if (y < 300) tempY = 300;
		if (y > 564) tempY = 564;

		view.setCenter(tempX, tempY);

	//}
};


