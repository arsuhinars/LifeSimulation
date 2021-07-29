#include <random>
#include "Config.h"
#include "Main.h"
#include "World.h"
#include "Utils.h"
#include "Gene.h"

Gene::Gene(uint16_t index, uint16_t parentIndex=0)
{
    _commands = new uint8_t[GENE_COMMANDS_COUNT];
    _index = index;
    _parentIndex = parentIndex;
    _mutationsCount = 0;
    color = Utils::hsvToRgb(Main::randomFloat() * 255.0f, 1.0f, 255.0f);
}

Gene::~Gene()
{
    delete[] _commands;
}

uint8_t Gene::getCommand(uint8_t num)
{
    return _commands[num];
}

void Gene::setCommand(uint8_t num, uint8_t command)
{
    if (num < GENE_COMMANDS_COUNT)
        _commands[num] = command;
}

uint16_t Gene::getIndex()
{
    return _index;
}

uint16_t Gene::getParentIndex()
{
    return _parentIndex;
}

uint16_t Gene::mutate(World& world)
{
    static std::uniform_int_distribution<int> indexDistribution(0, GENE_COMMANDS_COUNT - 1);
    static std::uniform_int_distribution<int> opcodeDistribution(0, GENE_COMMANDS_COUNT + COMMANDS_COUNT);

    uint8_t index = indexDistribution(Main::getRandomGenerator());
    uint8_t command = opcodeDistribution(Main::getRandomGenerator());

    Gene* gene;

    if (_mutationsCount < MAX_MUTATIONS_COUNT) {
        gene = world.addGene(_parentIndex);
        gene->color = color;
        gene->_mutationsCount = _mutationsCount + 1;
    } else {
        gene = world.addGene(_index);
        gene->color = Utils::hsvToRgb(Main::randomFloat() * 255.0f, 1.0f, 255.0f);
        gene->_mutationsCount = 0;
    }
    memcpy(gene->_commands, _commands, GENE_COMMANDS_COUNT);
    gene->_commands[index] = command;

    return gene->getIndex();
}
