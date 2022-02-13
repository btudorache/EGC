#include "lab_m1/tema3/tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{   
    // Generate dance floor colors
    for (int i = 0; i < NUM_FLOOR_HEIGHT_TILES; i++) {
        for (int j = 0; j < NUM_FLOOR_WIDTH_TILES; j++) {
            floorColors[i][j] = glm::vec3((float)(rand() % 255) / 255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255);
            newFloorColors[i][j] = glm::vec3((float)(rand() % 255) / 255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255);
            floorIsTransition[i][j] = false;
            floorTransitionTime[i][j] = 1;
            floorWaitTime[i][j] = 1 + rand() % 3;
        }
    }

    // Generate cones
    for (int i = 0; i < NUM_CONES; i++) {
        coneColors[i] = glm::vec3((float)(rand() % 255) / 255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255);
        coneDirections[i] = glm::vec3(0, -1, 0);
    }
    conePositions[0] = glm::vec3(1.5, 3, 1.5);
    conePositions[1] = glm::vec3(5.5, 3, 1.5);
    conePositions[2] = glm::vec3(1.5, 3, 5.5);
    conePositions[3] = glm::vec3(5.5, 3, 5.5);

    // Generate player positions
    for (int i = 0; i < NUM_DANCERS; i++) {
        int randX = rand() % 4 + 2;
        int randZ = rand() % 4 + 2;
        dancerPositions[i] = glm::vec3(randX, 0.5, randZ);
        dancerBehavior[i] = rand() % 2;
        dancerDirection[i] = (dancerBehavior[i] == 1) ? 1 : -1;
        dancerDistances[i] = DANCER_MAX_DISTANCE;
        waitTime[i] = rand() % 4;
        isWaiting[i] = (rand() % 2 == 1) ? false : true;
    }

    {
        mapTextures["discoBall"] = CreateDiscoBallTexture();
    }

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

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Add cone mesh
    Mesh* cone = Tema3::CreateCone("cone", 1, 40);
    AddMeshToList(cone);

    // Creating dancer shader
    {
        Shader *shader = new Shader("DancerShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "DancerVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "DancerFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Creating floor shader
    {
        Shader* shader = new Shader("FloorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FloorVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FloorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Creating wall shader
    {
        Shader* shader = new Shader("WallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "WallVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "WallFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Creating disco ball and celling shader
    {
        Shader* shader = new Shader("DiscoBallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "DiscoBallVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "DiscoBallFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Creating simple color shader
    {
        Shader* shader = new Shader("ColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "ColorVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "ColorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Creating cone color shader
    {
        Shader* shader = new Shader("ConeShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "ConeVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "ConeFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }
}

Mesh* Tema3::CreateCone(
    const std::string& name,
    float radius,
    int numSides)
{
    glm::vec3 center = glm::vec3(0, 0, 0);


    std::vector<VertexFormat> vertices;
    for (int i = 0; i < 360; i += 360 / numSides) {
        float degreeToRad = (float)i * (M_PI / 180);
        vertices.push_back(VertexFormat(glm::vec3(cos(degreeToRad) * radius, -1, sin(degreeToRad) * radius), glm::vec3(1)));
    }

    Mesh* circle = new Mesh(name);
    std::vector<unsigned int> indices;

    for (int i = 0; i < numSides; i++) {
        indices.push_back(i);
    }

    circle->SetDrawMode(GL_TRIANGLE_FAN);

    vertices.insert(vertices.begin(), VertexFormat(center, glm::vec3(1)));
    vertices.push_back(VertexFormat(glm::vec3(cos(0) * radius, -1, sin(0) * radius), glm::vec3(1)));

    indices.push_back(numSides);
    indices.push_back(numSides + 1);

    circle->InitFromData(vertices, indices);
    return circle;
}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    {
        // Render out of bounds barriers
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 1.5, -2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 3));
        RenderSimpleColoredMesh(meshes["box"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 1.5, NUM_FLOOR_WIDTH_TILES + 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 3));
        RenderSimpleColoredMesh(meshes["box"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 3.5, NUM_FLOOR_WIDTH_TILES / 2 - 0.5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 14));
        RenderSimpleColoredMesh(meshes["box"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, -0.5, NUM_FLOOR_WIDTH_TILES / 2 - 0.5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 14));
        RenderSimpleColoredMesh(meshes["box"], modelMatrix);
    }

    {
        // Render celling and disco ball
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(NUM_FLOOR_HEIGHT_TILES / 2 + 0.05, 3.5, NUM_FLOOR_WIDTH_TILES / 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(9, 1, 9));
        RenderDiscoBallAndCelling(meshes["box"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, discoBallPosition);
        RenderDiscoBallAndCelling(meshes["sphere"], modelMatrix);
    }

    {   
        // Render floor
        for (int i = 0; i < NUM_FLOOR_HEIGHT_TILES; i++) {
            for (int j = 0; j < NUM_FLOOR_HEIGHT_TILES; j++) {
                glm::vec3 color;

                if (floorIsTransition[i][j]) {
                    color = mix(newFloorColors[i][j], floorColors[i][j], floorTransitionTime[i][j]);

                    floorTransitionTime[i][j] -= deltaTimeSeconds / 2;
                    if (floorTransitionTime[i][j] < 0) {
                        floorIsTransition[i][j] = false;
                        floorWaitTime[i][j] = 1 + rand() % 3;
                        floorColors[i][j] = newFloorColors[i][j];
                    }
                }
                else {
                    color = floorColors[i][j];

                    floorWaitTime[i][j] -= deltaTimeSeconds;
                    if (floorWaitTime[i][j] < 0) {
                        floorTransitionTime[i][j] = 1;
                        floorIsTransition[i][j] = true;
                        newFloorColors[i][j] = glm::vec3((float)(rand() % 255) / 255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255);
                    }
                }

                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0.01f, j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
                RenderSimpleMesh(meshes["quad"], shaders["FloorShader"], modelMatrix, glm::vec3(i, 0, j), color, color);
            }
        }
    }

    {   
        // Render walls
        glm::vec3 color;
        for (int i = 0; i < NUM_FLOOR_HEIGHT_TILES; i++) {
            color = (floorIsTransition[i][0]) ? glm::mix(newFloorColors[i][0], floorColors[i][0], floorTransitionTime[i][0]) : floorColors[i][0];
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 1.5, -1));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 1));
            RenderSimpleMesh(meshes["box"], shaders["WallShader"], modelMatrix, glm::vec3(i, 0.01f, 0), color);

            color = (floorIsTransition[i][7]) ? glm::mix(newFloorColors[i][7], floorColors[i][7], floorTransitionTime[i][7]) : floorColors[i][7];
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 1.5, NUM_FLOOR_WIDTH_TILES));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 1));
            RenderSimpleMesh(meshes["box"], shaders["WallShader"], modelMatrix, glm::vec3(i, 0.01f, NUM_FLOOR_WIDTH_TILES - 1), color);
        }

        for (int j = 0; j < NUM_FLOOR_WIDTH_TILES; j++) {
            color = (floorIsTransition[7][j]) ? glm::mix(newFloorColors[7][j], floorColors[7][j], floorTransitionTime[7][j]) : floorColors[7][j];
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(NUM_FLOOR_HEIGHT_TILES, 1.5, j));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 1));
            RenderSimpleMesh(meshes["box"], shaders["WallShader"], modelMatrix, glm::vec3(NUM_FLOOR_HEIGHT_TILES - 1, 0.01f, j), color);
        }
    }

    // Render Dancers
    for (int i = 0; i < NUM_DANCERS; i++) {
        glm::vec3 floor_light_positions[DANCER_TILES];
        glm::vec3 floor_light_colors[DANCER_TILES];
        int arrayPos = 0;

        int roundedDancerX = (int)round(dancerPositions[i].x);
        int roundedDancerZ = (int)round(dancerPositions[i].z);
        for (int i = -1; i <= 1; i++) {
            for (int z = -1; z <= 1; z++) {
                int lightPosX = roundedDancerX + i;
                int lightPosZ = roundedDancerZ + z;
                if (lightPosX >= 0 && lightPosX < NUM_FLOOR_HEIGHT_TILES && lightPosZ >= 0 && lightPosZ < NUM_FLOOR_WIDTH_TILES) {
                    floor_light_positions[arrayPos] = glm::vec3(lightPosX, -0.5f, lightPosZ);
                    floor_light_colors[arrayPos] = (floorIsTransition[lightPosX][lightPosZ]) ? 
                        mix(newFloorColors[lightPosX][lightPosZ], floorColors[lightPosX][lightPosZ], floorTransitionTime[lightPosX][lightPosZ]) : 
                        floorColors[lightPosX][lightPosZ];

                }
                else {
                    floor_light_positions[arrayPos] = glm::vec3(0, 0, 0);
                    floor_light_colors[arrayPos] = glm::vec3(0, 0, 0);
                }
                arrayPos += 1;
            }
        }

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dancerPositions[i]);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1, 0.5f));
        RenderDancer(modelMatrix, floor_light_positions, floor_light_colors);


        if (!isWaiting[i]) {
            float distanceToGo = DANCER_SPEED * deltaTimeSeconds * dancerDirection[i];
            dancerDistances[i] -= deltaTimeSeconds;

            if (dancerBehavior[i] == 0) {
                dancerPositions[i].x += distanceToGo;
            }
            else {
                dancerPositions[i].z += distanceToGo;
            }

            if (dancerDistances[i] < 0) {
                isWaiting[i] = true;
            }
        }
        else {
            waitTime[i] -= deltaTimeSeconds;
            if (waitTime[i] < 0) {
                isWaiting[i] = false;
                waitTime[i] = rand() % 4;
                dancerDistances[i] = DANCER_MAX_DISTANCE;
                dancerDirection[i] = (dancerDirection[i] == 1) ? -1 : 1;
            }
        }
    }
    
    if (reflectorLightsActivated) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glDepthMask(GL_FALSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = 0; i < NUM_CONES; i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, conePositions[i]);
            //TODO: rotation
            //modelMatrix = glm::rotate(modelMatrix, RADIANS(firstConeDir.x), glm::vec3(1, 0, 0));
            //modelMatrix = glm::rotate(modelMatrix, RADIANS(firstConeDir.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(RADIANS(spotLightAngle)), 1, tan(RADIANS(spotLightAngle))) * 3.0f);
            RenderSimpleColoredMesh(meshes["cone"], modelMatrix, coneColors[i], true);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }
}


void Tema3::FrameEnd()
{

}


Texture2D* m1::Tema3::CreateDiscoBallTexture()
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = BALL_RESOLUTION_X * BALL_RESOLUTION_Y * channels;
    unsigned char* data = new unsigned char[size];

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 255;
    }

    // TODO(student): Generate and bind the new texture ID
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);


    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, BALL_RESOLUTION_X, BALL_RESOLUTION_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps
    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, BALL_RESOLUTION_X, BALL_RESOLUTION_Y, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}

void m1::Tema3::SendModelViewProjection(Shader* shader, const glm::mat4& modelMatrix)
{
    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void m1::Tema3::SendCommonUniforms(Shader* shader, const glm::mat4& modelMatrix)
{   
    // disco ball related uniforms
    int enlapsed_location = glGetUniformLocation(shader->program, "enlapsed_time");
    glUniform1f(enlapsed_location, (float)(Engine::GetElapsedTime() / 10));

    int ball_position_loc = glGetUniformLocation(shader->program, "disco_ball_position");
    glUniform3f(ball_position_loc, discoBallPosition.x, discoBallPosition.y, discoBallPosition.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["discoBall"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "ballTexture"), 0);

    // send buttons lights uniforms
    int floor_lights_activated_location = glGetUniformLocation(shader->program, "floor_lights_activated");
    glUniform1i(floor_lights_activated_location, floorLightsActivated);

    int disco_light_activated_location = glGetUniformLocation(shader->program, "disco_light_activated");
    glUniform1i(disco_light_activated_location, discoLightActivated);

    int reflector_lights_activated_location = glGetUniformLocation(shader->program, "reflector_lights_activated");
    glUniform1i(reflector_lights_activated_location, reflectorLightsActivated);

    SendModelViewProjection(shader, modelMatrix);
}

void m1::Tema3::SendLightUniforms(Shader* shader, const glm::vec3& color)
{
    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    // Set cone lights uniforms
    int spot_light_angle = glGetUniformLocation(shader->program, "spot_light_angle");
    glUniform1f(spot_light_angle, spotLightAngle);

    int spot_light_position = glGetUniformLocation(shader->program, "spot_light_position");
    glUniform3fv(spot_light_position, NUM_CONES, glm::value_ptr(conePositions[0]));

    int spot_light_direction = glGetUniformLocation(shader->program, "spot_light_direction");
    glUniform3fv(spot_light_direction, NUM_CONES, glm::value_ptr(coneDirections[0]));

    int spot_light_color = glGetUniformLocation(shader->program, "spot_light_color");
    glUniform3fv(spot_light_color, NUM_CONES, glm::value_ptr(coneColors[0]));
}

void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& light_position, const glm::vec3& light_color, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int light_position_loc = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(light_position_loc, light_position.x, light_position.y, light_position.z);

    int light_color_loc = glGetUniformLocation(shader->program, "light_color");
    glUniform3f(light_color_loc, light_color.x, light_color.y, light_color.z);

    SendLightUniforms(shader, color);

    SendCommonUniforms(shader, modelMatrix);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void m1::Tema3::RenderDancer(const glm::mat4& modelMatrix, glm::vec3* floor_light_positions, glm::vec3* floor_light_colors)
{
    Mesh* mesh = meshes["box"];
    Shader* shader = shaders["DancerShader"];
    Texture2D* texture = mapTextures["discoBall"];

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int light_position_array_loc = glGetUniformLocation(shader->program, "pointLightPos");
    glUniform3fv(light_position_array_loc, DANCER_TILES, glm::value_ptr(floor_light_positions[0]));

    int light_color_array_loc = glGetUniformLocation(shader->program, "pointLightColor");
    glUniform3fv(light_color_array_loc, DANCER_TILES, glm::value_ptr(floor_light_colors[0]));

    SendLightUniforms(shader, glm::vec3(0.01f));

    SendCommonUniforms(shader, modelMatrix);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void m1::Tema3::RenderDiscoBallAndCelling(Mesh* mesh, const glm::mat4& modelMatrix)
{
    Shader* shader = shaders["DiscoBallShader"];

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    SendCommonUniforms(shaders["DiscoBallShader"], modelMatrix);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void m1::Tema3::RenderSimpleColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color, bool isCone)
{
    Shader* shader = (isCone) ? shaders["ConeShader"] : shaders["ColorShader"];
    glUseProgram(shader->program);

    int object_color = glGetUniformLocation(shader->program, "Color");
    glUniform3f(object_color, color.x, color.y, color.z);

    SendModelViewProjection(shader, modelMatrix);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema3::OnKeyPress(int key, int mods)
{
    // Add key press event

    // TODO(student): Set keys that you might need
    if (key == GLFW_KEY_J) {
        floorLightsActivated = 1 - floorLightsActivated;
    } else if (key == GLFW_KEY_K) {
        discoLightActivated = 1 - discoLightActivated;
    } else if (key == GLFW_KEY_L) {
        reflectorLightsActivated = 1 - reflectorLightsActivated;
    }
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
