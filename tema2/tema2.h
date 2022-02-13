#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/camera.h"
#include "lab_m1/tema2/shapes.h"
#include "lab_m1/tema2/maze_generator.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

        static constexpr float MAX_GUN_COOLDOWN = 0.5f;
        static constexpr float MAX_TIMER = 60.5;
        static const int MAX_TIMER_BARS = 12;

        static const int MAX_PROJECTILE_DISTANCE = 20;
        static const int BULLET_SPEED = 10;
        static const int MAZE_HEIGHT = 11;
        static const int MAZE_WIDTH = 11;
        static const int WALL_SIDE = 4;
        static const int NUM_ENEMIES = 10;
        static const int MAX_HP = 5;
        static const int ENEMY_SCALE = 2;
        static const int PLANE_SCALE = 2;
        static const int ENEMY_STEP_LENGTH = 2;
        static constexpr float MAX_HP_FLOAT = 5.0f;
        static constexpr float MAX_TIMER_BARS_FLOAT = 12.0f;
        static constexpr float GUI_BAR_SCALE = 2.0f;
        static constexpr float TIME_TO_DIE = 3;
        static constexpr float ENEMY_STEP = 0.1;
        static constexpr float CAMERA_SPEED = 3.0f;;

        static constexpr float BULLET_SPAWN_HEIGHT = 2.2;
        static constexpr float BULLET_SCALE = 0.5;
        static constexpr float BULLET_RADIUS = 0.5;

        static constexpr float SWITCH_PERSON_FORWARD_DISTANCE = 4.7;
        static constexpr float SWITCH_PERSON_HEIGHT_DISTANCE = -2;

        static constexpr float HEALTH_BAR_WORLD_X = -10;
        static constexpr float HEALTH_BAR_WORLD_Z = -10;
        static constexpr float TIMER_BAR_WORLD_X = -20;
        static constexpr float TIMER_BAR_WORLD_Z = -20;
    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMeshSimple(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(1));

        void resetGame();

        void renderGame(float deltaTimeSeconds);
        void renderProjectiles(float deltaTimeSeconds);
        void renderEnemies(float deltaTimeSeconds);
        void renderPlayer();
        void renderEnviroment();
        void renderHUD();

        void runDeleteLogic();
        inline void updateTimers(float deltaTimeSeconds);
        void checkEndGame();

        void renderViewport(GLint x, GLint y, GLsizei width, GLsizei height);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        bool boxToBoxCollision(tema2::Box& box1, tema2::Box& box2);
        bool boxToSphereCollision(tema2::Box& box, tema2::Sphere& sphere);
        bool sphereToSphereCollision(tema2::Sphere& sphere1, tema2::Sphere& sphere2);
        bool playerWallCollisions(float futureX, float futureZ);
        bool projectileEnviromentCollisions(tema2::Sphere& projectileSphere);

    protected:
        cam::Camera* camera;
        cam::Camera* hpCamera;
        cam::Camera* timerCamera;
        glm::mat4 projectionMatrix = glm::perspective(RADIANS(60.0f), window->props.aspectRatio, 0.01f, 200.f);
        glm::mat4 hpProjectionOrtho = glm::ortho(-MAX_HP_FLOAT, MAX_HP_FLOAT, -GUI_BAR_SCALE / 2, GUI_BAR_SCALE / 2, 0.01f, 200.0f);
        glm::mat4 timerProjectionOrtho = glm::ortho(-MAX_TIMER_BARS_FLOAT, MAX_TIMER_BARS_FLOAT, -GUI_BAR_SCALE / 2, GUI_BAR_SCALE / 2, 0.01f, 200.0f);

        glm::mat4 modelMatrix = glm::mat4(1);
        tema2::Box playerBox;
        float worldPlayerX = 0;
        float worldPlayerZ = 0;
        float currPlayerX = 0;
        float currPlayerZ = 0;
        float playerAngularRotation = 0;
        int playerHp = MAX_HP;

        float currTimer = MAX_TIMER;

        float healthBarViewportX;
        float healthBarViewportY;
        float healthBarViewportWidth = 300;
        float healthBarViewportHeight = 100;

        float timerBarViewportX;
        float timerBarViewportY;
        float timerBarViewportWidth = 300;
        float timerBarViewportHeight = 100;

        bool isFirstPersonMode = false;
        glm::vec3 prevForward = glm::vec3(0, 0, -1);
        glm::vec3 prevUp = glm::vec3(0, 1, 0);

        float gunCooldownSeconds = 0;
        int projectileId = 0;

        std::vector<tema2::Box> walls;
        std::vector<std::pair<int, int>> freeMazeSpaces;
        std::vector<bool> takenFreeMazeSpaces;

        std::unordered_map<int, tema2::Enemy> enemies;
        std::vector<int> enemiesToDelete;
        int enemiesToGenerate = NUM_ENEMIES;
        int enemyId = 0;

        std::unordered_map<int, tema2::Projectile> projectiles;
        std::vector<int> projectilesToDelete;
    };
}   // namespace m1
