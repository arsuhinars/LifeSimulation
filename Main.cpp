#include <chrono>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include "roboto.h"
#include "IconsMaterialDesign.h"
#include "Config.h"
#include "World.h"
#include "Commands.h"
#include "Gene.h"
#include "Utils.h"
#include "Main.h"

using namespace sf;

std::minstd_rand0 Main::_randomGenerator;
std::uniform_real_distribution<float> Main::_floatDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

RenderWindow* Main::_renderWindow = nullptr;
RenderTexture Main::_renderTexture;
Clock Main::_clock;
float Main::_timeDelta = 0.0f;
Vector2f Main::_mousePos;
Vector2f Main::_mouseDelta;
int Main::_scrollDelta = 0;

World* Main::_currentWorld = nullptr;
Clock Main::_stepClock;
bool Main::_isPaused = true;
bool Main::_skipStep = false;
float Main::_playSpeed = 1.0f;

Gene* Main::_editingGene = nullptr;
ImFont* Main::_iconicFont = nullptr;
bool Main::_isAboutWindowOpened = false;

int main(int argc, char** argv)
{
	Main::start();
	return 0;
}

RenderWindow& Main::getRenderWindow()
{
	return *_renderWindow;
}

float Main::getTimeDelta()
{
	return _timeDelta;
}

Vector2f Main::getMouseDelta()
{
	return _mouseDelta;
}

int Main::getScrollDelta()
{
	return _scrollDelta;
}

std::minstd_rand0& Main::getRandomGenerator()
{
	return _randomGenerator;
}

float Main::randomFloat()
{
	return _floatDistribution(_randomGenerator);
}

void Main::start()
{
	// Инитилизируем генератор случайных чисел
	_randomGenerator.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	// Создаем окно
	_renderWindow = new RenderWindow(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_NAME, Style::Close | Style::Titlebar);
	_renderWindow->setFramerateLimit(60);

	// Создаем текстуру для отрисовки графики
	_renderTexture.create(320, 240);

	// Инитилизируем ImGui
	ImGui::SFML::Init(*_renderWindow, false);
	ImGui::StyleColorsDark();

	// Загружаем шрифты
	ImGuiIO& io = ImGui::GetIO();
	io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(roboto_compressed_data, roboto_compressed_size, 14.0f);

	static const ImWchar iconRanges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
	_iconicFont = io.Fonts->AddFontFromMemoryCompressedTTF(MaterialDesignIconicFont_compressed_data, MaterialDesignIconicFont_compressed_size, 14.0f, (const ImFontConfig*)0, iconRanges);
	
	ImGui::SFML::UpdateFontTexture();
	
	// Загружаем мир
	_currentWorld = new World(128, 128);
	_currentWorld->regenerate();

	// Главный цикл
	while (_renderWindow->isOpen()) {
		update();
	}

	// Освобождаем все ресурсы
	release();
}

void Main::update()
{
	// Находим время отрисовки кадра
	Time dt = _clock.restart();
	_timeDelta = dt.asSeconds();

	// Сбрасываем значения
	_scrollDelta = 0;

	// Получаем все события и обрабатываем их
	Event event;
	while (_renderWindow->pollEvent(event)) {
		handleEvent(event);
		ImGui::SFML::ProcessEvent(event);
	}

	// Находим перемещение мыши
	_mouseDelta = Vector2f(Mouse::getPosition()) - _mousePos;
	_mousePos = Vector2f(Mouse::getPosition());

	// Выполняем один шаг в текущем мире
	if (_skipStep || (!_isPaused && _stepClock.getElapsedTime().asSeconds() > SIMULATION_STEP_TIME / _playSpeed)) {
		_stepClock.restart();
		_currentWorld->update();
	}
	_skipStep = false;

	// Рисуем текущий мир
	_renderTexture.resetGLStates();
	_renderTexture.clear(Color::Transparent);
	_currentWorld->render(_renderTexture);
	_renderTexture.display();

	// Рисуем интерфейс
	ImGui::SFML::Update(*_renderWindow, dt);
	renderGUI();

	// Выводим все на экран
	_renderWindow->clear(BACKGROUND_COLOR);
	ImGui::SFML::Render();
	_renderWindow->display();
}

void Main::handleEvent(sf::Event& event)
{
	switch (event.type)
	{
	case Event::MouseWheelScrolled:
		_scrollDelta = static_cast<int>(event.mouseWheelScroll.delta);
		break;
	case Event::Closed:
		_renderWindow->close();
		break;
	}
}

void Main::release()
{
	ImGui::SFML::Shutdown();
	delete _renderWindow;
	delete _currentWorld;
}

void Main::renderGUI()
{
	renderMainMenu();
	renderToolsWindow();
	renderSettingsWindow();
	renderSimulationWindow();
	renderGeneEditor();
	renderAboutWindow();
}

void Main::renderMainMenu()
{
	// Главное меню наверху окна
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		ImGui::MenuItem("Load state");
		ImGui::MenuItem("Save state");
		ImGui::MenuItem("Save state as");
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("About")) {
		_isAboutWindowOpened = true;
	}
	ImGui::EndMainMenuBar();
}

void Main::renderToolsWindow()
{
	const ImVec2 toolsPos = ImVec2(WINDOWS_OFFSET_X, WINDOWS_OFFSET_Y);
	const ImVec2 toolsSize = ImVec2(TOOLS_WIDTH, TOOLS_HEIGHT);

	// Меню инструментов
	ImGui::SetNextWindowPos(toolsPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(toolsSize, ImGuiCond_Once);
	ImGui::Begin("Tools");

	ImGui::PushItemWidth(50.0f);

	ImGui::PushFont(_iconicFont);
	if ((_isPaused && ImGui::Button(ICON_MD_PLAY_ARROW)) || (!_isPaused && ImGui::Button(ICON_MD_PAUSE)))
		_isPaused = !_isPaused;

	ImGui::SameLine(0.0f, 10.0f);
	if (ImGui::Button(ICON_MD_SKIP_NEXT))
		_skipStep = true;

	ImGui::SameLine(0.0f, 10.0f);
	if (ImGui::Button(ICON_MD_REPLAY))
		_currentWorld->regenerate();
	ImGui::PopFont();

	ImGui::SameLine(0.0f, 20.0f);
	ImGui::DragFloat("Play speed", &_playSpeed, 0.01f, 0.0f, 0.0f, "%.1f");

	ImGui::SameLine(0.0f, 10.0f);
	ImGui::DragFloat("Zoom", &_currentWorld->cameraZoom, 0.01f, MIN_ZOOM, MAX_ZOOM, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	
	ImGui::SameLine(0.0f, 10.0f);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::Combo("Display mode", reinterpret_cast<int*>(&_currentWorld->displayMode), DISPLAY_MODES_STRINGS, DISPLAY_MODES_COUNT);

	ImGui::PushFont(_iconicFont);
	ImGui::SameLine(0.0f, 10.0f);
	if ((_currentWorld->isGridEnabled && ImGui::Button(ICON_MD_GRID_OFF)) || 
		(!_currentWorld->isGridEnabled && ImGui::Button(ICON_MD_GRID_ON)))
		_currentWorld->isGridEnabled = !_currentWorld->isGridEnabled;

	ImGui::SameLine(0.0f, 10.0f);
	if (ImGui::Button(ICON_MD_FULLSCREEN)) {
		_currentWorld->cameraCenter.x = _currentWorld->getWidth() * 0.5f;
		_currentWorld->cameraCenter.y = _currentWorld->getHeight() * 0.5f;
		_currentWorld->cameraZoom = (float)_renderTexture.getSize().x / _currentWorld->getWidth() / TILE_SIZE;
	}

	ImGui::SameLine(0.0f, 10.0f);
	bool& followSelectedTile = _currentWorld->followSelectedTile;
	if ((followSelectedTile && ImGui::Button(ICON_MD_VISIBILITY_OFF)) || (!followSelectedTile && ImGui::Button(ICON_MD_VISIBILITY)))
		followSelectedTile = !followSelectedTile;

	ImGui::PopFont();

	ImGui::PopItemWidth();

	ImGui::End();
}

void Main::renderSettingsWindow()
{
	const ImVec2 settingsPos = ImVec2(WINDOWS_OFFSET_X, WINDOWS_OFFSET_Y + TOOLS_HEIGHT + WINDOWS_PADDING_Y);
	const ImVec2 settingsSize = ImVec2(SETTINGS_WIDTH, SETTINGS_HEIGHT);

	// Меню настроек мира
	ImGui::SetNextWindowPos(settingsPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(settingsSize, ImGuiCond_Once);
	ImGui::Begin("Settings");

	ImGui::PushItemWidth(100.0f);

	// Статистка мира
	if (ImGui::CollapsingHeader("Stats")) {
		ImGui::Text("World size: %ix%i", _currentWorld->getWidth(), _currentWorld->getHeight());
		ImGui::Text("Steps: %i", _currentWorld->getStepsCount());
		ImGui::Text("Energy maximum: %.2f", _currentWorld->getEnergyMaximum());
		ImGui::Text("Alive tiles counter: %i", _currentWorld->getAliveTilesCount());

		static float fps = 0.0f;
		static Clock fpsUpdateTimer;
		if (fpsUpdateTimer.getElapsedTime().asMilliseconds() > 200) {
			fps = 1.0f / _timeDelta;
			fpsUpdateTimer.restart();
		}

		ImGui::Text("FPS: %.1f", fps);;
	}

	// Статистка выделенного тайла
	Tile* selectedTile = _currentWorld->getSelectedTile();
	if (selectedTile != nullptr && ImGui::CollapsingHeader("Selected tile stats")) {
		ImGui::LabelText("Position", "(%i, %i)", _currentWorld->selectedTilePos.x, _currentWorld->selectedTilePos.y);
		ImGui::LabelText("Eaten food count", "%i", selectedTile->eatenFoodCount);
		ImGui::LabelText("Photosynth count", "%i", selectedTile->photosynthCount);
		ImGui::LabelText("Commands counter", "%i", selectedTile->commandsCounter);

		ImGui::InputFloat("Energy", &selectedTile->energy, 0.01f, 0.1f);

		int geneIndex = selectedTile->geneIndex;
		if (ImGui::InputInt("Gene index", &geneIndex, 0, 0) && _currentWorld->getGene(geneIndex) != nullptr) {
			selectedTile->geneIndex = geneIndex;
		}

		int direction = selectedTile->direction;
		if (ImGui::Combo("Direction", &direction, DIRECTION_NAMES, DIRECTIONS_COUNT)) {
			selectedTile->direction = static_cast<uint8_t>(direction);
		}
	}

	// Настройки мира
	if (ImGui::CollapsingHeader("World")) {
		ImGui::InputFloat("Photosynth energy", &_currentWorld->photosynthEnergy, 0.01f, 0.1f);
		ImGui::InputFloat("Energy spending", &_currentWorld->energySpending, 0.01f, 0.1f);
		ImGui::InputFloat("Reproduction energy", &_currentWorld->reproductionEnergy, 0.01f, 0.1f);
		ImGui::InputFloat("Move energy", &_currentWorld->moveEnergy, 0.01f, 0.1f);
		ImGui::SliderFloat("Mutation chance", &_currentWorld->mutationChance, 0.0f, 1.0f);
		ImGui::InputFloat("Population density", &_currentWorld->populationDensity, 0.01f, 0.1f);
		ImGui::InputFloat("Spawn energy", &_currentWorld->spawnEnergy, 0.01f, 0.1f);
	}

	ImGui::PopItemWidth();

	// Настройки генов
	if (ImGui::CollapsingHeader("Genes")) {
		for (int i = 1; i <= _currentWorld->getGenesCount(); i++) {
			Gene* gene = _currentWorld->getGene(i);
			if (gene != nullptr) {
				ImGui::PushID(i);
				ImGui::TextColored(gene->color, "Gene #%i", i);
				ImGui::SameLine(0.0f, 10.0f);
				if (ImGui::SmallButton("Edit")) {
					_editingGene = gene;
					ImGui::SetWindowFocus("Gene editor");
				}
				ImGui::PopID();
			}
		}
	}

	ImGui::End();
}

void Main::renderSimulationWindow()
{
	const ImVec2 simulationPos = ImVec2(WINDOWS_OFFSET_X + SETTINGS_WIDTH + WINDOWS_PADDING_X, WINDOWS_OFFSET_Y + TOOLS_HEIGHT + WINDOWS_PADDING_Y);
	const ImVec2 simualtionSize = ImVec2(SIMULATION_WIDTH, SIMULATION_HEIGHT);

	// Меню отображения симуляции
	ImGui::SetNextWindowPos(simulationPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(simualtionSize, ImGuiCond_Once);
	ImGui::Begin("Simulation");

	// Находим разрешение текстуры рендеринга
	static ImVec2 textureSize;
	ImVec2 currTextureSize = ImGui::GetContentRegionAvail();
	if (currTextureSize.x != textureSize.x || currTextureSize.y != textureSize.y) {
		textureSize = currTextureSize;
		_renderTexture.create(static_cast<unsigned int>(textureSize.x), static_cast<unsigned int>(textureSize.y));
	}

	// Получаем позицию изображения
	Vector2f imagePos = Vector2f(
		ImGui::GetWindowPos().x + ImGui::GetCursorPosX(),
		ImGui::GetWindowPos().y + ImGui::GetCursorPosY()
	);

	// Рисуем симуляцию
	ImGui::Image(_renderTexture);

	if (ImGui::IsItemHovered()) {
		_currentWorld->cameraZoom += _scrollDelta * ZOOM_DRAG;
		_currentWorld->cameraZoom = Utils::clamp(_currentWorld->cameraZoom, MIN_ZOOM, MAX_ZOOM);

		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
			// Перемещаем камеру в окне
			_currentWorld->cameraCenter -= Vector2f(
				_mouseDelta.x / _currentWorld->getTileSize(),
				_mouseDelta.y / _currentWorld->getTileSize()
			);

			// Ограничиваем камеру в пределы мира
			_currentWorld->cameraCenter.x = Utils::clamp(_currentWorld->cameraCenter.x, 0.0f, static_cast<float>(_currentWorld->getWidth()));
			_currentWorld->cameraCenter.y = Utils::clamp(_currentWorld->cameraCenter.y, 0.0f, static_cast<float>(_currentWorld->getHeight()));

			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			_currentWorld->selectTile(
				static_cast<Vector2f>(ImGui::GetMousePos()) - imagePos,
				Vector2f(_renderTexture.getSize())
			);
		}
	}

	ImGui::End();
}

void Main::renderGeneEditor()
{
	if (_editingGene == nullptr)
		return;

	bool isOpened = true;
	ImGui::Begin("Gene editor", &isOpened, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

	if (!isOpened) {
		_editingGene = nullptr;
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar()) {
		ImGui::MenuItem("Save gene");
		ImGui::MenuItem("Load gene");
		ImGui::EndMenuBar();
	}

	float color[] = {
		_editingGene->color.r / 255.0f,
		_editingGene->color.g / 255.0f,
		_editingGene->color.b / 255.0f
	};
	ImGui::ColorEdit3("Color", color);
	_editingGene->color.r = static_cast<Uint8>(color[0] * 255.0f);
	_editingGene->color.g = static_cast<Uint8>(color[1] * 255.0f);
	_editingGene->color.b = static_cast<Uint8>(color[2] * 255.0f);

	ImGui::NewLine();
	ImGui::TextUnformatted("Commands: ");

	ImGui::PushItemWidth(30.0f);
	for (uint8_t i = 0; i < GENE_COMMANDS_COUNT; i += 8) {
		ImGui::PushID(i);
		for (uint8_t k = 0; k < 8; k++) {
			ImGui::PushID(k);

			int command = _editingGene->getCommand(i + k);
			if (ImGui::InputInt("", &command, 0, 0)) {
				_editingGene->setCommand(i + k, static_cast<uint8_t>(Utils::clamp(command, 0, 255)));
			}

			ImGui::SameLine(0.0f, 4.0f);
			ImGui::PopID();
		}
		ImGui::PopID();
		ImGui::NewLine();
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

void Main::renderAboutWindow()
{
	if (!_isAboutWindowOpened)
		return;

	ImGui::Begin("About", &_isAboutWindowOpened, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	if (_isAboutWindowOpened && !ImGui::IsWindowFocused())
		_isAboutWindowOpened = false;

	ImGui::TextUnformatted("Simulation program");
	ImGui::TextUnformatted("Version: " SIMULATION_VERSION);
	ImGui::TextUnformatted("Author: Arseny Fedorov");
	ImGui::TextUnformatted("My github: https://github.com/arsuhinars");
	ImGui::TextUnformatted("My contacts: arsuhinars@gmail.com");
	ImGui::NewLine();
	ImGui::TextUnformatted("It was written on C++ with using:");
	ImGui::BulletText("SFML - https://www.sfml-dev.org");
	ImGui::BulletText("ImGui - https://github.com/ocornut/imgui");
	ImGui::BulletText("JSON for Modern C++ - https://github.com/nlohmann/json");

	ImGui::End();
}