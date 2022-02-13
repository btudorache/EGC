#include "lab_m1/tema2/tema2.h"

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <stdlib.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    resetGame();

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color specified
    {
        Shader* shader = new Shader("ColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "ColorVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "ColorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a displacement shader program for dying enemies
    {
        Shader* shader = new Shader("DisplacementShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "DisplacementVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "DisplacementFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    glm::ivec2 resolution = window->GetResolution();

    healthBarViewportX = resolution.x / 2 + 250;
    healthBarViewportY = resolution.y / 2 + 200;

    timerBarViewportX = 100;
    timerBarViewportY = resolution.y / 2 + 200;
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    renderGame(deltaTimeSeconds);
    runDeleteLogic();
    updateTimers(deltaTimeSeconds);
    checkEndGame();
}


void Tema2::FrameEnd()
{

}


void Tema2::RenderMeshSimple(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int color_location = glGetUniformLocation(shader->program, "Color");
    glUniform3fv(color_location, 1, glm::value_ptr(color));

    int time_location = glGetUniformLocation(shader->program, "Time");
    glUniform1f(time_location, .0005f * Engine::GetElapsedTime() * 50);

    mesh->Render();
}

void m1::Tema2::resetGame()
{
    walls.clear();
    freeMazeSpaces.clear();
    takenFreeMazeSpaces.clear();

    maze::MazeGenerator mazeGenerator(MAZE_HEIGHT, MAZE_WIDTH);
    std::vector<std::vector<bool>> maze = mazeGenerator.generateMaze();

    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[0].size(); j++) {
            if (maze[i][j]) {
                walls.push_back(tema2::Box(i * WALL_SIDE, 1, j * WALL_SIDE, WALL_SIDE));
            }
            else {
                freeMazeSpaces.push_back(std::make_pair(i, j));
                takenFreeMazeSpaces.push_back(0);
            }
        }
    }

    int playerPosIndex = rand() % freeMazeSpaces.size();
    std::pair<int, int> playerPos = freeMazeSpaces[playerPosIndex];
    takenFreeMazeSpaces[playerPosIndex] = true;

    int initialPlayerX = playerPos.first * WALL_SIDE;
    int initialPlayerZ = playerPos.second * WALL_SIDE;

    camera = new cam::Camera(glm::vec3(initialPlayerX, 4, initialPlayerZ + 3.5f), glm::vec3(initialPlayerX, 2, initialPlayerZ), glm::vec3(0, 1, 0));

    hpCamera = new cam::Camera(glm::vec3(-10, 5, -10), glm::vec3(-10, 0, -10), glm::vec3(0, 0, 1));

    timerCamera = new cam::Camera(glm::vec3(-20, 5, -20), glm::vec3(-20, 0, -20), glm::vec3(0, 0, 1));

    playerBox = tema2::Box(initialPlayerX, 1, initialPlayerZ, 1, 2, 1);

    while (enemiesToGenerate > 0) {
        int enemyPosIndex = rand() % freeMazeSpaces.size();
        if (!takenFreeMazeSpaces[enemyPosIndex]) {
            takenFreeMazeSpaces[enemyPosIndex] = true;
            enemiesToGenerate -= 1;

            std::pair<int, int> enemyPos = freeMazeSpaces[enemyPosIndex];
            int enemyX = enemyPos.first * WALL_SIDE;
            int enemyZ = enemyPos.second * WALL_SIDE;
            enemies[enemyId] = tema2::Enemy(enemyId, tema2::Sphere(enemyX, 1, enemyZ, 1), TIME_TO_DIE);
            enemyId += 1;
        }
    }

    playerHp = MAX_HP;
    currTimer = MAX_TIMER;
    enemiesToGenerate = NUM_ENEMIES;
    gunCooldownSeconds = 0;
    projectileId = 0;
    enemyId = 0;
    playerAngularRotation = 0;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void m1::Tema2::renderGame(float deltaTimeSeconds)
{
    renderProjectiles(deltaTimeSeconds);
    renderEnemies(deltaTimeSeconds);
    renderPlayer();
    renderEnviroment();
    renderHUD();
}

void m1::Tema2::renderProjectiles(float deltaTimeSeconds)
{
    for (auto& it : projectiles) {
        tema2::Projectile proj = it.second;

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(proj.prevPlayerX, BULLET_SPAWN_HEIGHT, proj.prevPlayerZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(BULLET_SCALE, BULLET_SCALE, BULLET_SCALE));
        modelMatrix = glm::translate(modelMatrix, glm::normalize(proj.prevCameraForward) * proj.projectileDistance);

        RenderMeshSimple(meshes["sphere"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.2, 0.2, 0.2));

        tema2::Sphere projectileSphere = tema2::Sphere(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2], BULLET_RADIUS);

        if (proj.projectileDistance > MAX_PROJECTILE_DISTANCE || projectileEnviromentCollisions(projectileSphere)) {
            projectilesToDelete.push_back(it.first);
        }

        for (auto& enm : enemies) {
            if (sphereToSphereCollision(projectileSphere, enm.second.sphere) && !enm.second.isDying) {
                projectilesToDelete.push_back(it.first);
                enemies.at(enm.first).isDying = true;
            }
        }

        projectiles.at(proj.id).projectileDistance += BULLET_SPEED * deltaTimeSeconds;
    }
}

void m1::Tema2::renderEnemies(float deltaTimeSeconds)
{
    for (auto& it : enemies) {
        tema2::Enemy* enemy = &enemies.at(it.first);

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->sphere.worldX, enemy->sphere.worldY, enemy->sphere.worldZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE));

        if (enemy->isDying) {
            RenderMeshSimple(meshes["sphere"], shaders["DisplacementShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.6, 0, 0));
        }
        else {
            RenderMeshSimple(meshes["sphere"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.6, 0, 0));
        }

        tema2::Box newPlayerBox = tema2::Box(playerBox.worldX, playerBox.worldY, playerBox.worldZ, playerBox.sideX, playerBox.sideY, playerBox.sideZ);
        if (boxToSphereCollision(newPlayerBox, enemy->sphere)) {
            enemiesToDelete.push_back(it.first);
            playerHp -= 1;
            return;
        }

        if (enemy->isDying) {
            enemy->timeToDie -= deltaTimeSeconds;
        }

        if (enemy->isDying && enemy->timeToDie < 0) {
            enemiesToDelete.push_back(it.first);
        }

        if (!enemy->isDying ) {
            if (enemy->direction == 0) {
                enemy->sphere.worldZ += ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH / 2) {
                    enemy->direction = 1;
                    enemy->phaseCurrDist = 0;
                }
            }
            else if (enemy->direction == 1) {
                enemy->sphere.worldX += ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH / 2) {
                    enemy->direction = 2;
                    enemy->phaseCurrDist = 0;
                }
            }
            else if (enemy->direction == 2) {
                enemy->sphere.worldZ -= ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH) {
                    enemy->direction = 3;
                    enemy->phaseCurrDist = 0;
                }
            }
            else if (enemy->direction == 3) {
                enemy->sphere.worldX -= ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH) {
                    enemy->direction = 4;
                    enemy->phaseCurrDist = 0;
                }
            }
            else if (enemy->direction == 4) {
                enemy->sphere.worldZ += ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH) {
                    enemy->direction = 5;
                    enemy->phaseCurrDist = 0;
                }
            }
            else if (enemy->direction == 5) {
                enemy->sphere.worldX += ENEMY_STEP;
                enemy->phaseCurrDist += ENEMY_STEP;
                if (enemy->phaseCurrDist > ENEMY_STEP_LENGTH) {
                    enemy->direction = 2;
                    enemy->phaseCurrDist = 0;
                }
            }
        }
    }
}

void m1::Tema2::renderPlayer()
{   
    // main body
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 0.5));

    float tempX = modelMatrix[3][0];
    float tempZ = modelMatrix[3][2];
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.2, 0.6, 0.4));

    // legs
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.24, 0.5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 1, 0.4));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.8, 0.8, 1));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.24, 0.5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 1, 0.4));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.8, 0.8, 1));

    // hands
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5, 1.5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.9, 0.4));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.8, 0.8, 1));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5, 1.5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.9, 0.4));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.8, 0.8, 1));

    // head
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currPlayerX, 0, currPlayerZ));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2.2, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.4, 0.4));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(1, 0.6, 0.2));

    playerBox.worldX = tempX;
    playerBox.worldZ = tempZ;
}

void m1::Tema2::renderEnviroment()
{
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(PLANE_SCALE, 0, PLANE_SCALE));
    RenderMeshSimple(meshes["plane"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.66, 0.66, 0.66));

    for (tema2::Box wall : walls) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(wall.worldX, wall.worldY, wall.worldZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(WALL_SIDE, WALL_SIDE, WALL_SIDE));
        RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, camera->GetViewMatrix(), projectionMatrix, glm::vec3(0.8, 0.8, 0.6));
    }
}

void m1::Tema2::renderHUD()
{
    // RENDERING HP BAR
    glm::ivec2 resolution = window->GetResolution();
    renderViewport(healthBarViewportX, healthBarViewportY, healthBarViewportWidth, healthBarViewportHeight);

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(HEALTH_BAR_WORLD_X, 0, HEALTH_BAR_WORLD_Z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2 * MAX_HP, 0, GUI_BAR_SCALE));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, hpCamera->GetViewMatrix(), hpProjectionOrtho, glm::vec3(0, 0, 0));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(HEALTH_BAR_WORLD_X + MAX_HP, 1, HEALTH_BAR_WORLD_Z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4 * playerHp, 0, GUI_BAR_SCALE));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, hpCamera->GetViewMatrix(), hpProjectionOrtho, glm::vec3(0.6, 0, 0));

    // RENDERING HP BAR
    renderViewport(timerBarViewportX, timerBarViewportY, timerBarViewportWidth, timerBarViewportHeight);

    int timerBars = ((int)currTimer) / 5;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(TIMER_BAR_WORLD_X, 0, TIMER_BAR_WORLD_Z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2 * (MAX_TIMER / 5), 0, GUI_BAR_SCALE));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, timerCamera->GetViewMatrix(), timerProjectionOrtho, glm::vec3(0, 0, 0));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(TIMER_BAR_WORLD_X + MAX_TIMER_BARS_FLOAT, 1, TIMER_BAR_WORLD_Z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4 * timerBars, 0, GUI_BAR_SCALE));
    RenderMeshSimple(meshes["box"], shaders["ColorShader"], modelMatrix, timerCamera->GetViewMatrix(), timerProjectionOrtho, glm::vec3(1, 1, 1));
}

void m1::Tema2::runDeleteLogic()
{
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

inline void m1::Tema2::updateTimers(float deltaTimeSeconds)
{
    gunCooldownSeconds -= deltaTimeSeconds;
    currTimer -= deltaTimeSeconds;
}

void m1::Tema2::checkEndGame()
{
    if (playerHp == 0 || currTimer < 0) {
        cout << "Game over!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        resetGame();
    }

    if (playerBox.worldZ <= -2) {
        cout << "You Won!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        resetGame();
    }
}

void m1::Tema2::renderViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    currPlayerX = 0;
    currPlayerZ = 0;

    if (!isFirstPersonMode) {
        if (window->KeyHold(GLFW_KEY_W) && !playerWallCollisions(0, -CAMERA_SPEED * deltaTime)) {
            camera->MoveForward(CAMERA_SPEED * deltaTime);
            currPlayerZ += -CAMERA_SPEED * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_A) && !playerWallCollisions(-CAMERA_SPEED * deltaTime, 0)) {
            camera->TranslateRight(-CAMERA_SPEED * deltaTime);
            currPlayerX += -CAMERA_SPEED * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_S) && !playerWallCollisions(0, CAMERA_SPEED * deltaTime)) {
            camera->MoveForward(-CAMERA_SPEED * deltaTime);
            currPlayerZ += CAMERA_SPEED * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_D) && !playerWallCollisions(CAMERA_SPEED * deltaTime, 0)) {
            camera->TranslateRight(CAMERA_SPEED * deltaTime);
            currPlayerX += CAMERA_SPEED * deltaTime;
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (isFirstPersonMode) {
            camera->Set(camera->position, camera->position + prevForward, prevUp);
            camera->TranslateUpward(-SWITCH_PERSON_HEIGHT_DISTANCE);
            camera->MoveForward(-SWITCH_PERSON_FORWARD_DISTANCE);
        }
        else {
            camera->MoveForward(SWITCH_PERSON_FORWARD_DISTANCE);
            camera->TranslateUpward(SWITCH_PERSON_HEIGHT_DISTANCE);

            prevForward = camera->forward;
            prevUp = camera->up;
        }
        isFirstPersonMode = !isFirstPersonMode;
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0 && !isFirstPersonMode) {
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
            playerAngularRotation += -deltaX * sensivityOX;
        }
        else if (window->GetSpecialKeyState() == 0 && isFirstPersonMode) {
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && gunCooldownSeconds <= 0 && isFirstPersonMode) {
        projectiles[projectileId] = tema2::Projectile(projectileId, playerBox.worldX, playerBox.worldZ, camera->forward, 0);
        projectileId += 1;
        gunCooldownSeconds = MAX_GUN_COOLDOWN;
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

bool m1::Tema2::boxToBoxCollision(tema2::Box& box1, tema2::Box& box2)
{   
    return (box1.minX <= box2.maxX && box1.maxX >= box2.minX) &&
           (box1.minY <= box2.maxY && box1.maxY >= box2.minY) &&
           (box1.minZ <= box2.maxZ && box1.maxZ >= box2.minZ);
}

bool m1::Tema2::boxToSphereCollision(tema2::Box& box, tema2::Sphere& sphere)
{
    // get box closest point to sphere center by clamping
    double x = max(box.minX, min(sphere.worldX, box.maxX));
    double y = max(box.minY, min(sphere.worldY, box.maxY));
    double z = max(box.minZ, min(sphere.worldZ, box.maxZ));

    // this is the same as isPointInsideSphere
    double distance = sqrt((x - sphere.worldX) * (x - sphere.worldX) +
                           (y - sphere.worldY) * (y - sphere.worldY) +
                           (z - sphere.worldZ) * (z - sphere.worldZ));

    return distance < sphere.radius;
}

bool m1::Tema2::sphereToSphereCollision(tema2::Sphere& sphere1, tema2::Sphere& sphere2)
{
    double distance = sqrt((sphere1.worldX - sphere2.worldX) * (sphere1.worldX - sphere2.worldX) +
                           (sphere1.worldY - sphere2.worldY) * (sphere1.worldY - sphere2.worldY) +
                           (sphere1.worldZ - sphere2.worldZ) * (sphere1.worldZ - sphere2.worldZ));
    return distance < (sphere1.radius + sphere2.radius);
}

bool m1::Tema2::playerWallCollisions(float deltaX, float deltaZ)
{
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerBox.worldX, 0, playerBox.worldZ));
    modelMatrix = glm::rotate(modelMatrix, playerAngularRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(deltaX, 0, deltaZ));

    tema2::Box newPlayerBox = tema2::Box(modelMatrix[3][0], playerBox.worldY, modelMatrix[3][2], playerBox.sideX, playerBox.sideY, playerBox.sideZ);
    for (tema2::Box wall : walls) {
        if (boxToBoxCollision(newPlayerBox, wall)) {
            return true;
        }
    }
    return false;
}

bool m1::Tema2::projectileEnviromentCollisions(tema2::Sphere& projectileSphere)
{
    for (tema2::Box wall : walls) {
        if (boxToSphereCollision(wall, projectileSphere)) {
            return true;
        }
    }

    if (projectileSphere.worldY < 0) {
        return true;
    }

    return false;
}
