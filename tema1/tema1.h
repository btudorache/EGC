#pragma once

#include "components/simple_scene.h"
#include "shapes.h"
#include <vector>
#include <unordered_map>


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

        static constexpr float MAIN_CHARACTER_SIDE = 60;
        static constexpr float ENEMY_SIDE = 60;
        static constexpr float PROJECTILE_SIDE = 20;
        static constexpr float OBSTACLE_SIDE = 100;
        static constexpr float HAND_SIDE = 20;
        static constexpr float HEALTH_BAR_SIDE = 20;

        static const int BOTTOM_LEFT_SPAWN_ZONE = 0;
        static const int BOTTOM_RIGHT_SPAWN_ZONE = 3;
        static const int TOP_LEFT_SPAWN_ZONE = 2;
        static const int TOP_RIGHT_SPAWN_ZONE = 1;
        static const int NUM_ZONES = 4;
        static const int MAX_SPAWN_OFFSET = 100;

        static constexpr float MAX_GUN_COOLDOWN = 0.5f;
        static const int MAX_PROJECTILE_DISTANCE = 200;
        static const int PROJECTILE_OFFSET_X = 20;
        static const int PROJECTILE_OFFSET_Y = -10;
        static const int BULLET_SPEED = 200;

        static const int PLAYER_SPEED = 150;
        static const int BASE_ENEMY_SPEED = 50;

        static const int SPAWN_ENEMY_TIME = 3;
        static const int MIN_ENEMY_SPEED = 2;
        static const int MAX_ENEMY_ADDITIONAL_SPEED = 6;

        static const int MAX_HEALTH = 10;
        static const int HEALTHBAR_OFFSET_X = 130;
        static const int HEALTHBAR_OFFSET_Y = 155;

        static const int FIRST_HAND_OFFSET_X = 5;
        static const int FIRST_HAND_OFFSET_Y = -10;
        static const int SECOND_HAND_OFFSET_X = 35;
        static const int SECOND_HAND_OFFSET_Y = -10;

        static const int FIRST_OBSTACLE_X = 100;
        static const int FIRST_OBSTACLE_Y = 200;
        static const int SECOND_OBSTACLE_X = 100;
        static const int SECOND_OBSTACLE_Y = 500;
        static const int THIRD_OBSTACLE_X = 1000;
        static const int THIRD_OBSTACLE_Y = 100;
        static const int FOURTH_OBSTACLE_X = 800;
        static const int FOURTH_OBSTACLE_Y = 500;

        static const int CIRCLE_NUM_SIDES = 40;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void renderGame(float deltaTimeSeconds);
        void renderProjectiles(float deltaTimeSeconds);
        void renderEnemies();
        void renderPlayer();
        void renderEnviroment();
        void runDeleteLogic();
        void runSpawnLogic(float deltaTimeSeconds);
        void resetGame();
        inline void updateTimers(float deltaTimeSeconds);

        bool checkBoundsAndObstacleCollisions(Circle& circle, float dx, float dy);
     protected:
        float cameraX = 0;
        float cameraY = 0;

        float gunCooldownSeconds = 0;
        int projectileId = 0;
        float projectileCx = 10;
        float projectileCy = PROJECTILE_SIDE / 2 + MAIN_CHARACTER_SIDE / 2;;

        float spawnEnemyTimer = SPAWN_ENEMY_TIME;
        int enemyId = 0;

        int score = 0;
        int currHealth = MAX_HEALTH;

        float cx = MAIN_CHARACTER_SIDE / 2;
        float cy = MAIN_CHARACTER_SIDE / 2;
        float firstHandCx = HAND_SIDE + 5;
        float firstHandCy = HAND_SIDE / 2 + MAIN_CHARACTER_SIDE / 2;
        float secondHandCx = -5;
        float secondHandCy = HAND_SIDE / 2 + MAIN_CHARACTER_SIDE / 2;

        glm::mat3 modelMatrix = glm::mat3(1);
        glm::vec3 corner = glm::vec3(0, 0, 0);
        float angularStep = 0;

        Circle mainPlayerCircle;

        std::vector<Rectangle> obstacles;

        std::unordered_map<int, Enemy> enemies;
        std::vector<int> enemiesToDelete;

        std::unordered_map<int, Projectile> projectiles;
        std::vector<int> projectilesToDelete;
    };

    bool isOutOfBounds(Circle& player, WindowObject* window);

    bool checkBallToRectCollision(Circle& circle, Rectangle& rect);

    double calculateArctan(glm::vec2 targetPoint, glm::vec2 centerPoint);

}   // namespace m1

