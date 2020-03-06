#pragma once

#include <memory>

#include "Camera.h"
#include "RenderElement.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"
#include "Texture.h"

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
    
    Camera& camera();
private:
    Camera m_camera;
    DebugDraw m_debugDraw;
    RenderElement m_test;
    Texture m_texture;
    std::unique_ptr<ShaderProgram> m_program;
};
