#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "view.h"
//#include "map.h"
#include "level.h"
#include <vector>
#include <list>
#include <ctime>

using namespace sf;

class Entity {
public:
	std::vector<Object> obj;
	float x, y, dx, dy, speed, moveTimer, currentFrame;
	int w, h, health;
	bool life, isMove, onGround, inWater;
	Texture texture;
	Sprite sprite;
	String name; //классы врагов будут различаться по именам

	Entity(Image& image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0, currentFrame;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false; inWater = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}

	virtual void update(float time) = 0;
};

class Player : public Entity{
public:
	int playerScore;
	enum {left, right, up, down, jump, stay} state;
	bool canJump;

	Player(Image &image, String Name, Level &lev, float X, float Y, float W, float H): Entity(image, Name, X, Y, W, H) {
		playerScore = 0;
		state = stay; canJump = false;
		obj = lev.GetAllObjects();
		if (Name == "Player1") {
			sprite.setTextureRect((IntRect(255, 26, W, H)));
		}
	}

	void control() {
		
		if (Keyboard::isKeyPressed(Keyboard::A)) {
			state = left;
			speed = 0.1;
			if (inWater) { speed /= 3; }
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) {
			state = right;
			speed = 0.1;
			if (inWater) { speed /= 3; }
		}
		//if ((Keyboard::isKeyPressed(Keyboard::W)) && (onGround)) {
		if (Keyboard::isKeyPressed(Keyboard::W)) {
			if (onGround) {
				dy = -0.6;
				onGround = false;
			}
			if (inWater) {
				state = up;
				speed = 0.1 / 3;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) {
			state = down;
		}
		if (Keyboard::isKeyPressed(Keyboard::Space)) {
			if (canJump) {
				state = jump;
				dy = -0.3;
			}
		}
	}

	void checkCollisionWithMap(float Dx, float Dy) //столкновения с картой
	{
		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w; }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; }
				}
				if (obj[i].name == "bridge") 
				{
					if (!(Keyboard::isKeyPressed(Keyboard::S))) 
						if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (onGround && !inWater) {
						if (Dx > 0) { x = obj[i].rect.left - w; }
						if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; }
					}
				}
				if (obj[i].name == "water") {
					inWater = true;
					if (Dy < 0) {
						if (obj[i].rect.top - y > 0) {
							canJump = true;
							if (state != jump)
								y = y + rand() % 10;
						}
						else
							canJump = false;
					}
				}
				else {
					inWater = false;
					canJump = false;
				}
					
			}
		
				
	}

	void update(float time) {
		control();//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right: dx = speed; break;
		case left: dx = -speed; break;
		case up: dy = -speed; break; //в дальнейшем состояния подъема по лестнице
		case down: dx = 0; break;
		case stay: break;
		}
		if (health <= 0) { life = false; }
		if ( life ) {
			x += dx * time;
			checkCollisionWithMap(dx, 0);
			y += dy * time;
			checkCollisionWithMap(0, dy);
			//if (inWater)
			//	std::cout << "  " << inWater << std::endl;
			//else
			//	std::cout << inWater << std::endl;
			
			sprite.setPosition(x + w / 2, y + h / 2);
		
			if (!isMove){ speed = 0; }
			//if (life) { setPlayerCoordinateForView(x, y,  ); }
			if (inWater)
				dy = dy + 0.0001 * time;//притяжение в воде;
			else
				dy = dy + 0.0015 * time;//притяжение к земле;
		}
	}
	
};

class Enemy :public Entity {
private:
	int rotateTime;
	enum { right, left, stay, fall, dead } state;
public:
	Enemy(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) : Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetAllObjects();
		if (Name == "EasyEnemy") {
			sprite.setTextureRect(IntRect(5, 35, W, H));
			speed = 0.05;
			dx = -speed;
			rotateTime = 2500 + rand() % 1000;
			state = left;
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф-ция проверки столкновений с картой
	{
		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if ((obj[i].name == "solid") || (obj[i].name == "bridge") || (obj[i].name == "border") || (moveTimer > rotateTime))//если встретили препятствие
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -speed; state = left; }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx = speed; state = right; }
				}
				if (obj[i].name == "water")
					inWater = true;
				else
					inWater = false;
			}
	}

	void updateAnimation() {
		if (name == "EasyEnemy") {
			switch (state) {
			case left:
				if (currentFrame > 7) { currentFrame = 0; }
				sprite.setTextureRect(IntRect(7 + w * int(currentFrame), 35, w, h));
				break;
			case right:
				if (currentFrame > 7) { currentFrame = 0; }
				sprite.setTextureRect(IntRect(7 + w * int(currentFrame) + w, 35, -w, h));
				break;
			case dead:
				if (currentFrame > 8) { currentFrame = 0; }
				sprite.setTextureRect(IntRect(32 + w * int(currentFrame), 1258, w, h));
				break;
			}
		}
	}

	void update(float time) {
		if (name == "EasyEnemy") {//для персонажа с таким именем логика будет такой
			if (health <= 0) {
				currentFrame += 0.003 * time;
				w = 38; h = 38;
				state = dead;
				if (currentFrame > 8)
					life = false;
				updateAnimation();
				sprite.setPosition(x + w, y + h); 
			}
			if (health > 0) {
				moveTimer += time;
				currentFrame += 0.0075 * time;
				x += dx * time;
				checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
				if (moveTimer > rotateTime) {
					moveTimer = 0;
					rotateTime = 2500 + rand() % 2000;
				}
				y += dy * time;
				checkCollisionWithMap(0, dy);
				if (inWater) { dx /= 3; dy /= 3; }
				updateAnimation();
				sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
				//if (health <= 0) { life = false; }
				dy = dy + 0.0015 * time;
			}
		}
	}
};

void makeScreenShot(int windowWidth, int windowHeight, RenderWindow &window) {
	//if (Keyboard::isKeyPressed(Keyboard::P)) {
		std::ostringstream screenName;
		screenName << time(0);
		Texture texture;
		texture.create(windowWidth, windowHeight);
		texture.update(window);
		Image screenShot = texture.copyToImage();
		screenShot.saveToFile("images/screenshots/img" + screenName.str() + ".png");
	//}
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Platformer");
	Vector2u windowSize = window.getSize();

	view.reset(FloatRect(0, 0, 800, 600));

	Level lvl;
	lvl.LoadFromFile("map_1.tmx");

	srand(time(0));

	/*Image map_img;
	map_img.loadFromFile("images/newmap.png");
	Texture map;
	map.loadFromImage(map_img);
	Sprite s_map;
	s_map.setTexture(map);*/

	Image heroImage;
	heroImage.loadFromFile("images/hero_1.png");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/commander.png");
	easyEnemyImage.createMaskFromColor((Color(69, 130, 161)));

	std::list<Entity*> entities;
	std::list<Entity*>::iterator it;

	std::vector<Object> enemies = lvl.GetObjects("easyEnemy");

	for (int i = 0; i < enemies.size(); i++)
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, enemies[i].rect.left, enemies[i].rect.top, 30, 43)); 

	std::cout << entities.size() << std::endl;

	Object player = lvl.GetObject("player");//объект игрока
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//объект легкого врага

	Player hero(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 36, 35);
	//Enemy easyEnemy(easyEnemyImage, "EasyEnemy", lvl, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 30, 40);

	Clock clock;
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		
		clock.restart();
		time = time / 1000;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::P)) {
			makeScreenShot(windowSize.x, windowSize.y, window);
		}
		
		/*if (Keyboard::isKeyPressed(Keyboard::P)) {   //////Создание скриншотов, можно доработать и выделить в отдульную функцию. Только к имени нужно добавлять время!
			sf::Texture texture;
			texture.create(windowSize.x, windowSize.y);
			texture.update(window);
			sf::Image screenshot = texture.copyToImage();
			screenshot.saveToFile("images/screen.png");
		}*/ 

		hero.update(time);

		for (it = entities.begin(); it != entities.end();) {
			(*it)->update(time);
			if ((*it)->life == false) {
				it = entities.erase(it);
			}
			else
				it++;
		}
		
		for (it = entities.begin(); it != entities.end(); it++) {
			if ((*it)->getRect().intersects(hero.getRect())) {
				if ((*it)->name == "EasyEnemy") {
					if ((hero.dy > 0) && (!hero.onGround)) {
						if ((*it)->health > 0) {
							(*it)->dx = 0;
							hero.dy = -0.2;
							(*it)->health = 0;
						}
					}
					else {
						if ((*it)->health > 0) {
							hero.health -= 1;
						}
					}						
				}
			}
		}

		
		//std::cout << hero.sprite.getGlobalBounds().left << " - " << windowSize.x << "\n";
		setPlayerCoordinateForView(hero.x, hero.y, hero.sprite.getPosition().x, hero.sprite.getPosition().y , windowSize.x, windowSize.y);
		window.setView(view);
		window.clear();
		lvl.Draw(window);

		/*for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (tileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (tileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((tileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((tileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((tileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}*/

		window.draw(hero.sprite);
		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite);
		}
		//window.draw(easyEnemy.sprite);
		window.display();
	}

	return 0;
}