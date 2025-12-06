#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

enum class direction {up, right, down, left, COUNT};
enum class state {generate, display};

direction turnClockwise(direction dir)
{
	int dirAsInteger = static_cast<int>(dir);
	dirAsInteger = (dirAsInteger + 1) % static_cast<int>(direction::COUNT);
	return static_cast<direction>(dirAsInteger);
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
	bool goInDirection(direction dir)
	{
		int size = pixels.size();
		
		if (size == 0) return false;

		int x = static_cast<int>(pixels[size - 1].position.x);
		int y = static_cast<int>(pixels[size - 1].position.y);
		switch (dir)
		{
		case direction::up:
			push_back(x, y + 1);
			break;
		case direction::right:
			push_back(x + 1, y);
			break;
		case direction::down:
			push_back(x, y - 1);
			break;
		case direction::left:
			push_back(x - 1, y);
			break;
		}
		return true;
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

	View view(FloatRect(0.f, 0.f, pixelWidth, pixelHeight));
	view.setCenter(0.f, 0.f);
	view.setSize(pixelWidth, -pixelHeight);
	window.setView(view);

	vector<direction> dir;
	dir.push_back(direction::up);
	dir.push_back(direction::right);

	DragonGenerator dragon;
	dragon.push_back(0, 0);
	dragon.goInDirection(dir[0]);
	dragon.goInDirection(dir[1]);

	state state = state::display;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) window.close();
			if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();

			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					state = state::generate;
				}
				else if (event.mouseButton.button == Mouse::Right)
				{
					Vector2f coords = window.mapPixelToCoords(Mouse::getPosition(window));
					dir.clear();
					dir.push_back(direction::up);
					dir.push_back(direction::right);
					dragon.clear();
					dragon.push_back(coords.x, coords.y);
					dragon.goInDirection(dir[0]);
					dragon.goInDirection(dir[1]);
				}
			}
		}

		//update
		if (state == state::generate)
		{
			for (int i = dir.size() - 1; i > -1; i--)
			{
				dir.push_back(turnClockwise(dir[i]));
				dragon.goInDirection(dir.back());
			}
			state = state::display;
		}
		
		//draw
		window.clear();
		window.draw(dragon);
		window.display();
	}

	return 0;
}