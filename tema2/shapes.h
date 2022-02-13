#pragma once

#include "utils/glm_utils.h"


namespace tema2
{
    struct Sphere
    {
        Sphere() : worldX(0), worldY(0), worldZ(0), radius(0) {}
        Sphere(Sphere& sphere) {
            worldX = sphere.worldX;
            worldY = sphere.worldY;
            worldZ = sphere.worldZ;
            radius = sphere.radius;
        }
        Sphere(float worldX, float worldY, float worldZ, float radius)
            : worldX(worldX), worldY(worldY), worldZ(worldZ), radius(radius) {}
        float worldX;
        float worldY;
        float worldZ;
        float radius;
    };

    struct Box
    {
        Box() : worldX(0), worldY(0), worldZ(0), sideX(0), sideY(0), sideZ(0) {
            minX = 0;
            minY = 0;
            minZ = 0;
            maxX = 0;
            maxY = 0;
            maxZ = 0;
        }

        Box(float worldX, float worldY, float worldZ, float side)
            : worldX(worldX), worldY(worldY), worldZ(worldZ) {
            sideX = side;
            sideY = side;
            sideZ = side;

            minX = worldX - side / 2;
            minY = worldY - side / 2;
            minZ = worldZ - side / 2;

            maxX = worldX + side / 2;
            maxY = worldY + side / 2;
            maxZ = worldZ + side / 2;
        }

        Box(float worldX, float worldY, float worldZ, float sideX, float sideY, float sideZ)
            : worldX(worldX), worldY(worldY), worldZ(worldZ), sideX(sideX), sideY(sideY), sideZ(sideZ) {
            minX = worldX - sideX / 2;
            minY = worldY - sideY / 2;
            minZ = worldZ - sideZ / 2;

            maxX = worldX + sideX / 2;
            maxY = worldY + sideY / 2;
            maxZ = worldZ + sideZ / 2;
        }

        float worldX;
        float worldY;
        float worldZ;
        float sideX;
        float sideY;
        float sideZ;

        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;
    };

    struct Projectile 
    {
        Projectile() : id(-1), prevPlayerX(0), prevPlayerZ(0), prevCameraForward(0), projectileDistance(0) {}
        Projectile(int id, float prevPlayerX, float prevPlayerZ, glm::vec3 prevCameraForward, float projectileDistance)
            : id(id), 
              prevPlayerX(prevPlayerX), 
              prevPlayerZ(prevPlayerZ), 
              prevCameraForward(prevCameraForward),
              projectileDistance(projectileDistance) {}
        int id;
        float prevPlayerX;
        float prevPlayerZ;
        glm::vec3 prevCameraForward;
        float projectileDistance;
    };

    struct Enemy {
        Enemy() : id(-1), direction(-1), phaseCurrDist(0), sphere(Sphere()), isDying(false), timeToDie(1) {}
        Enemy(int id, Sphere sphere, float timeToDie) 
            : sphere(sphere),          
              timeToDie(timeToDie) {
              direction = 1;
              phaseCurrDist = 0;
              isDying = false;
        }

        int id;
        int direction;
        float phaseCurrDist;
        Sphere sphere;
        bool isDying;
        float timeToDie;
    };
} 
