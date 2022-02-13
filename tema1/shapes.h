#pragma once

#include "utils/glm_utils.h"


namespace m1
{
    struct Rectangle
    {
        Rectangle() : x(0), y(0), width(1), height(1), scaleX(1), scaleY(1) {}
        Rectangle(float x, float y, float width, float height, float scaleX, float scaleY)
            : x(x), y(y), width(width), height(height), scaleX(scaleX), scaleY(scaleY) {}
        float x;
        float y;
        float width;
        float height;
        float scaleX;
        float scaleY;
    };

    struct Enemy
    {
        Enemy() : rect(Rectangle()), id(-1), speed(0) {}
        Enemy(Rectangle rect, int id, float speed) : rect(rect), id(id), speed(speed) {}
        Rectangle rect;
        int id;
        float speed;
    };

    struct Circle
    {
        Circle() : x(0), y(0), radius(0), scaleX(0), scaleY(0) {}
        Circle(float x, float y, float radius, float scaleX, float scaleY)
            : x(x), y(y), radius(radius), scaleX(scaleX), scaleY(scaleY) {}
        float x;
        float y;
        float radius;
        float scaleX;
        float scaleY;
    };

    struct Projectile 
    {
        Projectile() : id(-1), prevPlayerX(0), prevPlayerY(0), projectileCx(0), projectileCy(0), prevAngularStep(0), projectileDistance(0) {}
        Projectile(int id, float prevPlayerX, float prevPlayerY, float projectileCx, float projectileCy, float prevAngularStep, float projectileDistance)
            : id(id), 
              prevPlayerX(prevPlayerX), 
              prevPlayerY(prevPlayerY), 
              projectileCx(projectileCx), 
              projectileCy(projectileCy), 
              prevAngularStep(prevAngularStep), 
              projectileDistance(projectileDistance) {}
        int id;
        float prevPlayerX;
        float prevPlayerY;
        float projectileCx;
        float projectileCy;
        float prevAngularStep;
        float projectileDistance;
    };
} 
