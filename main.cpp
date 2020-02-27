#include <iostream>
#include <vector>
#include <assert.h>
#include <chrono>
#include <thread>
#include <windows.h>

namespace Time
{
	typedef std::chrono::steady_clock::time_point TimePoint;
	TimePoint(*getTime)() = std::chrono::high_resolution_clock::now;

	float getDuration(TimePoint timeEnd, TimePoint timeStart)
	{
		return std::chrono::duration<float>(timeEnd - timeStart).count();
	}
}

namespace Window
{
	const uint64_t WIDTH = 40;
	const uint64_t HEIGHT = 20;
}

namespace Console
{
	const HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	const HANDLE rHnd = GetStdHandle(STD_INPUT_HANDLE);
	const COORD characterBufferSize = { Window::WIDTH, Window::HEIGHT };
	const COORD characterPosition = { 0, 0 };
	SMALL_RECT consoleWriteArea = { 0, 0, Window::WIDTH - 1, Window::HEIGHT - 1 };
}

struct Position
{
	uint64_t x, y;

	Position &operator =(const Position &other)
	{
		this->x = other.x;
		this->y = other.y;

		return *this;
	}

	bool operator ==(const Position &other)
	{
		return this->x == other.x && this->y == other.y;
	}
};

enum MoveDirection
{
	TOP,
	DOWN,
	LEFT,
	RIGHT
};

static CHAR_INFO arena[Window::WIDTH * Window::HEIGHT];
static std::vector<Position> snake = {};
static Position food = {};

void initConsole()
{
	SMALL_RECT windowSize = { 0, 0, Window::WIDTH - 1, Window::HEIGHT - 1 };
	COORD bufferSize = { Window::WIDTH, Window::HEIGHT };

	SetConsoleTitle("Snake");
	SetConsoleWindowInfo(Console::wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(Console::wHnd, bufferSize);
}

void initArena()
{
	for (size_t y = 0; y < Window::HEIGHT; ++y)
	{
		for (size_t x = 0; x < Window::WIDTH; ++x)
		{
			arena[x + Window::WIDTH * y].Attributes = BACKGROUND_INTENSITY | FOREGROUND_RED;
		}
	}
}

void initSnake()
{
	uint64_t x = 0 + rand() % (Window::WIDTH - 1);
	uint64_t y = 0 + rand() % (Window::HEIGHT - 1);

	snake.push_back(Position{ x, y });
}

void initFood()
{
	uint64_t x = 0 + rand() % (Window::WIDTH - 1);
	uint64_t y = 0 + rand() % (Window::HEIGHT - 1);

	food = { x, y };
}

bool testSnakeFood()
{
	if (snake[0] == food) return true;
	return false;
}

bool testSnakeSnake()
{
	for (size_t i = 0; i < snake.size() - 1; i++)
	{
		for (size_t j = i + 1; j < snake.size(); j++)
		{
			if (snake[i] == snake[j]) return true;
		}
	}
	return false;
}

void moveSnake(MoveDirection moveDirection)
{
	assert(snake.size() != 0);

	Position tail = snake.back();
	for (size_t i = snake.size() - 1; i > 0; i--)
	{
		snake[i] = snake[i - 1];
	}

	switch (moveDirection)
	{
		case TOP:
		{
			snake[0].y = snake[0].y == 0 ? Window::HEIGHT - 1 : snake[0].y - 1;
		}
		break;
		case DOWN:
		{
			snake[0].y = snake[0].y == Window::HEIGHT - 1 ? 0 : snake[0].y + 1;
		}
		break;
		case LEFT:
		{
			snake[0].x = snake[0].x == 0 ? Window::WIDTH - 1 : snake[0].x - 1;
		}
		break;
		case RIGHT:
		{
			snake[0].x = snake[0].x == Window::WIDTH - 1 ? 0 : snake[0].x + 1;
		}
		break;
		default:
			assert(0);
			break;
	}
	arena[tail.x + Window::WIDTH * tail.y].Char.AsciiChar = ' ';
}

void drawArena()
{
	WriteConsoleOutputA(Console::wHnd,
		arena,
		Console::characterBufferSize,
		Console::characterPosition,
		&Console::consoleWriteArea);
}

void onInit()
{
	initConsole();
	initArena();
	initSnake();
	initFood();
}

void onInput()
{
	if (GetKeyState('W') & 0x8000)
	{
		moveSnake(TOP);
	}
	else if (GetKeyState('S') & 0x8000)
	{
		moveSnake(DOWN);
	}
	else if (GetKeyState('A') & 0x8000)
	{
		moveSnake(LEFT);
	}
	else if (GetKeyState('D') & 0x8000)
	{
		moveSnake(RIGHT);
	}
}

void onUpdate()
{
	if (testSnakeFood())
	{
		// Snake
		{
			Position tail = snake.back();

			snake.push_back(Position{ tail.x + 1, tail.y + 1 });
		}

		// Food
		{
			arena[food.x + Window::WIDTH * food.y].Char.AsciiChar = ' ';

			uint64_t x = 0 + rand() % (Window::WIDTH - 1);
			uint64_t y = 0 + rand() % (Window::HEIGHT - 1);

			food = { x, y };
		}
		
	}

	if (testSnakeSnake())
	{
		assert(0);
	}

	// Snake
	{
		for (size_t i = 0; i < snake.size(); i++)
		{
			uint64_t x = snake[i].x;
			uint64_t y = snake[i].y;

			arena[x + Window::WIDTH * y].Char.AsciiChar = '*';
		}
	}

	// Food
	{
		uint64_t x = food.x;
		uint64_t y = food.y;

		arena[x + Window::WIDTH * y].Char.AsciiChar = '!';
	}
}

void onRender()
{
	drawArena();
}

void run()
{
	onInit();

	float updateTime = 0.0f;
	float frameTime = 1 / 10.0f;
	Time::TimePoint lastTime = Time::getTime();
	for (; ;)
	{
		Time::TimePoint currentTime = Time::getTime();
		float passedTime = Time::getDuration(currentTime, lastTime);
		lastTime = currentTime;

		updateTime += passedTime;

		bool needRender = false;
		while (updateTime >= frameTime)
		{
			onInput();
			onUpdate();

			updateTime -= frameTime;
			needRender = true;
		}

		if (needRender)
		{
			onRender();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}

int main()
{
	run();
}
