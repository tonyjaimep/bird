#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class Player {
	float yVelocity;
	const float yAcceleration = 0.7;
	int score;
	bool jumping = false;
	public:
		sf::CircleShape shape;
		Player();
		void jump(void);
		void updatePosition(void);
		void reset(void);
		int getScore(void);
		void incrementScore(int);
};

Player::Player(void) {
	this->shape.setRadius(12);
	this->shape.setFillColor(sf::Color::White);
	this->shape.setPosition(100, 235);
	this->score = 0;
	this->yVelocity = 0;
}

void Player::jump(void) {
	if (this->yVelocity >= 0)
		this->yVelocity = -10;
}

void Player::updatePosition(void) {
	this->yVelocity += yAcceleration;
	this->shape.move(0, this->yVelocity);
}

void Player::reset(void) {
	this->shape.setPosition(100, 235);
	this->score = 0;
}

int Player::getScore(void) {
	return this->score;
}

void Player::incrementScore(int quantity) {
	this->score += quantity;
}

class PipePair {
	public:
		sf::RectangleShape topPipe;
		sf::RectangleShape bottomPipe;
		PipePair(sf::RenderWindow*);
		void scroll(float);
		void draw();
		bool collidesWith(sf::CircleShape);
	private:
		sf::RenderWindow* window;
};

PipePair::PipePair(sf::RenderWindow* window) {
	this->window = window;

	// opening between pipes
	float openingY = std::rand() % 200;
	const float openingHeight = 100;

	// style pipes
	topPipe.setFillColor(sf::Color::White);
	bottomPipe.setFillColor(sf::Color::White);

	// position top pipe at the top right of the window
	this->topPipe.setPosition(this->window->getSize().x, 0);
	this->topPipe.setSize(sf::Vector2f(24, openingY));

	// position bottom pipe
	this->bottomPipe.setPosition(this->window->getSize().x, openingY + openingHeight);
	this->bottomPipe.setSize(sf::Vector2f(24, this->window->sf::Window::getSize().y - (openingY + openingHeight)));
}

void PipePair::scroll(float speed) {
	this->topPipe.move(speed, 0);
	this->bottomPipe.move(speed, 0);
}

void PipePair::draw() {
	this->window->draw(this->topPipe);
	this->window->draw(this->bottomPipe);
}

bool PipePair::collidesWith(sf::CircleShape shape) {
	return this->topPipe.getGlobalBounds().intersects(shape.getGlobalBounds()) || this->bottomPipe.getGlobalBounds().intersects(shape.getGlobalBounds());
}

int main(void) {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(640, 480), "Bird", sf::Style::None, settings);
	window.setFramerateLimit(60);
	sf::Font font;
	Player player;

	bool gameHasStarted = false;
	bool gameEnded = false;

	if (!font.loadFromFile("assets/fonts/IBMPlexMono-Regular.ttf")) {
		std::cout << "Could not load font. Aborting" << std::endl;
		window.close();
	}

	// start text objects

	sf::Text titleText;
	titleText.setFont(font);
	titleText.setString("Bird");
	titleText.setCharacterSize(36);
	titleText.setFillColor(sf::Color::White);

	sf::FloatRect titleTextBounds = titleText.getGlobalBounds();
	// position at the center of the screen
	titleText.setPosition((window.getSize().x - titleTextBounds.width) / 2, (window.getSize().y - titleTextBounds.height) / 2);
	// redefine title text bounds for positioning startText relative to titleText
	titleTextBounds = titleText.getGlobalBounds();

	sf::Text startText;
	startText.setFont(font);
	startText.setString("Press <Space> to start");
	startText.setCharacterSize(12);
	startText.setFillColor(sf::Color::White);

	sf::FloatRect startTextBounds = startText.getGlobalBounds();
	// position is relative to titleText
	startText.setPosition((window.getSize().x - startTextBounds.width) / 2, titleTextBounds.top + titleTextBounds.height + 12);

	sf::Text helpText;
	helpText.setFont(font);
	helpText.setString("<Space>: Jump\n<Escape>: Quit");
	helpText.setCharacterSize(12);
	helpText.setFillColor(sf::Color::White);

	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setString("Game over");
	gameOverText.setCharacterSize(25);
	gameOverText.setFillColor(sf::Color::White);
	sf::FloatRect gameOverTextBounds = gameOverText.getGlobalBounds();
	// position at the center of the screen
	gameOverText.setPosition((window.getSize().x - gameOverTextBounds.width) / 2, (window.getSize().y - gameOverTextBounds.height) / 2);

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(12);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setString("Score: " + std::to_string(player.getScore()));

	sf::FloatRect scoreTextBounds = scoreText.getLocalBounds();
	scoreText.setPosition(0, window.getSize().y - scoreTextBounds.height * 2);
	std::vector<PipePair*> pipePairs;
	unsigned int i; // iterating over pipePairs

	// end text objects

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

		}

		scoreText.setString("Score: " + std::to_string(player.getScore()));

		window.clear(sf::Color(12, 12, 12));
		window.draw(scoreText);
		window.draw(helpText);

		if (gameHasStarted) {
			if (gameEnded) {
				pipePairs = {};
				window.draw(gameOverText);
				window.display();
				for (;;) {
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
						gameEnded = false;
						player.reset();
						pipePairs.push_back(new PipePair(&window));
						break;
					} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
						window.close();
						break;
					}
				}
			} else {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
					player.jump();

				player.updatePosition();

				for (i=0; i < pipePairs.size(); i++) {
					pipePairs[i]->scroll(-2);
					pipePairs[i]->draw();

					// detect collision
					if (pipePairs[i]->collidesWith(player.shape)) {
						gameEnded = true;
					} else if ((pipePairs[i]->topPipe.getSize().x + pipePairs[i]->topPipe.getPosition().x) == player.shape.getPosition().x) {
						player.incrementScore(1);
					} else if ((pipePairs[i]->topPipe.getSize().x + pipePairs[i]->topPipe.getPosition().x) < 0) {
						delete pipePairs[i];
						pipePairs.resize(1);
						pipePairs.push_back(new PipePair(&window));
					}
				}


				if (player.shape.getPosition().y > window.getSize().y)
					gameEnded = true;
			}
		} else {
			window.draw(titleText);
			window.draw(startText);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				gameHasStarted = true;
				pipePairs.push_back(new PipePair(&window));
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();

		window.draw(player.shape);
		window.display();
	}

	return 0;
}
