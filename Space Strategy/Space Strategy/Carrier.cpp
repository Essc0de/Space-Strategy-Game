#include "Carrier.h"
#include "Fleet.h"
#include "Grid.h"

Carrier::Carrier(Grid* grid, Fleet* fleet, Sakura::ResourceManager &resourceManager, std::string team, 
	ShipType shipType, glm::ivec2 position /* Position on GRID */, bool enemy){
	init(grid, fleet, resourceManager, team, shipType, position, enemy, 5, 7, 7, 0, 0, 2, 15, DamageEffect(EMP, 1.0f, 0.25f, 2));
}

Carrier::~Carrier(){

}

int Carrier::createCutter(glm::ivec2 position, Grid* grid, Sakura::ResourceManager& resourceManager){
	return m_fleet->addShip(grid, resourceManager, ShipType::CUTTER, grid->getScreenPos(position), position, false);
}