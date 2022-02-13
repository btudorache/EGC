#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"


namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
     public:
        Tema3();
        ~Tema3();

        void Init() override;

        static const int NUM_FLOOR_WIDTH_TILES = 8;
        static const int NUM_FLOOR_HEIGHT_TILES = 8;
        static const int DANCER_TILES = 9;
        static const int BALL_RESOLUTION_X = 16;
        static const int BALL_RESOLUTION_Y = 16;
        static const int NUM_CONES = 4;
        static const int NUM_DANCERS = 6;
        static constexpr float DANCER_SPEED = 2;
        static constexpr float DANCER_MAX_DISTANCE = 1;

     private:
         Mesh* Tema3::CreateCone(
             const std::string& name,
             float radius,
             int numSides);
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        Texture2D* CreateDiscoBallTexture();
        void SendModelViewProjection(Shader* shader, const glm::mat4& modelMatrix);
        void SendCommonUniforms(Shader* shader, const glm::mat4& modelMatrix);
        void SendLightUniforms(Shader* shader, const glm::vec3& color);
        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3& light_position = glm::vec3(1), const glm::vec3& light_color = glm::vec3(1), const glm::vec3& color = glm::vec3(0.01, 0.01, 0.01));
        void RenderDancer(const glm::mat4& modelMatrix, glm::vec3* floor_light_positions, glm::vec3* floor_light_colors);
        void RenderDiscoBallAndCelling(Mesh* mesh, const glm::mat4& modelMatrix);
        void RenderSimpleColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(0), const bool isCone = false);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        std::unordered_map<std::string, Texture2D*> mapTextures;

        glm::vec3 discoBallPosition = glm::vec3(NUM_FLOOR_HEIGHT_TILES / 2 - 0.5, 2.5, NUM_FLOOR_WIDTH_TILES / 2 - 0.5);
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        float spotLightAngle = 20;

        int discoLightActivated = 1;
        int floorLightsActivated = 1;
        int reflectorLightsActivated = 1;

        glm::vec3 floorColors[NUM_FLOOR_HEIGHT_TILES][NUM_FLOOR_WIDTH_TILES];
        glm::vec3 newFloorColors[NUM_FLOOR_HEIGHT_TILES][NUM_FLOOR_WIDTH_TILES];
        float floorTransitionTime[NUM_FLOOR_HEIGHT_TILES][NUM_FLOOR_WIDTH_TILES];
        float floorWaitTime[NUM_FLOOR_HEIGHT_TILES][NUM_FLOOR_WIDTH_TILES];
        bool floorIsTransition[NUM_FLOOR_HEIGHT_TILES][NUM_FLOOR_WIDTH_TILES];

        glm::vec3 dancerPositions[NUM_DANCERS];
        int dancerDirection[NUM_DANCERS];
        int dancerBehavior[NUM_DANCERS];
        bool isWaiting[NUM_DANCERS];
        float dancerDistances[NUM_DANCERS];
        float waitTime[NUM_DANCERS];

        glm::vec3 coneColors[NUM_CONES];
        glm::vec3 conePositions[NUM_CONES];
        glm::vec3 coneDirections[NUM_CONES];
    };
}   // namespace m1
