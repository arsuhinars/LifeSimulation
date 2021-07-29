#pragma once

#include <stdint.h>
#include <memory>
#include <random>
#include <vector>
#include "Tile.h"
#include <SFML/Graphics.hpp>

class Tile;
class Gene;

// ���� ����������� ����
enum DisplayMode {
	DISPLAY_MODE_ENERGY, DISPLAY_MODE_LIFE_FORMS, DISPLAY_MODE_SPECIES, DISPLAY_MODES_COUNT
};

// �������� ������� �� ���� �����������
const char* DISPLAY_MODES_STRINGS[];

class World
{
public:
	World(uint16_t width, uint16_t height);
	~World();

	// ������������ ������ � ����
	sf::Vector2f cameraCenter;
	float cameraZoom = 1.0f;

	// ���������� �� �����
	bool isGridEnabled = true;

	// �������, ���������� �����. ����� {-1, -1}, ���� ���� �� ������.
	sf::Vector2i selectedTilePos;
	// ��������� �� �� ���������� ������
	bool followSelectedTile = false;

	// ������� ��� ����������� ����
	DisplayMode displayMode;

	// ������� �� ����������
	float photosynthEnergy = 0.05f;
	// ����� ������� �� ���� ���
	float energySpending = 0.04f;
	// ����������� ���������� ������� ��� ����������� ������
	float reproductionEnergy = 0.8f;
	// �������, ���������� ��� ����������� ������
	float moveEnergy = 0.01f;
	// ���� ������������� �������
	float mutationChance = 0.25f;
	// ��������� ��������� ����
	float populationDensity = 0.01f;
	// ������� ��� ������, ������� ������ ��� � ���������
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

	// �������� ������ ����� � ������� �� ��� ����������
	size_t getTileIndex(int x, int y);

	// ����� ��������� ������
	void processTile(int x, int y);

	// �������� ���� �����
	sf::Color getTileColor(int x, int y);
};