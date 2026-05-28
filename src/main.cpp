#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include <array>
#include <entt/entt.hpp>
#include <raylib.h>

constexpr float WINDOW_WIDTH = 800.0f;
constexpr float WINDOW_HEIGHT = 800.0f;
constexpr int SHIP_W = 30;
constexpr int SHIP_H = 30;
constexpr int SHIP_X = WINDOW_WIDTH / 2 - SHIP_W / 2;
constexpr int SHIP_Y = WINDOW_HEIGHT - SHIP_H - 40;
constexpr int BULLET_W = 10;
constexpr int BULLET_H = 10;

// InputSystem
// InvaderMovementSystem
// BulletMovementSystem
// InvaderShootSystem
// CollisionSystem         dispatcher
// ScoreSystem             dispatcher
// GameStateSystem         game over? wave complete?
// RenderSystem
// CleanupSystem

enum class Sprites { Ship, Alien, UFO, Bullet, Count };

struct ShipTag {};

struct Renderable {
  Sprites sprite;
};

struct ShipStats {
  int lives = 3;
  float speed = 200.0f;
  float cool_rem = 0.0f;
  float cool_max = 20.0f;
};

struct Position {
  float x, y;
};

struct Box {
  int w, h;
};

struct Bullet {
  enum class From { Ship, Invader };
};

struct Invader {};

struct Fleet {
  bool drop = false;
  float drop_depth = 10.0f;
  int dir = 1;
  float speed = 100.0f;
};

struct Destroy {};

class Spawner {
  entt::registry &reg;
  float ship_x_;
  float ship_y_;
  int alien_w_;
  int alien_h_;

public:
  Spawner(entt::registry &reg, float ship_x, float ship_y, int alien_w,
          int alien_h)
      : reg(reg), ship_x_(ship_x), ship_y_(ship_y), alien_w_(alien_w),
        alien_h_(alien_h) {}

  void SpawnShip() {
    auto e = reg.create();
    reg.emplace<ShipTag>(e);
    reg.emplace<Renderable>(e, Sprites::Ship);
    reg.emplace<ShipStats>(e);
    reg.emplace<Position>(e, ship_x_, ship_y_);
    reg.emplace<Box>(e, SHIP_W, SHIP_H);
  }

  void SpawnBullet(float x, float y, Bullet::From from) {
    auto e = reg.create();
    reg.emplace<Bullet>(e, from);
    reg.emplace<Position>(e, x, y);
    reg.emplace<Renderable>(e, Sprites::Bullet);
    reg.emplace<Box>(e, BULLET_W, BULLET_H);
  }

  void SpawnFleet(int rows, int cols) {
    const int pad_x = 10, pad_y = 10;
    const int stride_x = alien_w_ + pad_x;
    const int stride_y = alien_h_ + pad_y;
    const float origin_x = (WINDOW_WIDTH - (cols * stride_x - pad_x)) / 2.0f;
    const float origin_y = 60.0f;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        auto e = reg.create();
        reg.emplace<Invader>(e);
        reg.emplace<Box>(e, alien_w_, alien_h_);
        reg.emplace<Position>(e, origin_x + c * stride_x,
                              origin_y + r * stride_y);
        reg.emplace<Renderable>(e, Sprites::Alien);
      }
    }
  }
};

using TextureArray = std::array<Texture, static_cast<int>(Sprites::Count)>;

void RenderSystem(entt::registry &reg, const TextureArray &texts) {
  auto view = reg.view<Position, Box, Renderable>();
  for (auto [e, pos, box, r] : view.each()) {
    switch (r.sprite) {
      // This is a fragile hack
      // an index into TextureArray with this index would return junk
    case Sprites::Bullet:
      DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y), box.w,
                    box.h, YELLOW);
      break;
    default:
      DrawTexture(texts[static_cast<int>(r.sprite)], static_cast<int>(pos.x),
                  static_cast<int>(pos.y), WHITE);
      break;
    }
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
  if (IsKeyDown(KEY_LEFT) && p.x > 10) p.x -= s.speed * dt;

  int rb = WINDOW_WIDTH - 10 - SHIP_W;
  if (IsKeyDown(KEY_RIGHT) && p.x < rb) p.x += s.speed * dt;

  if (IsKeyPressed(KEY_SPACE) && s.cool_rem <= 0.0f) {
    sp.SpawnBullet(p.x, p.y, Bullet::From::Ship);
    s.cool_rem = s.cool_max;
  }

  if (s.cool_rem > 0.0f) s.cool_rem -= dt;
}

void InvaderMovementSystem(entt::registry &reg, int alien_w, float dt) {
  auto &fs = reg.ctx().get<Fleet>();
  auto view = reg.view<Invader, Position>();
  bool hit_wall = false;
  for (auto [e, pos] : view.each()) {
    if ((fs.dir > 0 && pos.x + alien_w >= WINDOW_WIDTH) ||
        (fs.dir < 0 && pos.x <= 0))
      hit_wall = true;
  }
  if (hit_wall) {
    fs.drop = true;
    fs.dir *= -1;
  }
  for (auto [e, pos] : view.each()) {
    if (fs.drop) {
      pos.y += fs.drop_depth;
    } else {
      pos.x += fs.dir * fs.speed * dt;
    }
  }
  fs.drop = false;
}

int main() {
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "space invaders");
  SetTargetFPS(60);

  TextureArray texts = LoadAllTextures();

  int ALIEN_W = texts[static_cast<int>(Sprites::Alien)].width;
  int ALIEN_H = texts[static_cast<int>(Sprites::Alien)].height;

  entt::registry registry;
  registry.ctx().emplace<Fleet>();

  entt::dispatcher dispatcher;
  Spawner spawner{registry, SHIP_X, SHIP_Y, ALIEN_W, ALIEN_H};

  spawner.SpawnShip();
  spawner.SpawnFleet(8, 9);

  while (!WindowShouldClose()) {
    // update

    float dt = GetFrameTime();

    InputSystem(registry, dt, spawner);
    InvaderMovementSystem(registry, ALIEN_W, dt);

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
