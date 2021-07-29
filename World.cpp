#include <random>
#include <chrono>
#include <algorithm>
#include "Config.h"
#include "Utils.h"
#include "Tile.h"
#include "Gene.h"
#include "Main.h"
#include "World.h"

using namespace sf;

const char* DISPLAY_MODES_STRINGS[] = {
	"Energy", "Life forms", "Species"
};

World::World(uint16_t width, uint16_t height)
{
	cameraCenter = Vector2f(width * 0.5f, height * 0.5f);
	displayMode = DISPLAY_MODE_LIFE_FORMS;
	_width = width;
	_height = height;
	_tilemap = new Tile[static_cast<size_t>(width * height)];
	_directionDistribution = std::uniform_int_distribution<int>(0, DIRECTIONS_COUNT - 1);
	_tileVertices = new Vertex[static_cast<size_t>(width * height * 6)];

	size_t gridVerticesCount = static_cast<size_t>(width * height * 4);
	_gridVertices = new Vertex[gridVerticesCount];
	for (size_t i = 0; i < gridVerticesCount; i++)
		_gridVertices[i].color = GRID_COLOR;
}

World::~World()
{
	delete[] _tilemap;
	delete[] _tileVertices;
	delete[] _gridVertices;
}

void World::selectTile(Vector2f screenPos, Vector2f screenSize)
{
	float tileSize = std::max(1.0f, getTileSize());

	Vector2f tilePos = cameraCenter - (screenSize * 0.5f - screenPos) / tileSize;

	selectedTilePos.x = static_cast<int>(tilePos.x);
	selectedTilePos.y = static_cast<int>(tilePos.y);
}

void World::removeTileSelection()
{
	selectedTilePos.x = -1;
	selectedTilePos.y = -1;
}

void World::regenerate()
{
	_stepCounter = 0;

	for (uint16_t x = 0; x < _width; x++) {
		for (uint16_t y = 0; y < _height; y++) {
			Tile& tile = getTileAt(x, y);
			tile.temp = Main::randomFloat() * 2.0f - 1.0f;
			tile.direction = _directionDistribution(Main::getRandomGenerator());
			tile.energy = 0.0f;
			tile.geneIndex = 0;
			tile.eatenFoodCount = 0;
			tile.photosynthCount = 0;
			tile.commandsCounter = 0;

			if (Main::randomFloat() < populationDensity) {
				tile.energy = spawnEnergy;
				tile.geneIndex = 1;
			}
		}
	}

	_genes.clear();

	// Создаем базовый ген для клеток
	Gene* gene = addGene(0);
	for (uint8_t i = 0; i < GENE_COMMANDS_COUNT; i++) {
		gene->setCommand(i, COMMAND_PHOTOSYNTH);
	}
}

void World::update()
{
	_maxEnergy = 0.0f;
	_aliveTilesCounter = 0;

	// Сбрасываем счетчик ссылок на гены
	for (uint16_t i = 0; i < _genes.size(); i++) {
		if (_genes[i])
			_genes[i].get()->referenceCount = 0;
	}

	// Сбрасываем каждому тайлу флаг
	for (uint16_t x = 0; x < _width; x++) {
		for (uint16_t y = 0; y < _height; y++) {
			getTileAt(x, y).wasProcessed = false;
		}
	}

	// Обрабатываем каждый тайл
	for (uint16_t x = 0; x < _width; x++) {
		for (uint16_t y = 0; y < _height; y++) {
			processTile(x, y);
		}
	}

	// Удаляем гены, на которые уже нет ссылок
	for (uint16_t i = 0; i < _genes.size(); i++) {
		if (_genes[i] && _genes[i].get()->referenceCount == 0)
			_genes[i].reset();
	}

	_stepCounter++;
}

void World::render(RenderTarget& renderTarget)
{
	float tileSize = std::max(1.0f, getTileSize());

	auto cameraPos = cameraCenter * tileSize;
	auto halfSize = Vector2f(renderTarget.getSize()) * 0.5f;
	auto leftTop = cameraPos - halfSize;
	auto offset = Vector2f(leftTop.x - floorf(leftTop.x), leftTop.y - floorf(leftTop.y));
	auto leftTopTile = Vector2i(static_cast<int>(leftTop.x / tileSize), static_cast<int>(leftTop.y / tileSize));

	// Количество отрисовываемых тайлов
	int countX = static_cast<int>((renderTarget.getSize().x - 1) / tileSize) + 2;
	int countY = static_cast<int>((renderTarget.getSize().y - 1) / tileSize) + 2;

	// Находим предел, в котором будет идти отрисовка
	int startX = std::max(0, leftTopTile.x);
	int endX = std::min(leftTopTile.x + countX, _width);
	int startY = std::max(0, leftTopTile.y);
	int endY = std::min(leftTopTile.y + countY, _height);

	uint32_t tilesVerticesSize = (endX - startX) * (endY - startY) * 6;
	uint32_t gridVerticesSize = (endX - startX) * (endY - startY) * 4;

	uint32_t tilesVerticesCounter = 0;
	uint32_t gridVerticesCounter = 0;
	for (int x = startX; x < endX; x++) {
		for (int y = startY; y < endY; y++) {
			Tile& tile = getTileAt(x, y);
			Color tileColor = getTileColor(x, y);
			auto tilePos = Vector2f((float)x, (float)y) * tileSize - offset - cameraPos + halfSize;

			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos;
			_gridVertices[gridVerticesCounter++].position = tilePos;
			tilesVerticesCounter++;
			
			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos + Vector2f(tileSize, 0.0f);
			_gridVertices[gridVerticesCounter++].position = _tileVertices[tilesVerticesCounter].position;
			_gridVertices[gridVerticesCounter++].position = _tileVertices[tilesVerticesCounter].position;
			tilesVerticesCounter++;

			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos + Vector2f(tileSize, tileSize);
			_gridVertices[gridVerticesCounter++].position = _tileVertices[tilesVerticesCounter].position;
			tilesVerticesCounter++;

			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos + Vector2f(tileSize, tileSize);
			tilesVerticesCounter++;

			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos + Vector2f(0.0f, tileSize);
			tilesVerticesCounter++;

			_tileVertices[tilesVerticesCounter].color = tileColor;
			_tileVertices[tilesVerticesCounter].position = tilePos;
			tilesVerticesCounter++;
		}
	}

	// Рисуем все тайлы
	renderTarget.draw(_tileVertices, tilesVerticesSize, PrimitiveType::Triangles);

	// Рисуем сетку
	if (isGridEnabled)
		renderTarget.draw(_gridVertices, gridVerticesSize, PrimitiveType::Lines);

	static Vertex selectedTileVertices[] = {
		Vertex(Vector2f(), SELECTION_COLOR),
		Vertex(Vector2f(), SELECTION_COLOR),
		Vertex(Vector2f(), SELECTION_COLOR),
		Vertex(Vector2f(), SELECTION_COLOR),
		Vertex(Vector2f(), SELECTION_COLOR)
	};
	if (getSelectedTile() != nullptr) {
		Vector2f tilePos = Vector2f(selectedTilePos) * tileSize - offset - cameraPos + halfSize;
		selectedTileVertices[0].position = tilePos;
		selectedTileVertices[1].position = tilePos + Vector2f(tileSize, 0.0f);
		selectedTileVertices[2].position = tilePos + Vector2f(tileSize, tileSize);
		selectedTileVertices[3].position = tilePos + Vector2f(0.0f, tileSize);
		selectedTileVertices[4].position = tilePos;

		// Рисуем текущий выделенный тайл
		renderTarget.draw(selectedTileVertices, 5, PrimitiveType::LinesStrip);
	}
}

int World::getWidth()
{
	return _width;
}

int World::getHeight()
{
	return _height;
}

Tile* World::getSelectedTile()
{
	if (selectedTilePos.x >= 0 && selectedTilePos.y >= 0 &&
		selectedTilePos.x < _width && selectedTilePos.y < _height)
		return &getTileAt(selectedTilePos.x, selectedTilePos.y);
	return nullptr;
}

Tile& World::getTileAt(int x, int y)
{
	x = Utils::mod(x, (int)_width);
	y = Utils::mod(y, (int)_height);
	return _tilemap[getTileIndex(x, y)];
}

float World::getTileSize()
{
	return TILE_SIZE * cameraZoom;
}

int World::getStepsCount()
{
	return _stepCounter;
}

uint16_t World::getGenesCount()
{
	return static_cast<uint16_t>(_genes.size());
}

Gene* World::addGene(uint16_t parentGeneIndex)
{
	for (uint16_t i = 0; i < _genes.size(); i++) {
		if (!_genes[i]) {
			_genes[i] = std::make_unique<Gene>(i + 1, parentGeneIndex);
			return _genes[i].get();
		}
	}
	_genes.push_back(std::make_unique<Gene>(static_cast<uint16_t>(_genes.size() + 1), parentGeneIndex));
	return _genes[_genes.size() - 1].get();
}

Gene* World::getGene(uint16_t index)
{
	if (index == 0 || index > _genes.size())
		return nullptr;
	return _genes[index - 1].get();
}

float World::getEnergyMaximum()
{
	return _maxEnergy;
}

uint32_t World::getAliveTilesCount()
{
	return _aliveTilesCounter;
}

size_t World::getTileIndex(int x, int y)
{
	return (size_t)y * _width + x;
}

void World::processTile(int x, int y)
{
	// Получаем текущий тайл
	Tile& tile = getTileAt(x, y);

	// Не обрабатываем тайлы, которые перемещались
	if (tile.wasProcessed)
		return;
	tile.wasProcessed = true;

	// Если клетка - неживая
	if (tile.geneIndex == 0)
		return;

	_aliveTilesCounter++;

	// Получаем ген клетки
	Gene* gene = getGene(tile.geneIndex);
	// Увеличиваем счетчик ссылок на данный ген
	gene->referenceCount++;

	// Находим вектор направления клетки
	auto tileDirection = DIRECTION_VECTORS[tile.direction];

	// Находим клетку впереди текущей
	Tile& frontTile = getTileAt(x + tileDirection.x, y + tileDirection.y);

	// Тратим энергию раз в ход
	tile.energy -= energySpending;

	// Если у клетки достаточно энергии для размножения
	if (tile.energy > reproductionEnergy) {
		// Список свободных клеток
		bool freeTiles[DIRECTIONS_COUNT];
		uint8_t freeTilesCount = 0;

		// Ищем все свободные клетки
		for (uint8_t i = 0; i < DIRECTIONS_COUNT; i++) {
			Tile& currTile = getTileAt(x + DIRECTION_VECTORS[i].x, y + DIRECTION_VECTORS[i].y);
			if (currTile.geneIndex != tile.geneIndex) {
				freeTiles[i] = true;
				freeTilesCount++;
			}
		}

		// Если таковые есть
		if (freeTilesCount > 0) {
			std::uniform_int_distribution<int> distr(0, freeTilesCount - 1);
			// Выбираем случайное направление
			uint8_t spawnDirection = static_cast<uint8_t>(distr(Main::getRandomGenerator()));

			// Находим его
			uint8_t counter = 0;
			for (uint8_t i = 0; i < freeTilesCount; i++) {
				if (freeTiles[i])
					counter++;
				if (counter == spawnDirection) {
					spawnDirection = i;
					break;
				}
			}

			// Создаем новую клетку
			Tile& currTile = getTileAt(x + DIRECTION_VECTORS[spawnDirection].x, y + DIRECTION_VECTORS[spawnDirection].y);
			currTile.eatenFoodCount = 0;
			currTile.photosynthCount = 0;
			currTile.geneIndex = tile.geneIndex;
			currTile.energy += tile.energy / 2.0f;
			currTile.direction = _directionDistribution(Main::getRandomGenerator());
			currTile.wasProcessed = true;
			tile.energy /= 2.0f;

			// Создаем мутацию с определенным шансом
			if (Main::randomFloat() < mutationChance)
				currTile.geneIndex = gene->mutate(*this);

			getGene(currTile.geneIndex)->referenceCount++;
		} else {
			// Убиваем клетку, если все соседи заняты
			tile.geneIndex = 0;
		}
	}

	// Находим максимум энергии
	if (tile.energy > _maxEnergy)
		_maxEnergy = tile.energy;

	// Обрабатываем следующую команду
	tile.commandsCounter %= GENE_COMMANDS_COUNT;
	uint8_t opcode = gene->getCommand(tile.commandsCounter);
	switch (opcode)
	{
	case COMMAND_LOOK:
		// Клетка - неживая
		if (frontTile.geneIndex == 0) {
			// Клетка - еда
			if (frontTile.energy > 0.0f)
				tile.commandsCounter += 1;
			else	// Клетка пуста
				tile.commandsCounter += 3;
		}
		// Если клетка - родная
		else if (getGene(frontTile.geneIndex)->getParentIndex() == gene->getParentIndex()) {
			tile.commandsCounter += 2;
		} else {
			tile.commandsCounter += 1;
		}

		break;
	case COMMAND_MOVE:
		// Если клетка - родная
		if (frontTile.geneIndex > 0 && getGene(frontTile.geneIndex)->getParentIndex() == gene->getParentIndex()) {
			tile.commandsCounter += 2;
		} else {
			tile.commandsCounter += 1;
			tile.energy += frontTile.energy;
			tile.energy -= moveEnergy;
			if (frontTile.energy > 0.0f)
				tile.eatenFoodCount++;
			frontTile = tile;

			if (followSelectedTile && x == selectedTilePos.x && y == selectedTilePos.y) {
				selectedTilePos += tileDirection;
			}

			tile.energy = 0.0f;
			tile.geneIndex = 0;
			tile.commandsCounter = 0;
		}
		break;
	case COMMAND_TURN_CW:
		tile.direction = Utils::mod(tile.direction - 1, DIRECTIONS_COUNT);
		tile.commandsCounter++;
		break;
	case COMMAND_TURN_CCW:
		tile.direction = (tile.direction + 1) % DIRECTIONS_COUNT;
		tile.commandsCounter++;
		break;
	case COMMAND_PHOTOSYNTH:
		tile.photosynthCount++;
		tile.energy += photosynthEnergy;
		tile.commandsCounter++;
		break;
	default:
		tile.commandsCounter += opcode;
		break;
	}

	// Убиваем клетку, если у нее не осталось энергии
	if (tile.energy <= 0.0f) {
		tile.energy = 0.0f;
		tile.commandsCounter = 0;
		tile.eatenFoodCount = 0;
		tile.photosynthCount = 0;
		tile.geneIndex = 0;
	}
}

sf::Color World::getTileColor(int x, int y) {
	Tile& tile = getTileAt(x, y);

	switch (displayMode) {
	case DISPLAY_MODE_ENERGY:
		return Utils::mixColors(sf::Color::Blue, sf::Color::Red, tile.energy / _maxEnergy);
	case DISPLAY_MODE_LIFE_FORMS:
		if (tile.geneIndex != 0) {
			if (tile.eatenFoodCount > tile.photosynthCount)
				return PREDATOR_COLOR;
			else
				return PLANTS_COLOR;
		}
		else if (tile.energy > 0.0f)
			return FOOD_COLOR;
		else
			return sf::Color::Black;
	case DISPLAY_MODE_SPECIES:
		Gene* gene = getGene(tile.geneIndex);
		return gene != nullptr ? gene->color : sf::Color::Black;
	}
	return sf::Color::Black;
}