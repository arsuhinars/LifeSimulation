#pragma once

#include <stdint.h>
#include <SFML/Graphics.hpp>

// ���� ����������� ������
#define DIRECTION_UP			0
#define DIRECTION_LEFT_UP		1
#define DIRECTION_LEFT			2
#define DIRECTION_LEFT_DOWN		3
#define DIRECTION_DOWN			4
#define DIRECTION_RIGHT_DOWN	5
#define DIRECTION_RIGHT			6
#define DIRECTION_RIGHT_UP		7
#define DIRECTIONS_COUNT		8

// ������� ��� ������� �� �����������
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

// �������� ������� �� �����������
const char* DIRECTION_NAMES[];

class World;

// ��������� ����� ������
class Tile
{
public:
	// ����������� �������� �����. ����������� �� -1 �� 1
	float temp = 0.0f;

	// ���������� "��������� �������" � ������
	float energy = 0.0f;

	// ���������� �������� ���
	uint32_t eatenFoodCount = 0;

	// ���������� ����������� �������� �����������
	uint32_t photosynthCount = 0;
	
	// ������ ���� ����� ������. 0 - ������ �������. >=1 - ������ ����� ��� �������
	uint16_t geneIndex = 0;

	// �����������, � ������� ������� ������
	uint8_t direction = DIRECTION_UP;

	// ������� ������� ������
	uint8_t commandsCounter = 0;

	// ��� �� ��������� ����
	bool wasProcessed = false;
};
