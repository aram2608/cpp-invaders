#include <raylib.h>

constexpr float WINDOW_WIDTH = 800.0f;
constexpr float WINDOW_HEIGHT = 800.0f;

// InputSystem
// InvaderMovementSystem
// BulletMovementSystem
// InvaderShootSystem
// CollisionSystem         dispatcher
// ScoreSystem             dispatcher
// GameStateSystem         game over? wave complete?
// RenderSystem
// CleanupSystem

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "space invaders");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // update

    // draw
    BeginDrawing();
    ClearBackground({255, 255, 255, 255});

    EndDrawing();

    // cleanup
  }

  CloseWindow();
  return 0;
}
