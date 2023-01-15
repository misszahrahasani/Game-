#pragma once

#include <random>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
// SFML
#include <SFML/Graphics.hpp>

// generate random number between range
int randomRange(int begin, int end) {
  return (rand() % (end - begin)) + begin;
}

using std::vector;
using std::string;

class GameColumn {
 public:
  struct Groove {
    int y;     // top distance
    int width; //
  };

 protected:
  int _x;      // left distance
  int _height; //
  //
  vector<Groove> _grooves;

 public:
  GameColumn(int x, int height, vector<Groove> const& grooves)
    : _x(x),
      _height(height),
      _grooves(grooves) {
  }

  static GameColumn createRandom(int x, int height) {
    //
    vector<Groove> grooves;
    int y = (int) randomRange(0,  5);
    do {
      // check if groove is possible to create or not
      if (!grooves.empty() and y + 2 >= height)
        break;
      grooves.push_back(Groove{y, 2});
      // move to next groove
      y += (int) randomRange(2, 10) + 2;  // 2 is groove width
    } while (true);
    //
    return GameColumn{x, height, grooves};
  }

  // check if point is on column(not on grooves!)
  bool onColumn(int x, int y) const {
    if (_x == x) {
      int begin = 0;
      for (auto groove : _grooves) {
        if (begin <= y and y <= groove.y - 1)
          return true;
        // move to next groove
        begin = groove.y + groove.width;
      }
      if (begin < _height)
        return begin <= y;
    }
    return false;
  }

  int getX() const { return _x; }

  vector<Groove> getGrooves() const { return _grooves; }
};

class GameBoard {
 private:
  int _width,
      _height;
  vector<GameColumn> _columns;

 public:
  explicit GameBoard(int width, int height, vector<GameColumn> columns)
    : _width(width),
      _height(height),
      _columns(std::move(columns)) {
  }

  static GameBoard createRandom(int width, int height, int beginOffset, int endOffset) {
    int x = beginOffset;
    vector<GameColumn> columns;
    //
    columns.push_back(GameColumn(beginOffset, height, {GameColumn::Groove{height/2, 1}}));
    //
    do {
      //                     minGap   maxGap
      x += (int) randomRange(1      , 3) + 1; // gap between two columns
      //
      if (x >= width - endOffset - 1)
        break;
      columns.push_back(GameColumn::createRandom(x, height));
    } while (true);
    //
    columns.push_back(GameColumn(width-endOffset, height, {GameColumn::Groove{height/2, 1}}));
    //
    return GameBoard{width, height, columns};
  }

  bool onColumn(int x, int y) const {
    for (auto const& column : _columns)
      if (column.onColumn(x, y))
        return true;
    return false;
  }

  int getWidth()  const { return _width; }
  int getHeight() const { return _height; }

  vector<GameColumn> getColumns() const { return _columns; }
};

class GamePlayer {
 private:
  int _x,
      _y;

 public:
  GamePlayer(int x, int y)
    : _x(x),
      _y(y) {
 }

  bool move(int xO, int yO, GameBoard const& board) {
    // calculate final position
    int x = _x + xO,
        y = _y + yO;
    // check if position is on board
    if (0 <= x and x <= board.getWidth() and
        0 <= y and y < board.getHeight()) {
      // check if position isn't on column
      if (not board.onColumn(x, y)) {
        _x = x;
        _y = y;
        //
        return true;
      }
    }
    return false;
  }

  int getX() const { return _x; }
  int getY() const { return _y; }
};

class GameRender : public sf::RenderWindow {
 private:
  GameBoard  &_board;
  GamePlayer &_player;

 public:
  GameRender(GameBoard &board, GamePlayer &player)
    : sf::RenderWindow(sf::VideoMode(pixelSize * board.getWidth(), pixelSize * board.getHeight()), "Game window"),
      //
      _board (board),
      _player(player) {
  }

  void render() {
    static auto block   = _loadImage("Assets/Images/grass.png"  , pixelSize);
    static auto vehicle = _loadImage("Assets/Images/vehicle.png", pixelSize);

    for (auto const& column : _board.getColumns()) {
      int x = column.getX();
      int y = 0;
      //
      int grooveIndex = 0;
      for (auto groove : column.getGrooves()) {
        for (int i=y; i<groove.y; ++i)
          block->setPosition((float) x * pixelSize, (float) i * pixelSize), draw(*block);
        //
        y = groove.y + groove.width;
        // move to next groove
        ++grooveIndex;
      }
      // draw last groove border
      for (int i=y; i<_board.getHeight(); ++i)
        block->setPosition((float) x * pixelSize, (float) i * pixelSize), draw(*block);
    }
    // draw player vehicle
    vehicle->setPosition((float) _player.getX() * pixelSize, (float) _player.getY() * pixelSize), draw(*vehicle);
  }

  void renderWon(int seconds, int score) {
    string message = "Result: \n"
                     "+ Elapsed time: " + std::to_string(seconds) + "s\n"
                     "+ Score: " + std::to_string(score) + " (Score 0 is normal)";
    //
    sf::Text text;
    sf::Font font;
    //
    if(font.loadFromFile("Assets/font.ttf")) {
      text.setFont(font);
      text.setString(message);
      text.setCharacterSize(35);
      text.setOutlineThickness(2);
      text.setOutlineColor(sf::Color::Red);
      //
      text.setPosition(30, 30), draw(text);
      //
      display();
    }
  }

 private:
  static sf::Sprite* _loadImage(const string& path, float size) {
    auto *texture = new sf::Texture;
    auto *sprite  = new sf::Sprite;
    // load texture
    texture->loadFromFile(path);
    // create sprite
    sprite->setTexture(*texture);
    sprite->setScale({size / (float) texture->getSize().x,
                      size / (float) texture->getSize().y});
    return sprite;
  }
};