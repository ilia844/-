#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>


using namespace sf;

class Player {
private:
	float x, y;
public:
	float w, h, dx, dy, speed;
	int dir, playerScore, health;
	bool life;
	String File;
	Image image;
	Texture texture;
	Sprite sprite;

	Player(String F, float X, float Y, float W, float H) {
		File = F;
		w = W; h = H;
		dir = 0; speed = 0; playerScore = 0;
		health = 100; life = true;
		image.loadFromFile("Images/" + File);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		x = X; y = Y;
		sprite.setTextureRect(IntRect(0, 0, w, h));
//		sprite.setOrigin(w / 2, h / 2);
	}

	void update(float time) {
		switch (dir) {
		case 0: dx = speed; dy = 0; break;
		case 1: dx = -speed; dy = 0; break;
		case 2: dx = 0; dy = -speed; break;
		case 3: dx = 0; dy = speed; break;
		}

		x += dx * time;
		y += dy * time;

		speed = 0;
		if (health == 0) life = false;

		sprite.setPosition(x, y);
		interactionWithMap();
		
	}

	void interactionWithMap() {
		for (int i = (y + 4) / 32; i < (y + h - 4) / 32; i++)
			for (int j = (x + 8) / 32; j < (x + w - 8) / 32; j++) {
				if (tileMap[i][j] == '0') {
					if (dy > 0) {
						y = i * 32 - h + 4;
					}
					if (dy < 0) {
						y = i * 32 + 32 - 4;
					}
					if (dx > 0) {
						x = j * 32 - w + 8;
					}
					if (dx < 0) {
						x = j * 32 + 32 - 8;
					}
				}

				if (tileMap[i][j] == 's') {
					playerScore++;
					tileMap[i][j] = ' ';
				}

				if (tileMap[i][j] == 'f') {
					health -= 20;
					if (health < 0)	health = 0;
					tileMap[i][j] = ' ';
				}

				if (tileMap[i][j] == 'h') {
					health += 10;
					if (health > 100) health = 100;
					tileMap[i][j] = ' ';
				}
			}
	}

	float getPlayerCoordinateX() {
		return x;
	}

	float getPlayerCoordinateY() {
		return y;
	}
};


int main()
{
	RenderWindow window(sf::VideoMode(640, 480), "Lesson 1. kychka-pc.ru");
	view.reset(FloatRect(0, 0, 640, 480));
//////Карта//////
	Image map_img;
	map_img.loadFromFile("Images/newmap.png");
	Texture map_texture;
	map_texture.loadFromImage(map_img);
	Sprite s_map;
	s_map.setTexture(map_texture);

//////Fonts//////
	Font font;
	font.loadFromFile("fonts/CyrilicOld.TTF");
	Text text("", font, 25);
	text.setStyle(Text::Bold);
	text.setOutlineColor(Color::Black);
	text.setFillColor(Color::Green);
//	text.setColor(Color::Red);

/////Переменная для вывода в text//////
	std::stringstream playerScoreString;
	std::stringstream playerHealth;
	std::stringstream playerGameTime;

/*	Image heroimage;
	heroimage.loadFromFile("Images/hero.png");

	Texture herotexture;
	herotexture.loadFromImage(heroimage);

	Sprite herosprite;
	herosprite.setTexture(herotexture);
	herosprite.setTextureRect(IntRect(0, 96, 32, 48));  f'fhf,jsi  43.24 = ie{x6}{hf6}' 
	herosprite.setPosition(50, 25);
*/
	Clock clock;
	Clock gameTimeClock;
	int gameTime = 0;

	float currentFrame = 0;

	Player hero("hero.png", 250, 250, 32, 48);

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;

		if (hero.life)
			gameTime = gameTimeClock.getElapsedTime().asSeconds();
		else
			view.move(0.1, 0);

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		if (hero.life) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { hero.sprite.setScale(1, 1); };
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) { hero.sprite.setScale(4, 2); };

			if ((Keyboard::isKeyPressed(Keyboard::Left)) || (Keyboard::isKeyPressed(Keyboard::A))) {
				hero.dir = 1; hero.speed = 0.1;
				currentFrame += 0.005 * time;
				if (currentFrame > 4) currentFrame = 0;
				hero.sprite.setTextureRect(IntRect(32 * int(currentFrame), 48, 32, 48));
				getPlayerCoordinateForView(hero.getPlayerCoordinateX(), hero.getPlayerCoordinateY());
			}
			if ((Keyboard::isKeyPressed(Keyboard::Right)) || (Keyboard::isKeyPressed(Keyboard::D))) {
				hero.dir = 0; hero.speed = 0.1;
				currentFrame += 0.005 * time;
				if (currentFrame > 4) currentFrame = 0;
				hero.sprite.setTextureRect(IntRect(32 * int(currentFrame), 96, 32, 48));
				getPlayerCoordinateForView(hero.getPlayerCoordinateX(), hero.getPlayerCoordinateY());
				//			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 96, 32, 48));
				//			herosprite.move(0.1 * time, 0);
			}
			if ((Keyboard::isKeyPressed(Keyboard::Up)) || (Keyboard::isKeyPressed(Keyboard::W))) {
				hero.dir = 2; hero.speed = 0.1;
				currentFrame += 0.005 * time;
				if (currentFrame > 4) currentFrame = 0;
				hero.sprite.setTextureRect(IntRect(32 * int(currentFrame), 144, 32, 48));
				getPlayerCoordinateForView(hero.getPlayerCoordinateX(), hero.getPlayerCoordinateY());
				//			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 144, 32, 48));
				//			herosprite.move(0, -0.1 * time);
			}
			if ((Keyboard::isKeyPressed(Keyboard::Down)) || (Keyboard::isKeyPressed(Keyboard::S))) {
				hero.dir = 3; hero.speed = 0.1;
				currentFrame += 0.005 * time;
				if (currentFrame > 4) currentFrame = 0;
				hero.sprite.setTextureRect(IntRect(32 * int(currentFrame), 0, 32, 48));
				getPlayerCoordinateForView(hero.getPlayerCoordinateX(), hero.getPlayerCoordinateY());
				//			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 0, 32, 48));
				//			herosprite.move(0, 0.1 * time);
			}
		}

		hero.update(time);
		window.setView(view);

		window.clear();

////////Обновление рейтинга/////////

		playerScoreString.str(""); playerHealth.str(""); playerGameTime.str("");
		playerScoreString << hero.playerScore; playerHealth << hero.health; playerGameTime << gameTime;
		text.setString("Собрано камней: " + playerScoreString.str() + "\nЗдоровье: " + playerHealth.str() + "\nВремя жизни: " + playerGameTime.str());
		text.setPosition(view.getCenter().x - 300, view.getCenter().y - 200);

		/////Отрисовка карты/////
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++) {
				if (tileMap[i][j] == ' ') s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (tileMap[i][j] == 's') s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if (tileMap[i][j] == '0') s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if (tileMap[i][j] == 'f') s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if (tileMap[i][j] == 'h') s_map.setTextureRect(IntRect(128, 0, 32, 32));

				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}

		window.draw(hero.sprite);
		window.draw(text);
		window.display();
	}

	return 0;
}