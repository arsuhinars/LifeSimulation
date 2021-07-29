#pragma once

#include <stdint.h>
#include <random>
#include <SFML/Graphics.hpp>
#include "Commands.h"

class World;

class Gene
{
public:
	Gene(uint16_t index, uint16_t parentIndex);
	~Gene();

	// Счетчик ссылок на данный ген
	uint32_t referenceCount = 0;

	// Цвет данного гена
	sf::Color color;

	uint8_t getCommand(uint8_t num);
	void setCommand(uint8_t num, uint8_t command);
	uint16_t getIndex();
	uint16_t getParentIndex();

	// Создать мутацию гена. Возвращаем индекс нового гена
	uint16_t mutate(World& world);

private:
	uint8_t* _commands;
	uint16_t _index;
	uint16_t _parentIndex;

	// Количество мутаций от родителя
	uint16_t _mutationsCount;
};
