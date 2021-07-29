#pragma once

#include <stdint.h>
#include <memory>
#include <random>
#include <vector>
#include "Tile.h"
#include <SFML/Graphics.hpp>

class Tile;
class Gene;

// Типы отображения мира
enum DisplayMode {
	DISPLAY_MODE_ENERGY, DISPLAY_MODE_LIFE_FORMS, DISPLAY_MODE_SPECIES, DISPLAY_MODES_COUNT
};

// Названия каждого из типа отображения
const char* DISPLAY_MODES_STRINGS[];

class World
{
public:
	World(uint16_t width, uint16_t height);
	~World();

	// Расположение камеры в мире
	sf::Vector2f cameraCenter;
	float cameraZoom = 1.0f;

	// Отображать ли сетку
	bool isGridEnabled = true;

	// Позиция, выбранного тайла. Равна {-1, -1}, если тайл не выбран.
	sf::Vector2i selectedTilePos;
	// Следовать ли за выделенным тайлом
	bool followSelectedTile = false;

	// Текущий тип отображения мира
	DisplayMode displayMode;

	// Энергия за фотосинтез
	float photosynthEnergy = 0.05f;
	// Трата энергии за один ход
	float energySpending = 0.04f;
	// Минимальное количество энергии для размножения клетки
	float reproductionEnergy = 0.8f;
	// Энергия, необходима для перемещения клетки
	float moveEnergy = 0.01f;
	// Шанс возникновения мутации
	float mutationChance = 0.25f;
	// Плотность заселения мира
	float populationDensity = 0.01f;
	// Энергия для клетки, которая дается при её заселении
	float spawnEnergy = 0.05f;

	void selectTile(sf::Vector2f screenPos, sf::Vector2f screenSize);
	void removeTileSelection();
	void regenerate();
	void update();
	void render(sf::RenderTarget&);

	int getWidth();
	int getHeight();
	Tile* getSelectedTile();
	Tile& getTileAt(int x, int y);
	float getTileSize();
	int getStepsCount();
	uint16_t getGenesCount();
	Gene* addGene(uint16_t parentGeneIndex);
	Gene* getGene(uint16_t index);
	float getEnergyMaximum();
	uint32_t getAliveTilesCount();

private:
	int _width;
	int _height;
	uint32_t _stepCounter = 0;
	Tile* _tilemap;
	float _maxEnergy = 0.0f;
	uint32_t _aliveTilesCounter = 0;
	std::vector<std::unique_ptr<Gene>> _genes;
	std::uniform_int_distribution<int> _directionDistribution;
	sf::Vertex* _tileVertices;
	sf::Vertex* _gridVertices;

	// Получить индекс тайла в массиве по его координате
	size_t getTileIndex(int x, int y);

	// Метод обработки клетки
	void processTile(int x, int y);

	// Получить цвет тайла
	sf::Color getTileColor(int x, int y);
};