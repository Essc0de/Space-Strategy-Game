#include "Ship.h"
#include "Grid.h"
#include <algorithm>

//   TODO
//! Incorporate grid positioning
//! Initialize collision grid
//! Initialize texture
//! Setup Functions

Ship::Ship(Fleet* fleet, Sakura::ResourceManager &resourceManager, std::string team, ShipType shipType, glm::ivec2 position /* Position on GRID */, bool enemy, float speed, int shield, int hull, int shieldDamage, int hullDamage, int damageEffectStrength, DamageEffect damageEffect /*= NORMAL*/){
	init(fleet, resourceManager, team, shipType, position, enemy, speed, shield, hull, shieldDamage, hullDamage, damageEffectStrength, damageEffect);
}

Ship::~Ship(){

}

void Ship::init(Fleet* fleet, Sakura::ResourceManager &resourceManager, std::string team, ShipType shipType, glm::ivec2 position /* Position on GRID */, bool enemy, float speed, int shield, int hull, int shieldDamage, int hullDamage, int damageEffectStrength, DamageEffect damageEffect /*= NORMAL*/){
	std::string texturePath = "Assets/Sprites/Ships/" + team + "/" + shipType;
	glm::ivec2 tileDims = (shipType == CUTTER) ? glm::ivec2(3, 1) : glm::ivec2(1, 1);
	m_texture = resourceManager.getTileSheet(texturePath.c_str(), tileDims, MIPMAP | PIXELATED | EDGE_CLAMP);
	m_hearts = resourceManager.getTileSheet("Assets/Sprites/ship_health_heart.png", glm::ivec2(3,1), MIPMAP | PIXELATED | EDGE_CLAMP);
	m_fleet = fleet;
	m_team = team;
	m_speed = speed;
	m_shipType = shipType;
	m_position = position;
	m_newPosition = position;
	m_enemy = enemy;
	m_shieldMax= shield;
	m_shield = shield;
	m_hullMax= hull;
	m_hull = hull;
	m_shieldDamage = shieldDamage;
	m_hullDamage = hullDamage;
	m_damageEffectStrength = damageEffectStrength;
	m_damageEffect = damageEffect;
	if (shipType == DESTROYER || shipType == ASSUALT_CARRIER || shipType == CARRIER || shipType == BATTLESHIP || shipType == CRUISER){
		m_tileSpan = glm::vec2(2, 1);
	}
	else m_tileSpan = glm::vec2(1, 1);
}

void Ship::update(float deltaTime, bool isTurn, Grid* grid){
	m_absolutePosition = grid->getScreenPos(m_position);
	if (isTurn){
		if (m_position != m_newPosition){
			m_position = m_newPosition;
		}
	}
}

void Ship::draw(Sakura::SpriteBatch& spriteBatch, Grid* grid){
	glm::vec4 uvRect = m_texture.getUVs(0);
	if (m_enemy){
		uvRect = m_texture.getUVs(0);
		uvRect.x += 1.0f / m_texture.dims.x;
		uvRect.z *= -1;
		if (m_tileSpan.x > 1){
			m_absolutePosition.x -= grid->getTileDims().x;
		}
	}
	float shipScale = std::min(grid->getTileDims().x / (m_texture.texture.width / m_texture.dims.x), grid->getTileDims().y / (m_texture.texture.height / m_texture.dims.y));
	glm::vec2 shipSize = glm::vec2((m_texture.texture.width / m_texture.dims.x) * shipScale * m_tileSpan.x, (m_texture.texture.height / m_texture.dims.y) * shipScale * m_tileSpan.y);
	glm::vec4 destRect = glm::vec4(m_absolutePosition.x, m_absolutePosition.y + ((grid->getTileDims().y/2.0f) - (shipSize.y/2.0f)), shipSize.x, shipSize.y);
	spriteBatch.draw(destRect, uvRect, m_texture.texture.id, 0.0f, Sakura::ColorRGBA8(255,255,255,255));
	//destRect = glm::vec4(m_absolutePosition, glm::vec2(m_hearts.texture.width, m_hearts.texture.height));
// 	for (int i = 0; i < m_shield; ++i){
// 		destRect.x -= ((m_hearts.texture.width / 3))*(i+1);
// 		spriteBatch.draw(destRect, m_hearts.getUVs(0), m_hearts.texture.id, 10.0f, Sakura::ColorRGBA8(255, 255, 255, 255));
// 	}
}

void Ship::Damage(int hullDamage, int shieldDamage, int effectStrength, DamageEffect statusEffect /*= NORMAL*/){

}

void Ship::ApplyEffect(DamageEffect statusEffect, int effectStrength){

}

void Ship::damageOther(glm::ivec2 damagePosition){

}