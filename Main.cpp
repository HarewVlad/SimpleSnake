#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>

#define SIZE 40

inline void move_snake(char *zoneSecondBuffer, std::vector<int> &snake, int new_head_coord);
void input_system(char *zoneSecondBuffer, std::vector<int> &snake);
inline void food_system(char *zoneSecondBuffer, std::vector<int> &snake, std::vector<int> &food);
inline void set_cursor_position(int x, int y);
inline void draw(char *zoneSecondBuffer, char *zoneOriginalBuffer, char *GameOver);

bool LOSE = false;

inline void move_snake(char *zoneSecondBuffer, std::vector<int> &snake, int new_head_coord)
{
	for (int i = 0; i < snake.size(); i++)
	{
		if (snake[i] == new_head_coord)
		{
			LOSE = true;
		}
	}

	snake.push_back(new_head_coord);
	zoneSecondBuffer[snake[0]] = '-';

	for (int i = snake.size() - 1; i > 0; i--)
	{
		zoneSecondBuffer[snake[i]] = '*';
	}

	Sleep(100);

	snake.erase(snake.begin(), snake.begin() + 1);
}

void input_system(char *zoneSecondBuffer, std::vector<int> &snake)
{
	int old_head_coord = snake.back();
	if (GetKeyState('A') & 0x8000 // was W
		&& !(GetKeyState('W') & 0x8000)
		&& !(GetKeyState('S') & 0x8000)
		&& !(GetKeyState('D') & 0x8000))
	{
		if (old_head_coord <= SIZE - 1)
		{
			int new_head_coord = SIZE * SIZE - (SIZE - old_head_coord);
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
		else
		{
			int new_head_coord = old_head_coord - SIZE;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
	}

	if (GetKeyState('D') & 0x8000 // was S
		&& !(GetKeyState('W') & 0x8000)
		&& !(GetKeyState('A') & 0x8000)
		&& !(GetKeyState('S') & 0x8000))
	{
		if (old_head_coord >= SIZE * SIZE - SIZE)
		{
			int new_head_coord = SIZE - (SIZE * SIZE - old_head_coord);
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
		else
		{
			int new_head_coord = old_head_coord + SIZE;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
	}

	if (GetKeyState('W') & 0x8000 // was A
		&& !(GetKeyState('A') & 0x8000)
		&& !(GetKeyState('S') & 0x8000)
		&& !(GetKeyState('D') & 0x8000))
	{
		if (old_head_coord % SIZE == 0)
		{
			int new_head_coord = old_head_coord + SIZE - 1;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
		else
		{
			int new_head_coord = old_head_coord - 1;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
	}

	if (GetKeyState('S') & 0x8000 // was D
		&& !(GetKeyState('W') & 0x8000)
		&& !(GetKeyState('A') & 0x8000)
		&& !(GetKeyState('D') & 0x8000))
	{
		if ((old_head_coord + 1) % SIZE == 0)
		{
			int new_head_coord = old_head_coord - SIZE + 1;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
		else
		{
			int new_head_coord = old_head_coord + 1;
			move_snake(zoneSecondBuffer, snake, new_head_coord);
		}
	}
}

inline void food_system(char *zoneSecondBuffer, std::vector<int> &snake, std::vector<int> &food)
{
	if (food.size() == 0)
	{
		food.push_back(0 + rand() % (SIZE * SIZE));
		zoneSecondBuffer[food[0]] = '!';
	}

	if (snake[0] == food[0])
	{
		snake.push_back(snake[snake.size() - 1]);
		food.pop_back();
	}
}

inline void set_cursor_position(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

inline void draw(char *zoneSecondBuffer, char *zoneOriginalBuffer, char *GameOver)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (zoneSecondBuffer[i * SIZE + j] == zoneOriginalBuffer[i * SIZE + j])
			{
				continue;
			}
			set_cursor_position(i, j);
			std::cout << zoneSecondBuffer[i * SIZE + j];
		}
	}
	std::cout.flush();
	std::memcpy((char*)zoneOriginalBuffer, (const char*)zoneSecondBuffer, SIZE * SIZE);
}

int main()
{
	char zoneSecondBuffer[SIZE * SIZE];
	char zoneOriginalBuffer[SIZE * SIZE];
	char GameOver[SIZE * SIZE + 1] =
	{
		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@!!\n"
		"!!''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''!!\n"
		"!!''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''!!\n"
		"!!''N'''''''''''N''''IIIIIIII'''''''''CCCCCC'''''EEEEEEEEE''''!!\n"
		"!!''N'N'''''''''N'''''''II'''''''''''C'''''''''''EEEEEEEEE''''!!\n"
		"!!''N''N''''''''N'''''''II''''''''''C''''''''''''E''''''''''''!!\n"
		"!!''N'''N'''''''N'''''''II'''''''''C'''''''''''''E''''''''''''!!\n"
		"!!''N''''N''''''N'''''''II'''''''''C'''''''''''''E''''''''''''!!\n"
		"!!''N'''''N'''''N'''''''II'''''''''C'''''''''''''EEEEEEEEE''''!!\n"
		"!!''N''''''N''''N'''''''II'''''''''C'''''''''''''E''''''''''''!!\n"
		"!!''N'''''''N'''N'''''''II'''''''''C'''''''''''''E''''''''''''!!\n"
		"!!''N''''''''N''N'''''''II''''''''''C''''''''''''E''''''''''''!!\n"
		"!!''N'''''''''N'N'''''''II'''''''''''C'''''''''''EEEEEEEEE''''!!\n"
		"!!''N'''''''''''N''''IIIIIIII'''''''''CCCCCC'''''EEEEEEEEE''''!!\n"
		"!!''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''!!\n"
		"!!''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''!!\n"
		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@!!\n"
	};
	std::vector<int> snake;
	std::vector<int> food;

	snake.push_back(60);
	zoneSecondBuffer[snake[0]] = '*';

	while (!LOSE)
	{
		draw(zoneSecondBuffer, zoneOriginalBuffer, GameOver);

		input_system(zoneSecondBuffer, snake);

		food_system(zoneSecondBuffer, snake, food);
	}

	system("cls");

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			std::cout << GameOver[i * SIZE + j];
		}
	}
	
	
	system("pause");
	return 0;
}
