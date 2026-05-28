#include "entt/entity/fwd.hpp"
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

struct ShipTag {};

struct Renderable {
  // Placeholder for now will index into a sprite array
  int sprite_id;
  Color color;
};

struct ShipStats {
  int lives = 3;
  float speed = 100.0f;
};

struct Position {
  int x, y;
};

struct Box {
  int w, h;
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
    reg.emplace<Renderable>(e, 0, GREEN);
    reg.emplace<ShipStats>(e);
    reg.emplace<Position>(e, ship_x, ship_y);
    reg.emplace<Box>(e, SHIP_W, SHIP_H);
  }
};

void RenderSystem(entt::registry &reg) {
  auto view = reg.view<Position, Box, Renderable>();
  for (auto [e, pos, box, r] : view.each()) {
    DrawRectangle(pos.x, pos.y, box.w, box.h, r.color);
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

  while (!WindowShouldClose()) {
    // update

    // draw
    BeginDrawing();
    ClearBackground({255, 255, 255, 255});

    RenderSystem(registry);

    EndDrawing();

    // cleanup
  }

  CloseWindow();
  return 0;
}
