#include "entt/entity/entity.hpp"
#include <array>
#include <entt/entt.hpp>
#include <raylib.h>

constexpr float WINDOW_WIDTH = 800.0f;
constexpr float WINDOW_HEIGHT = 800.0f;
constexpr int SHIP_W = 30;
constexpr int SHIP_H = 30;
constexpr int SHIP_X = WINDOW_WIDTH / 2 - SHIP_W / 2;
constexpr int SHIP_Y = WINDOW_HEIGHT - SHIP_H - 40;

// InputSystem
// InvaderMovementSystem
// BulletMovementSystem
// InvaderShootSystem
// CollisionSystem         dispatcher
// ScoreSystem             dispatcher
// GameStateSystem         game over? wave complete?
// RenderSystem
// CleanupSystem

enum class Sprites { Ship, Alien, UFO, Count };

struct ShipTag {};

struct Renderable {
  // Placeholder for now will index into a sprite array
  Sprites sprite;
};

struct ShipStats {
  int lives = 3;
  float speed = 200.0f;
  float cool_rem = 0.0f;
  float cool_max = 20.0f;
};

struct Position {
  int x, y;
};

struct Box {
  int w, h;
};

struct Bullet {
  enum class From { Ship, Invader };
};

class Spawner {
  entt::registry &reg;
  int ship_x;
  int ship_y;

public:
  Spawner(entt::registry &reg, int ship_x, int ship_y)
      : reg(reg), ship_x(ship_x), ship_y(ship_y) {}

  void SpawnShip() {
    auto e = reg.create();
    reg.emplace<ShipTag>(e);
    reg.emplace<Renderable>(e, Sprites::Ship);
    reg.emplace<ShipStats>(e);
    reg.emplace<Position>(e, ship_x, ship_y);
    reg.emplace<Box>(e, SHIP_W, SHIP_H);
  }

  void SpawnBullet(int x, int y, Bullet::From from) {}
};

using TextureArray = std::array<Texture, static_cast<int>(Sprites::Count)>;

void RenderSystem(entt::registry &reg, const TextureArray &texts) {
  auto view = reg.view<Position, Box, Renderable>();
  for (auto [e, pos, box, r] : view.each()) {
    DrawTexture(texts[static_cast<int>(r.sprite)], pos.x, pos.y, WHITE);
  }
}

TextureArray LoadAllTextures() {
  Image ship_image = LoadImage("./assets/icons/ship.png");
  Image alien_image = LoadImage("./assets/icons/alien.png");
  Image ufo_image = LoadImage("./assets/icons/ufo.png");
  TextureArray texts = {
      LoadTextureFromImage(ship_image),
      LoadTextureFromImage(alien_image),
      LoadTextureFromImage(ufo_image),
  };

  UnloadImage(ship_image);
  UnloadImage(alien_image);
  UnloadImage(ufo_image);

  return texts;
};

void InputSystem(entt::registry &reg, float dt, Spawner &sp) {
  auto view = reg.view<ShipTag, ShipStats, Position>();

  auto entity = view.front();

  if (entity == entt::null) return;

  auto [s, p] = view.get<ShipTag, ShipStats, Position>(entity);

  if (IsKeyDown(KEY_LEFT) && p.x > 10) {
    p.x -= s.speed * dt;
  }

  if (IsKeyDown(KEY_RIGHT) && p.x < WINDOW_WIDTH - 10 - SHIP_W) {
    p.x += s.speed * dt;
  }

  if (IsKeyPressed(KEY_SPACE) && s.cool_rem <= 0.0f) {
    sp.SpawnBullet(p.x, p.y, Bullet::From::Ship);
    s.cool_rem = s.cool_max;
  }

  if (s.cool_rem > 0.0f) {
    s.cool_rem -= dt;
  }
}

int main() {

  entt::registry registry;
  entt::dispatcher dispatcher;
  Spawner spawner{registry, SHIP_X, SHIP_Y};

  spawner.SpawnShip();

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "space invaders");
  SetTargetFPS(60);

  TextureArray texts = LoadAllTextures();

  while (!WindowShouldClose()) {
    // update

    float dt = GetFrameTime();

    InputSystem(registry, dt, spawner);

    // draw
    BeginDrawing();
    ClearBackground({255, 255, 255, 255});

    RenderSystem(registry, texts);

    EndDrawing();

    // cleanup
  }

  for (auto &t : texts) UnloadTexture(t);
  CloseWindow();
  return 0;
}
