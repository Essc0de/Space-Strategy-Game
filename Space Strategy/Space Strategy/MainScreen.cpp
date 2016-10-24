#include "MainScreen.h"
#include <Sakura/IMainSystem.h>
#include "Ship.h"

#define screen_scale_level 1.0f

/************************************************************************/
/* SCREEN                                                               */
/************************************************************************/
MainScreen::MainScreen(Sakura::Window* window) : m_window(window){
	/* Empty */
}

MainScreen::~MainScreen(){
	/* Empty */
}

int MainScreen::getNextScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

int MainScreen::getPreviousScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

void MainScreen::build(){

}

void MainScreen::destroy(){

}

void MainScreen::onEntry(){
	m_GLSLSamplerName = "mySampler";
	m_GLSLPUniformName = "P";
	m_spriteBatch.init();
	m_debugRenderer.init();
	m_resourceManager.init();
	m_glyphSortType = Sakura::GlyphSortType::FRONT_TO_BACK;

	// Compile the color shader
	m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	// Compile static resources
	m_mainMusic = m_resourceManager.loadMusic("Assets/Audio/Music/n-Dimensions.ogg");
	m_bossMusic = m_resourceManager.loadMusic("Assets/Audio/Music/Orbital Colossus.ogg");
	m_background = m_resourceManager.getTexture("Assets/Sprites/UI/Grid.png", MIPMAP | PIXELATED | EDGE_CLAMP);
	m_userFont.initTTF("Assets/Fonts/destructobeambb_reg.ttf", 144,  MIPMAP | LINEAR | TRANS_BORDER);
	m_enemyFont.initTTF("Assets/Fonts/Sprykski.ttf", 144, MIPMAP | LINEAR | TRANS_BORDER);
	m_debugFont.initTTF("Assets/Fonts/_bitmap_font.ttf", 144, MIPMAP | PIXELATED | TRANS_BORDER);

	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(screen_scale_level);
	glm::vec2 cameraPositionOffset((m_window->getScreenWidth() / 2.0f) * (1.0f / screen_scale_level),
		(m_window->getScreenHeight() / 2.0f) * (1.0f / screen_scale_level));
	m_camera.setPosition(cameraPositionOffset);
	m_interface.init(this, &m_textureProgram, &m_resourceManager, glm::ivec2(m_window->getScreenWidth(), m_window->getScreenHeight()), cameraPositionOffset, screen_scale_level);
	m_interface.initButtons(m_window);
	m_interface.initShipIcons(m_window);
	m_grid.init(glm::ivec2(27, 27), glm::ivec2(), m_window);
	m_playerFleet.init(&m_enemyFleet, "Gray", &m_interface, false);
	m_enemyFleet.init(&m_playerFleet, "Red", &m_interface, true);
	m_playerFleet.addShip(&m_grid, &m_enemyFleet, m_resourceManager, ShipType::COMMANDSHIP, m_grid.getScreenPos(glm::ivec2(0, 10)), glm::ivec2(0, 10), 0, false);
	//HACK
	m_enemyFleet.addShip(&m_grid, &m_playerFleet, m_resourceManager, ShipType::ASSAULT_CARRIER, m_grid.getScreenPos(glm::ivec2(25, 10)), glm::ivec2(25, 10), 0);
}

void MainScreen::onExit(){
	m_textureProgram.dispose();
	m_debugRenderer.dispose();
	m_debugFont.dispose();
	m_resourceManager.destroyResources();
	m_spriteBatch.dispose();
	m_interface.destroy();
	m_userFont.dispose();
	m_enemyFont.dispose();
	m_debugFont.dispose();
}

void MainScreen::update(float deltaTime){
	m_deltaTime = deltaTime;
	glm::vec2 mouseCoords = m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords());
	/* Update game objects */

	switch (m_interface.getState()){
	case GAMEPLAY:
		m_playerFleet.update(deltaTime, &m_grid);
		m_enemyFleet.update(deltaTime, &m_grid);
		break;
	case MENU:
	case OPTIONSmain:

		break;
	case FLEET:
		m_playerFleet.update(deltaTime, &m_grid);
		m_enemyFleet.update(deltaTime, &m_grid);
		break;
	default:
		break;
	}
	
	if (m_enemyFleet.getFleetSize() == 0){
		m_interface.addScore(500);
		/* Spawn new enemy fleet */
	}

	m_camera.update();
	m_interface.update(m_game->inputManager);
	m_interface.setSelectedShipType(m_playerFleet.getAddedShip());
	/* Finish Updates */
	checkInput();
}

void MainScreen::draw(){
	/* Background Color */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec2 mouseCoords = m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords());

	/* returns a mat4 with the camera projection matrix */
	// *** camera_matrix(m_camera);
	/* Draw game elements concurrently */

	m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_background.id, -500.0f, Sakura::ColorRGBA8(255,255,255,255));

	m_playerFleet.draw(m_spriteBatch, mouseCoords);
	m_enemyFleet.draw(m_spriteBatch, mouseCoords);

	//x m_userFont.draw(m_spriteBatch, "Lorem ipsum dolor sit amet,\n consectetur adipiscing elit.\n Integer nec odio. Praesent libero.\n Sed cursus ante dapibus diam.\n Sed nisi. Nulla quis sem at nib.", glm::vec2(0.0f, m_window->getScreenHeight() - m_userFont.getFontHeight() * 0.2f), glm::vec2(0.2f), 1.0f, Sakura::ColorRGBA8(255, 0, 0, 255), Sakura::Justification::LEFT);
	//x m_enemyFont.draw(m_spriteBatch, "Lorem ipsum dolor sit amet,\n consectetur adipiscing elit.\n Integer nec odio. Praesent libero.\n Sed cursus ante dapibus diam.\n Sed nisi. Nulla quis sem at nib.", glm::vec2(m_window->getScreenWidth() - m_enemyFont.getFontHeight() * 25 * .2f, m_window->getScreenHeight() - m_userFont.getFontHeight() * 0.2f), glm::vec2(0.2f), 1.0f, Sakura::ColorRGBA8(255, 0, 0, 255), Sakura::Justification::LEFT);
	/* DEBUG */
	//HACK
	if (debug_game_events){
		drawDebugElements();
		m_grid.drawDebug(m_debugRenderer);
	}
	if (show_boxes){
		m_playerFleet.drawDebug(m_debugRenderer);
		m_enemyFleet.drawDebug(m_debugRenderer);
	}
	/* Finish draw */
}

void MainScreen::drawDebugElements(){
	std::string fps = "FPS: " + std::to_string(m_game->getFps());
	m_debugFont.draw(m_spriteBatch, fps.c_str(), glm::vec2(0.0f, m_window->getScreenHeight() - (float)m_debugFont.getFontHeight()*0.2f), glm::vec2(0.2f), ALWAYS_ON_TOP + 500.0f, Sakura::ColorRGBA8(255,255,255,255), Sakura::Justification::LEFT);
}

void MainScreen::specificDraw(){
	/* For Elements that require use of a seperate
	* spritebatch, camera, or otherwise need a specific element */
	m_interface.draw(m_GLSLPUniformName, m_game->getFps());
}

/* Process and check for all defined game inputs */
void MainScreen::checkInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_game->onSDLEvent(evnt);
	}
	/* Enable DEBUG mode */
	if (m_game->inputManager.isKeyPressed(KeyID::BACKQUOTE)){
		debug_game_events = !debug_game_events;
	}

	if (m_game->inputManager.isKeyPressed(KeyID::F1)){
		show_boxes = true;
	}

	if (m_game->inputManager.isKeyPressed(KeyID::F2)){
		m_playerFleet.setTurn(!m_playerFleet.getTurn());
	}

	/* Ship selection */
	if (m_game->inputManager.isKeyPressed(KeyID::NUM0)){
		m_playerFleet.setAddedShip(ShipType::CUTTER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM1)){
		m_playerFleet.setAddedShip(ShipType::FIGHTER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM2)){
		m_playerFleet.setAddedShip(ShipType::INTERCEPTOR);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM3)){
		m_playerFleet.setAddedShip(ShipType::BOMBER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM4)){
		m_playerFleet.setAddedShip(ShipType::CORVETTE);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM5)){
		m_playerFleet.setAddedShip(ShipType::CRUISER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM6)){
		m_playerFleet.setAddedShip(ShipType::DESTROYER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM7)){
		m_playerFleet.setAddedShip(ShipType::CARRIER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM8)){
		m_playerFleet.setAddedShip(ShipType::ASSAULT_CARRIER);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::NUM9)){
		m_playerFleet.setAddedShip(ShipType::BATTLESHIP);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::LEFT)){
		ShipType selectType = (ShipType)((int)m_playerFleet.getAddedShip() - 1);
		if ((int)selectType < 0){
			selectType = ShipType::BATTLESHIP;
		}
		m_playerFleet.setAddedShip(selectType);
	}
	if (m_game->inputManager.isKeyPressed(KeyID::RIGHT)){
		ShipType selectType = (ShipType)((int)m_playerFleet.getAddedShip() + 1);
		if ((int)selectType > (int)ShipType::BATTLESHIP){
			selectType = ShipType::CUTTER;
		}
		m_playerFleet.setAddedShip(selectType);
	}

	/* Reset Screen */
	if (m_game->inputManager.isKeyDown(KeyID::KeyMod::LSHIFT) && m_game->inputManager.isKeyDown(KeyID::KeyMod::LCTRL) && m_game->inputManager.isKeyPressed(KeyID::r)){
		onExit();
		onEntry();
	}

	/* Escape current funciton */
	if (m_game->inputManager.isKeyPressed(KeyID::ESCAPE)){
		if (m_playerFleet.getAddedShip() != ShipType::NOSHIP){
			m_playerFleet.setAddedShip(ShipType::NOSHIP);
		}else m_interface.setState((GUIState)!m_interface.getState());
	}

	/* Start placing ship*/
	if (m_game->inputManager.isKeyPressed(KeyID::RETURN)){
		m_placingShips = !m_placingShips;
	}

	//Mouse
	if (m_game->inputManager.isKeyPressed(MouseId::BUTTON_LEFT) && m_placingShips){
		glm::vec2 mouseCoords = m_camera.convertScreenToWorld(glm::vec2(m_game->inputManager.getMouseCoords()));
		int additionalData = (m_playerFleet.getAddedShip() == ShipType::INTERCEPTOR) ? false : true;
		m_playerFleet.addShip(&m_grid, &m_enemyFleet, m_resourceManager, m_playerFleet.getAddedShip(), mouseCoords, m_grid.getGridPos(mouseCoords), additionalData);
	}

	if (m_game->inputManager.isKeyPressed(MouseId::BUTTON_LEFT)){
		glm::vec2 mouseCoords = m_camera.convertScreenToWorld(glm::vec2(m_game->inputManager.getMouseCoords()));
		Ship* selectedShip = m_playerFleet.shipAtPosition(mouseCoords);
		if (selectedShip){
			if (m_playerFleet.getSelectedShip()){
				m_playerFleet.getSelectedShip()->setSelected(false);
			}
			m_playerFleet.setSelectedShip(selectedShip);
			m_playerFleet.getSelectedShip()->setSelected(true);
		}
		else if (m_playerFleet.getSelectedShip()){
			if (!m_enemyFleet.shipAtPosition(mouseCoords)){
				m_playerFleet.getSelectedShip()->move(m_grid.getGridPos(mouseCoords));
			}
		}
		if (debug_game_events){
			glm::vec2 mouseSize = glm::vec2(mouseCoords.x - m_previousMouseLocation.x, mouseCoords.y - m_previousMouseLocation.y);
			printf("Mouse Coords (x, y): %.2f, %.2f\n Size from previous (w, h): %.2f, %.2f\n", mouseCoords.x, mouseCoords.y, mouseSize.x, mouseSize.y);
			m_previousMouseLocation = mouseCoords;
		}
	}
	if (m_game->inputManager.isKeyPressed(MouseId::BUTTON_RIGHT)){
		glm::vec2 mouseCoords = m_camera.convertScreenToWorld(glm::vec2(m_game->inputManager.getMouseCoords()));
		Ship* selectedShip = m_playerFleet.shipAtPosition(mouseCoords);
		if (selectedShip){
			m_playerFleet.removeShip(selectedShip->getID());
		}
	}
	if (m_game->inputManager.isKeyPressed(MouseId::BUTTON_MIDDLE)){
		glm::vec2 mouseCoords = m_camera.convertScreenToWorld(glm::vec2(m_game->inputManager.getMouseCoords().x, m_game->inputManager.getMouseCoords().y));
		Ship* selectedShip = m_playerFleet.shipAtPosition(mouseCoords);
		if (!selectedShip){
			selectedShip = m_enemyFleet.shipAtPosition(mouseCoords);
		}
		if (selectedShip){
			selectedShip->Damage(1, 1, DamageEffect());
		}
	}
}