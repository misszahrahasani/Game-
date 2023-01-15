#include <iostream>
#include <ctime>

#define pixelSize 35
#include "GameObjects.h"

const int renderWidth  = 26;
const int renderHeight = 20;
//
const int optimalTime = 10;

const sf::Keyboard::Key movementKeys[4] = {
           sf::Keyboard::Up,
    sf::Keyboard::Left, sf::Keyboard::Right,
           sf::Keyboard::Down,
};

int main() {
  srand(std::time(nullptr));
  // create game elements
  auto gameBoard  = GameBoard::createRandom(renderWidth, renderHeight, 3, 1);
  auto gamePlayer = GamePlayer(1, renderHeight / 2);
  // game render
  GameRender gameRender(gameBoard, gamePlayer);

  bool wins = false;
  //
  time_t beginTime = 0;
  do {
    sf::Event event{};
    while (gameRender.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        gameRender.close();
    }
    //
    gameRender.clear({110,110,110});
    gameRender.render();
    gameRender.display();
    // movement with WASD
    for (int key=0; key<4; ++key) {
      if (sf::Keyboard::isKeyPressed(movementKeys[key])) {
        while (sf::Keyboard::isKeyPressed(movementKeys[key]));
        //
        // start the game if it's not started
        if (beginTime == 0)
          beginTime = std::time(nullptr);
        switch (key) {
          case 0: gamePlayer.move(0 , -1, gameBoard); break; // up
          case 1: gamePlayer.move(-1, 0 , gameBoard); break; // left
          case 2: gamePlayer.move(1 , 0 , gameBoard); break; // right
          case 3: gamePlayer.move(0 , 1 , gameBoard); break; // down
          default:;
        }
      }
    }
    if (gamePlayer.getX() == gameBoard.getWidth() - 1)
      wins = true;
  } while (not (wins and gameRender.isOpen()));
  //
  if (wins) {
    // calculate
    time_t winTime  = std::time(nullptr) - beginTime;
    int    winScore = optimalTime - winTime;
    // show result
    gameRender.clear({110,110,110});
    gameRender.render();
    gameRender.renderWon(winTime, winScore);
    sf::sleep(sf::seconds(5));
  }
}
