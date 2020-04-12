#pragma once

#include <memory>

#include "Camera.h"
#include "RenderElement.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"
#include "Texture.h"
#include "RenderShadow.h"

class ShaderProgram;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void beginRender();
    void endRender();

    void renderElement(const ShaderProgram& program, const RenderElement& element, const glm::mat4& transform = glm::mat4());

    void renderCube(const glm::mat4& transform);

    void setLightPos(glm::vec3 pos);
    ShaderProgram* getDefaultProgram() { return m_program; }
    Camera& camera();

    ShadowRt _shadow;
private:
    Camera m_camera;
    DebugDraw m_debugDraw;
    RenderElement m_test;
    Texture m_defaultTexture;
    ShaderProgram* m_program;
    glm::vec3 _lightPos;
};
