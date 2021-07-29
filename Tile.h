#pragma once

#include <stdint.h>
#include <SFML/Graphics.hpp>

// Виды направлений клетки
#define DIRECTION_UP			0
#define DIRECTION_LEFT_UP		1
#define DIRECTION_LEFT			2
#define DIRECTION_LEFT_DOWN		3
#define DIRECTION_DOWN			4
#define DIRECTION_RIGHT_DOWN	5
#define DIRECTION_RIGHT			6
#define DIRECTION_RIGHT_UP		7
#define DIRECTIONS_COUNT		8

// Векторы для каждого из направлений
const sf::Vector2i DIRECTION_VECTORS[8] = {
	sf::Vector2i(0, -1),
	sf::Vector2i(-1, -1),
	sf::Vector2i(-1, 0),
	sf::Vector2i(-1, 1),
	sf::Vector2i(0, 1),
	sf::Vector2i(1, 1),
	sf::Vector2i(1, 0),
	sf::Vector2i(1, -1),
};

// Названия каждого из направлений
const char* DIRECTION_NAMES[];

class World;

// Структура одной клетки
class Tile
{
public:
	// Температура текущего тайла. Варьируется от -1 до 1
	float temp = 0.0f;

	// Количество "жизненной энергии" в клетке
	float energy = 0.0f;

	// Количество съеденой еды
	uint32_t eatenFoodCount = 0;

	// Количество выполненных операций фотосинтеза
	uint32_t photosynthCount = 0;
	
	// Индекс гена живой клетки. 0 - клетка неживая. >=1 - клетка живая или мертвая
	uint16_t geneIndex = 0;

	// Направление, в которую смотрит клетка
	uint8_t direction = DIRECTION_UP;

	// Текущий счетчик команд
	uint8_t commandsCounter = 0;

	// Был ли обработан тайл
	bool wasProcessed = false;
};
