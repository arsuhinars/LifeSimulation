#pragma once

#include <random>
#include <imgui.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class World;
class Gene;

class Main
{
public:
	static sf::RenderWindow& getRenderWindow();
	static float getTimeDelta();
	static sf::Vector2f getMouseDelta();
	static int getScrollDelta();
	static std::minstd_rand0& getRandomGenerator();
	static float randomFloat();

	static void start();

private:
	static std::minstd_rand0 _randomGenerator;
	static std::uniform_real_distribution<float> _floatDistribution;

	static sf::RenderWindow* _renderWindow;
	static sf::RenderTexture _renderTexture;
	static sf::Clock _clock;
	static float _timeDelta;
	static sf::Vector2f _mousePos;
	static sf::Vector2f _mouseDelta;
	static int _scrollDelta;

	static World* _currentWorld;
	static sf::Clock _stepClock;
	static bool _isPaused;
	static bool _skipStep;
	static float _playSpeed;

	static Gene* _editingGene;
	static ImFont* _iconicFont;
	static bool _isAboutWindowOpened;

	static void update();
	static void handleEvent(sf::Event&);
	static void release();

	static void renderGUI();
	static void renderMainMenu();
	static void renderToolsWindow();
	static void renderSettingsWindow();
	static void renderSimulationWindow();
	static void renderGeneEditor();
	static void renderAboutWindow();
};