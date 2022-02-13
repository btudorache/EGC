#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/tema_object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{   
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x / 2, 0, (float)resolution.y / 2, 0.01f, 400);
    camera->SetPosition(glm::vec3(cameraX, cameraY, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    float initialPlayerX = (float)resolution.x / 4 - cx;
    float initialPlayerY = (float)resolution.y / 4 - cy;

    // initialize obstacles coords
    obstacles.push_back(Rectangle(FIRST_OBSTACLE_X, FIRST_OBSTACLE_Y, OBSTACLE_SIDE,  2 * OBSTACLE_SIDE, 1, 2));
    obstacles.push_back(Rectangle(SECOND_OBSTACLE_X, SECOND_OBSTACLE_Y, 3 * OBSTACLE_SIDE, OBSTACLE_SIDE, 3, 1));
    obstacles.push_back(Rectangle(THIRD_OBSTACLE_X, THIRD_OBSTACLE_Y, OBSTACLE_SIDE, 3 * OBSTACLE_SIDE, 1, 3));
    obstacles.push_back(Rectangle(FOURTH_OBSTACLE_X, FOURTH_OBSTACLE_Y, 2 * OBSTACLE_SIDE, OBSTACLE_SIDE, 2, 1));
    
    // initialize player circle
    mainPlayerCircle = m1::Circle((float)resolution.x / 4 - cx, (float)resolution.y / 4 - cy, MAIN_CHARACTER_SIDE / 2, 1, 1);

    // initialize meshes
    Mesh* map = tema_object2D::CreateRectangle("map", glm::vec3(0, 0, -10), (float)resolution.x, (float)resolution.y, glm::vec3(0.76, 0.76, 0.63), true);
    AddMeshToList(map);

    Mesh* healthBar = tema_object2D::CreateSquare("healthBar", glm::vec3(0, 0, 10), HEALTH_BAR_SIDE, glm::vec3(0.5, 0, 0), true);
    AddMeshToList(healthBar);

    Mesh* healthBarGrid = tema_object2D::CreateSquare("healthBarGrid", glm::vec3(0, 0, 10), HEALTH_BAR_SIDE, glm::vec3(0.5, 0, 0), false);
    AddMeshToList(healthBarGrid);

    Mesh* obstacle = tema_object2D::CreateSquare("obstacle", corner, OBSTACLE_SIDE, glm::vec3(0, 0.6, 0), true);
    AddMeshToList(obstacle);

    Mesh* projectile = tema_object2D::CreateCircle("projectile", corner, PROJECTILE_SIDE / 2, CIRCLE_NUM_SIDES, glm::vec3(0, 0, 0), true);
    AddMeshToList(projectile);

    Mesh* mainCharacter = tema_object2D::CreateCircle("mainCharacter", corner, MAIN_CHARACTER_SIDE / 2, CIRCLE_NUM_SIDES, glm::vec3(1, 0.6, 0.2), true);
    AddMeshToList(mainCharacter);

    Mesh* enemy = tema_object2D::CreateSquare("enemy", corner, ENEMY_SIDE, glm::vec3(0.69, 0.13, 0.13), true);
    AddMeshToList(enemy);

    Mesh* enemyHand = tema_object2D::CreateSquare("enemyHand", corner, HAND_SIDE, glm::vec3(0.54, 0, 0), true);
    AddMeshToList(enemyHand);

    Mesh* hand = tema_object2D::CreateCircle("hand", corner, HAND_SIDE / 2, CIRCLE_NUM_SIDES, glm::vec3(1, 0.4, 0), true);
    AddMeshToList(hand);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{   
    // camera rendering
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(cameraX, cameraY, 50));

    renderGame(deltaTimeSeconds);

    runDeleteLogic();

    runSpawnLogic(deltaTimeSeconds);

    updateTimers(deltaTimeSeconds);

    if (currHealth == 0) {
        resetGame();
    }
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{   
    if (window->KeyHold(GLFW_KEY_W) && !checkBoundsAndObstacleCollisions(mainPlayerCircle, 0, PLAYER_SPEED * deltaTime)) {
        cameraY += PLAYER_SPEED * deltaTime;
        mainPlayerCircle.y += PLAYER_SPEED * deltaTime;
    }
    else if (window->KeyHold(GLFW_KEY_S) && !checkBoundsAndObstacleCollisions(mainPlayerCircle, 0, -PLAYER_SPEED * deltaTime)) {
        cameraY -= PLAYER_SPEED * deltaTime;
        mainPlayerCircle.y -= PLAYER_SPEED * deltaTime;
    }
    else if (window->KeyHold(GLFW_KEY_A) && !checkBoundsAndObstacleCollisions(mainPlayerCircle, -PLAYER_SPEED * deltaTime, 0)) {
        cameraX -= PLAYER_SPEED * deltaTime;
        mainPlayerCircle.x -= PLAYER_SPEED * deltaTime;
    }
    else if (window->KeyHold(GLFW_KEY_D) && !checkBoundsAndObstacleCollisions(mainPlayerCircle, PLAYER_SPEED * deltaTime, 0)) {
        cameraX += PLAYER_SPEED * deltaTime;
        mainPlayerCircle.x += PLAYER_SPEED * deltaTime;
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
   
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 resolution = window->GetResolution();

    int cameraY = resolution.y - mouseY;
    int cameraX = mouseX;

    angularStep = M_PI_2 + calculateArctan(glm::vec2(cameraX, cameraY), glm::vec2(resolution.x / 2, resolution.y / 2));
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && gunCooldownSeconds <= 0) {
        projectiles[projectileId] = Projectile(projectileId, mainPlayerCircle.x, mainPlayerCircle.y, projectileCx, projectileCy, angularStep, 0);
        projectileId += 1;
        gunCooldownSeconds = MAX_GUN_COOLDOWN;
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}

void m1::Tema1::renderGame(float deltaTimeSeconds)
{
    renderProjectiles(deltaTimeSeconds);
    renderEnemies();
    renderPlayer();
    renderEnviroment();
}

void m1::Tema1::renderProjectiles(float deltaTimeSeconds)
{
    for (auto& it : projectiles) {
        Projectile proj = it.second;

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(proj.prevPlayerX + PROJECTILE_OFFSET_X, proj.prevPlayerY + PROJECTILE_OFFSET_Y);

        modelMatrix *= transform2D::Translate(proj.projectileCx, proj.projectileCy);
        modelMatrix *= transform2D::Rotate(proj.prevAngularStep);
        modelMatrix *= transform2D::Translate(-proj.projectileCx, -proj.projectileCy);

        modelMatrix *= transform2D::Translate(0, -proj.projectileDistance);

        RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);

        modelMatrix *= transform2D::Rotate(-proj.prevAngularStep);

        Circle projectileCircle = Circle(modelMatrix[2][0], modelMatrix[2][1], PROJECTILE_SIDE / 2, 1, 1);

        if (proj.projectileDistance > MAX_PROJECTILE_DISTANCE || checkBoundsAndObstacleCollisions(projectileCircle, -(PROJECTILE_SIDE / 2), -(PROJECTILE_SIDE / 2))) {
            projectilesToDelete.push_back(it.first);
        }

        for (auto& enm : enemies) {
            if (checkBallToRectCollision(projectileCircle, enm.second.rect)) {
                projectilesToDelete.push_back(it.first);
                enemiesToDelete.push_back(enm.second.id);
                score += 1;
                cout << "Score: " << score << "\n";
            }
        }

        projectiles.at(proj.id).projectileDistance += BULLET_SPEED * deltaTimeSeconds;
    }
}

void m1::Tema1::renderEnemies()
{
    for (auto& it : enemies) {
        Enemy enemy = it.second;

        float playerCenterX = mainPlayerCircle.x + mainPlayerCircle.radius;
        float playerCenterY = mainPlayerCircle.y + mainPlayerCircle.radius;

        float enemyCenterX = enemy.rect.x + cx;
        float enemyCenterY = enemy.rect.y + cy;

        float enemyRotateAngle = calculateArctan(glm::vec2(playerCenterX, playerCenterY), glm::vec2(enemyCenterX, enemyCenterY));

        // first enemy hand
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(enemy.rect.x + FIRST_HAND_OFFSET_X, enemy.rect.y + FIRST_HAND_OFFSET_Y);

        modelMatrix *= transform2D::Translate(firstHandCx, firstHandCy);
        modelMatrix *= transform2D::Rotate(enemyRotateAngle + M_PI_2);
        modelMatrix *= transform2D::Translate(-firstHandCx, -firstHandCy);
        RenderMesh2D(meshes["enemyHand"], shaders["VertexColor"], modelMatrix);

        // second enemy hand
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(enemy.rect.x + SECOND_HAND_OFFSET_X, enemy.rect.y + SECOND_HAND_OFFSET_Y);

        modelMatrix *= transform2D::Translate(secondHandCx, secondHandCy);
        modelMatrix *= transform2D::Rotate(enemyRotateAngle + M_PI_2);
        modelMatrix *= transform2D::Translate(-secondHandCx, -secondHandCy);
        RenderMesh2D(meshes["enemyHand"], shaders["VertexColor"], modelMatrix);

        // rendering enemy body
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(enemy.rect.x, enemy.rect.y);

        modelMatrix *= transform2D::Translate(cx, cy);
        modelMatrix *= transform2D::Rotate(enemyRotateAngle + M_PI_2);
        modelMatrix *= transform2D::Translate(-cx, -cy);

        RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);

        float distX = enemy.speed * cos(enemyRotateAngle);
        float distY = enemy.speed * sin(enemyRotateAngle);

        if (!isOutOfBounds(Circle(enemy.rect.x + distX, enemy.rect.y + distY, enemy.rect.width / 2, 1, 1), window)) {
            enemies.at(enemy.id).rect.x += distX;
            enemies.at(enemy.id).rect.y += distY;
        }

        if (checkBallToRectCollision(mainPlayerCircle, enemy.rect)) {
            enemiesToDelete.push_back(enemy.id);
            currHealth -= 1;
        }
    }
}

void m1::Tema1::renderPlayer()
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mainPlayerCircle.x + FIRST_HAND_OFFSET_X, mainPlayerCircle.y + FIRST_HAND_OFFSET_Y);

    modelMatrix *= transform2D::Translate(firstHandCx, firstHandCy);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-firstHandCx, -firstHandCy);
    RenderMesh2D(meshes["hand"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mainPlayerCircle.x + SECOND_HAND_OFFSET_X, mainPlayerCircle.y + SECOND_HAND_OFFSET_Y);

    modelMatrix *= transform2D::Translate(secondHandCx, secondHandCy);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-secondHandCx, -secondHandCy);
    RenderMesh2D(meshes["hand"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mainPlayerCircle.x, mainPlayerCircle.y);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-cx, -cy);
    RenderMesh2D(meshes["mainCharacter"], shaders["VertexColor"], modelMatrix);
}

void m1::Tema1::renderEnviroment()
{
    // render health bar
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mainPlayerCircle.x + HEALTHBAR_OFFSET_X, mainPlayerCircle.y + HEALTHBAR_OFFSET_Y);
    modelMatrix *= transform2D::Scale(MAX_HEALTH, 2);
    RenderMesh2D(meshes["healthBarGrid"], shaders["VertexColor"], modelMatrix);

    if (currHealth >= 0) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(mainPlayerCircle.x + HEALTHBAR_OFFSET_X, mainPlayerCircle.y + HEALTHBAR_OFFSET_Y);
        modelMatrix *= transform2D::Scale(currHealth, 2);
        RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix);
    }

    // obstacles rendering
    for (Rectangle& rect : obstacles) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(rect.x, rect.y);
        modelMatrix *= transform2D::Scale(rect.scaleX, rect.scaleY);
        RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    }

    // map rendering
    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["map"], shaders["VertexColor"], modelMatrix);
}

void m1::Tema1::runDeleteLogic() {
    if (projectilesToDelete.size() > 0) {
        for (int id : projectilesToDelete) {
            projectiles.erase(id);
        }
        projectilesToDelete.clear();
    }

    if (enemiesToDelete.size() > 0) {
        for (int id : enemiesToDelete) {
            enemies.erase(id);
        }
        enemiesToDelete.clear();
    }
}

void m1::Tema1::runSpawnLogic(float deltaTimeSeconds) {
    if (spawnEnemyTimer <= 0) {
        glm::ivec2 resolution = window->GetResolution();

        int zone = rand() % NUM_ZONES;
        float speed = (float)(rand() % MAX_ENEMY_ADDITIONAL_SPEED + MIN_ENEMY_SPEED);
        speed *= BASE_ENEMY_SPEED * deltaTimeSeconds;

        int startX = 0;
        int startY = 0;
        if (zone == BOTTOM_LEFT_SPAWN_ZONE) {
            startX = rand() % MAX_SPAWN_OFFSET;
            startY = rand() % MAX_SPAWN_OFFSET;
        }
        else if (zone == TOP_RIGHT_SPAWN_ZONE) {
            startX = resolution.x - rand() % MAX_SPAWN_OFFSET - MAIN_CHARACTER_SIDE;
            startY = resolution.y - rand() % MAX_SPAWN_OFFSET - MAIN_CHARACTER_SIDE;
        }
        else if (zone == TOP_LEFT_SPAWN_ZONE) {
            startX = rand() % MAX_SPAWN_OFFSET;
            startY = resolution.y - rand() % MAX_SPAWN_OFFSET - MAIN_CHARACTER_SIDE;
        }
        else if (zone == BOTTOM_RIGHT_SPAWN_ZONE) {
            startX = resolution.x - rand() % MAX_SPAWN_OFFSET - MAIN_CHARACTER_SIDE;
            startY = rand() % MAX_SPAWN_OFFSET;
        }

        enemies[enemyId] = Enemy(Rectangle(startX, startY, MAIN_CHARACTER_SIDE, MAIN_CHARACTER_SIDE, 1, 1), enemyId, speed);

        spawnEnemyTimer = SPAWN_ENEMY_TIME;
        enemyId += 1;
    }
}

void m1::Tema1::resetGame()
{
    glm::ivec2 resolution = window->GetResolution();

    currHealth = MAX_HEALTH;
    mainPlayerCircle.x = (float)resolution.x / 4 - cx;
    mainPlayerCircle.y = (float)resolution.y / 4 - cy;
    cameraX = 0;
    cameraY = 0;
    score = 0;
    spawnEnemyTimer = SPAWN_ENEMY_TIME;
    gunCooldownSeconds = 0;

    enemies.clear();
    projectiles.clear();

    cout << "Game over!\n";
}

inline void m1::Tema1::updateTimers(float deltaTimeSeconds)
{
    gunCooldownSeconds -= deltaTimeSeconds;
    spawnEnemyTimer -= deltaTimeSeconds;
}

bool Tema1::checkBoundsAndObstacleCollisions(Circle& circle, float dx, float dy)
{
    Circle futureCircle(circle.x + dx, circle.y + dy, circle.radius, circle.scaleX, circle.scaleY);
    if (isOutOfBounds(futureCircle, window)) {
        return true;
    }

    for (Rectangle rect : obstacles) {
        if (checkBallToRectCollision(futureCircle, rect)) {
            return true;
        }
    }

    return false;
}

bool m1::isOutOfBounds(Circle& circle, WindowObject* window) {
    glm::ivec2 resolution = window->GetResolution();
    float resolutionX = resolution.x;
    float resolutionY = resolution.y;

    if (circle.x < 0 || circle.x + circle.radius * 2 > resolutionX || circle.y < 0 || circle.y + circle.radius * 2 > resolutionY) {
        return true;
    }
    return false;
}

bool m1::checkBallToRectCollision(Circle& circle, Rectangle& rect)
{
    glm::vec2 circle_center(circle.x + circle.radius, circle.y + circle.radius);
    glm::vec2 rect_half_dims(rect.width / 2.0f, rect.height / 2.0f);
    glm::vec2 rect_center(rect.x + rect_half_dims.x, rect.y + rect_half_dims.y);

    glm::vec2 difference = circle_center - rect_center;
    glm::vec2 clamped = glm::clamp(difference, -rect_half_dims, rect_half_dims);
    glm::vec2 closest = rect_center + clamped;
    
    return glm::length(closest - circle_center) < circle.radius;
}

double m1::calculateArctan(glm::vec2 targetPoint, glm::vec2 centerPoint)
{
    float dy = targetPoint.y - centerPoint.y;
    float dx = targetPoint.x - centerPoint.x;

    return atan2(dy, dx);
}

