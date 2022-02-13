#include "lab_m1/tema1/tema_object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <math.h>


Mesh* tema_object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* tema_object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float height,
    float width,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(height, 0, 0), color),
        VertexFormat(corner + glm::vec3(height, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* tema_object2D::CreateCircle(
    const std::string& name, 
    glm::vec3 leftBottomCorner, 
    float radius,
    int numSides,
    glm::vec3 color, 
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    corner += glm::vec3(radius, radius, 0);

    std::vector<VertexFormat> vertices;
    for (int i = 0; i < 360; i += 360 / numSides) {
        float degreeToRad = (float)i * (M_PI / 180);
        vertices.push_back(VertexFormat(corner + glm::vec3(cos(degreeToRad) * radius, sin(degreeToRad) * radius, 0), color));
    }

    Mesh* circle = new Mesh(name);
    std::vector<unsigned int> indices;

    for (int i = 0; i < numSides; i++) {
        indices.push_back(i);
    }

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        circle->SetDrawMode(GL_TRIANGLE_FAN);

        vertices.insert(vertices.begin(), VertexFormat(corner, color));
        vertices.push_back(VertexFormat(corner + glm::vec3(cos(0) * radius, sin(0) * radius, 0), color));

        indices.push_back(numSides);
        indices.push_back(numSides + 1);
    }

    circle->InitFromData(vertices, indices);
    return circle;

}
