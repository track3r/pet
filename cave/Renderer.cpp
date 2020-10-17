#include "pch.h"
#include "Renderer.h"

void CheckGlError()
{
    int err = glGetError();
    assert(err == GL_NO_ERROR);
}


Renderer::Renderer()
    :m_program(new ShaderProgram())
{
}


Renderer::~Renderer()
{
}

void Renderer::init()
{
    _context.init();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    //m_program->init(vShaderStr, fShaderStr);
    m_program->init("../assets/shaders/main.glsl");

    GLfloat vertices[][3] =
    {
        { -1.f, -1.f, 0.0f },
        { 1.f, -1.f, 1.0f },
        { 0.f, 1.f, 2.0f },

        { -1.f, 0.f, 3.0f },
        { 1.f, 0.f, 4.0f },
        { 0.f, 2.f, 5.0f },

        { -1.f, 2.f, -6.0f },
        { 1.f, 2.f, -7.0f },
        { 0.f, 4.f, -8.0f },

        { -1.f, 1.f, -9.0f },
        { 1.f, 1.f, -10.0f },
        { 0.f, 3.f, -11.0f }
    };

    static const GLfloat vertices_cube[][3] = {
        { -1.0f, -1.0f, -1.0f }, // triangle 1 : begin
        { -1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f }, // triangle 1 : end
        { 1.0f, 1.0f, -1.0f }, // triangle 2 : begin
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f }, // triangle 2 : end
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f }
    };//36


    static VertexData vb(36, c_defaultVf);
    static IndexData ib(36);
    m_test.m_vertices = &vb;
    m_test.m_indices = &ib;

    glm::vec3 base;
    glm::vec2 uv[] =
    {
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f),
    };

    for (int i = 0; i < 36; i++)
    {
        const auto& tri = vertices_cube[i];
        //const auto& tri = vertices[i];
        glm::vec3 pos(tri[0], tri[1], tri[2]);
        //printf("{%f,%f,%f}\n", pos.x, pos.y, pos.z);
        vb.value<glm::vec3, VertexAttributeIndex::Pos>(i) = pos;
        vb.value<glm::vec2, VertexAttributeIndex::Uv>(i) = uv[i % 4];
        ib.intPointer()[i] = i;

        //m_debugDraw.addVec(pos, glm::normalize(pos));
        //m_debugDraw.addVec(pos, pos);
    }

    m_test.setupVbo(&_context, false);
    m_debugDraw.m_element.setupVbo(&_context, true);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_defaultTexture.init("..\\assets\\sponza\\textures\\spnza_bricks_a_diff.png");
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_defaultTexture.getTexture());
    _context.bindTexture(m_defaultTexture, 0);
    CheckGlError();

}

void Renderer::beginRender()
{
    // Setup
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glm::mat4 mat;
    //renderCube(mat);

    //mat = glm::translate(mat, glm::vec3(0, 0, 2));
    //renderCube(mat);
    m_debugDraw.reset();
    _context.bindProgram(m_debugDraw.m_program);
    m_debugDraw.m_program.setPMatrix(camera().getProjection());
    m_debugDraw.m_program.setVMatrix(camera().getView());
    m_debugDraw.m_program.setMMatrix(glm::mat4( 1.0f ));
    m_debugDraw.drawGrid();

    _context.bindProgram(*m_program);
    m_program->setLightPos(_lightPos);    
    
}

void Renderer::endRender()
{
    glDisable(GL_DEPTH_TEST);
    

    m_debugDraw.m_element.updateVbo(&_context);
    renderElement(m_debugDraw.m_program, m_debugDraw.m_element);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::renderElement_dep(const ShaderProgram& program, const RenderElement& element, const glm::mat4& transform)
{
    if (element._count == 0)
        return;

    _context.bindProgram(program);
    program.setMMatrix(transform);
    if (element.textures[0] == nullptr)
    {
        _context.bindTexture(m_defaultTexture, 0);
    }

    element.render(&_context);
}

void Renderer::renderElement(const ShaderProgram& program, const RenderElement& element)
{
    if (element._count == 0)
        return;

    _context.bindProgram(program);
    if (element.textures[0] == nullptr)
    {
        _context.bindTexture(m_defaultTexture, 0);
    }

    element.render(&_context);
}


void Renderer::renderCube(const glm::mat4& transform)
{
    //m_debugDraw.drawCube(glm::vec3(transform[3]));
    renderElement_dep(*m_program, m_test, transform);
}

Camera& Renderer::camera()
{
    return m_camera;
}


bool RenderContext::init()
{
#ifdef OPENGL
    assert(oglContext == nullptr);
    oglContext = this;
#endif
    return true;
}

#ifdef OPENGL
RenderContext* RenderContext::oglContext = nullptr;

// Vertex, Index, Uniform, Indirect, MaxType
static GLenum toGlEnum[] = { GL_NONE, GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER, GL_DRAW_INDIRECT_BUFFER, GL_NONE };

GLenum RenderContext::glBufferType(GpuBuffer::Type type)
{
    return toGlEnum[type];
}

void RenderContext::bindGlBuffer(GpuBuffer::Type type, GLuint buffer)
{
    if (_boundBuffers[type] == buffer)
    {
        return;
    }
    LOG("[%i,%i]=%i", type, toGlEnum[type], buffer);
    glBindBuffer(toGlEnum[type], buffer);
    _boundBuffers[type] = buffer;
}
#endif

void RenderContext::bindProgram(const ShaderProgram& program)
{
    if (_program == &program)
    {
        return;
    }

    glUseProgram(program.getGlProgram());
    _program = &program;
}

void RenderContext::bindBuffer(const GpuBuffer& buffer, GpuBuffer::Type bindAs)
{
    if (bindAs == GpuBuffer::None)
    {
        bindAs = buffer.getType();
    }

    bindGlBuffer(bindAs, buffer.getGlObject());
}

void RenderContext::bindUniform(const GpuBuffer& uniform, uint8_t unit)
{
    if (_uniforms[unit] == &uniform)
    {
        return;
    }

    _uniforms[unit] = &uniform;
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, uniform.getGlObject());
}

void RenderContext::bindTexture(const Texture& texture, uint8_t unit)
{
    if (_texture[unit] == &texture)
    {
        return;
    }

    setTu(unit);
    _texture[unit] = &texture;
    glBindTexture(texture.getGlType(), texture.getGlObject());
}

void RenderContext::bindVao(GLuint vao)
{
    if (_vao == vao)
    {
        return;
    }

    glBindVertexArray(vao);
    _vao = vao;

    bindGlBuffer(GpuBuffer::Index, 0);
}

void RenderContext::setTu(uint8_t unit)
{
    if (_tu == unit)
    {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
}