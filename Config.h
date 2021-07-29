#pragma once

#include <SFML/Graphics.hpp>

#define SIMULATION_VERSION		"0.1"

#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600
#define WINDOW_NAME				"Simulation " SIMULATION_VERSION

#define TILE_SIZE				16

#define ZOOM_DRAG				0.01f
#define MIN_ZOOM				0.01f
#define MAX_ZOOM				10.0f

#define WINDOWS_OFFSET_X		5
#define WINDOWS_OFFSET_Y		25
#define WINDOWS_PADDING_X		5
#define WINDOWS_PADDING_Y		5
#define TOOLS_WIDTH				WINDOW_WIDTH - WINDOWS_PADDING_X * 2
#define TOOLS_HEIGHT			60
#define SETTINGS_WIDTH			300
#define SETTINGS_HEIGHT			500
#define SIMULATION_WIDTH		485
#define SIMULATION_HEIGHT		500
#define SIMULATION_STEP_TIME	0.1f

#define GENE_COMMANDS_COUNT		32
#define MAX_MUTATIONS_COUNT		8

const sf::Color BACKGROUND_COLOR = sf::Color(0x323232FF);
const sf::Color FOOD_COLOR = sf::Color(0xFFBC00FF);
const sf::Color PLANTS_COLOR = sf::Color::Green;
const sf::Color PREDATOR_COLOR = sf::Color::Red;
const sf::Color GRID_COLOR = sf::Color(0x626262FF);
const sf::Color SELECTION_COLOR = sf::Color::White;
