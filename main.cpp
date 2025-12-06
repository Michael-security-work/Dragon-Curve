#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

enum class Direction {up, right, down, left};
enum class State {generate, restart, display};

Direction turnClockwise(Direction dir)
{
	int dirAsInteger = static_cast<int>(dir);
	dirAsInteger = (dirAsInteger + 1) % 4;
	return static_cast<Direction>(dirAsInteger);
}

class DragonGenerator : public Drawable
{
private:
	vector<Vertex> pixels;
public:
	void draw(RenderTarget& target, RenderStates states) const
	{
		for (auto& v : pixels) target.draw(&v, 1, Points);
	}
	void push_back(int x, int y)
	{
		pixels.push_back(Vertex(Vector2f(static_cast<float>(x), static_cast<float>(y)), Color::White));
	}
	void moveInDirection(Direction dir)
	{
		int size = pixels.size();
		if (size == 0) return;
		int x = static_cast<int>(pixels.back().position.x);
		int y = static_cast<int>(pixels.back().position.y);
		switch (dir)
		{
		case Direction::up:
			push_back(x, y + 1);
			break;
		case Direction::right:
			push_back(x + 1, y);
			break;
		case Direction::down:
			push_back(x, y - 1);
			break;
		case Direction::left:
			push_back(x - 1, y);
			break;
		}
	}
	void clear()
	{
		pixels.clear();
	}
};

int main()
{
	int pixelWidth = VideoMode::getDesktopMode().width;
	int pixelHeight = VideoMode::getDesktopMode().height;
	VideoMode vm(pixelWidth, pixelHeight);

	RenderWindow window(vm, "Dragon Curve", Style::Default);

	Font font;
	if (!font.loadFromFile("fonts/DejaVuSansMono.ttf"))
		font.loadFromFile("Dragon-Curve/fonts/DejaVuSansMono.ttf");
	Text text;
	text.setFont(font);
	text.setFillColor(Color::White);
	text.setCharacterSize(24);
	text.setPosition(20, 20);
	text.setString("Rightclick to start a new fractal.\nLeftclick to iterate the fractal.");

	vector<Direction> pathVector;
	DragonGenerator dragon;

	State state = State::display;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)	
				window.close();
			if (Keyboard::isKeyPressed(Keyboard::Escape)) 
				window.close();

			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left) 
					state = State::generate;
				else if (event.mouseButton.button == Mouse::Right) 
					state = State::restart;
			}
		}

		//update
		if (state == State::generate)
		{
			for (int i = pathVector.size() - 1; i > -1; i--) //go backwards along the path vector
			{
				pathVector.push_back(turnClockwise(pathVector[i])); //rotate the direction by 90 deg clockwise, then uppend the new direction
				dragon.moveInDirection(pathVector.back()); //then go in that new direction
			}
			state = State::display;
		}
		else if (state == State::restart)
		{
			Vector2i coords = Mouse::getPosition(window);
			pathVector.clear();
			pathVector.push_back(Direction::up);
			pathVector.push_back(Direction::right);
			dragon.clear();
			dragon.push_back(coords.x, coords.y);
			dragon.moveInDirection(pathVector[0]);
			dragon.moveInDirection(pathVector[1]);
			state = State::display;
		}

		//draw
		window.clear();
		window.draw(dragon);
		window.draw(text);
		window.display();
	}

	return 0;
}