#pragma once

#include <memory>

#include "Camera.h"
#include "RenderElement.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"
#include "Texture.h"
#include "RenderShadow.h"

class ShaderProgram;
class FrameBuffer;

class RenderContext
{
public:
    bool init();
    void bindProgram(const ShaderProgram& program);
    void bindBuffer(const GpuBuffer& buffer, GpuBuffer::Type bindAs = GpuBuffer::None);
    void bindUniform(const GpuBuffer& uniform, uint8_t unit);
    //void bindFrameBuffer();
    void bindTexture(const Texture& texture, uint8_t unit);

#ifdef OPENGL
    static RenderContext* oglContext;
    static GLenum glBufferType(GpuBuffer::Type type);
    void bindGlBuffer(GpuBuffer::Type type, GLuint buffer);
    void bindVao(GLuint vao);
    void setTu(uint8_t unit);
#endif

private:
    ShaderProgram const *  _program = nullptr;
    Texture const *  _texture[6] = {};
    GpuBuffer const* _uniforms[16] = {};
#ifdef OPENGL
    GLuint _boundBuffers[GpuBuffer::MaxType] = {};
    GLuint _vao = 0;
    GLuint _tu = 0;
#endif
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void beginRender();
    void endRender();

    void renderElement_dep(const ShaderProgram& program, const RenderElement& element, const glm::mat4& transform);
    void renderElement(const ShaderProgram& program, const RenderElement& element);
    void renderElement(const RenderElement& element);

    void renderCube(const glm::mat4& transform);

    void setLightPos(glm::vec3 pos);
    ShaderProgram* getDefaultProgram() { return m_program; }
    Camera& camera();
    DebugDraw& getDebugDraw() { return m_debugDraw; }
    DebugDraw& getDebugDrawOverlay() { return m_debugDrawOverlay; }
    RenderContext& getRenderContext() { return _context; }
private:
    Camera m_camera;
    DebugDraw m_debugDraw;
    DebugDraw m_debugDrawOverlay;
    RenderElement m_test;
    Texture m_defaultTexture;
    ShaderProgram* m_program;
    glm::vec3 _lightPos;
    RenderContext _context;
};
