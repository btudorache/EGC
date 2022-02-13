#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace tema_object2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    // Create rectangle
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);

    // Create circle
    Mesh* CreateCircle(const std::string& name, glm::vec3 leftBottomCorner, float radius, int numSides, glm::vec3 color, bool fill = false);
}
