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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    //m_program->init(vShaderStr, fShaderStr);
    m_program->init("..\\assets\\shaders\\main.glsl");

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


    static VertexBuffer vb(36, c_defaultVf);
    static IndexBuffer ib(36);
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

    m_test.setupVbo(false);
    m_debugDraw.m_element.setupVbo(true);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_defaultTexture.init("..\\assets\\sponza\\textures\\spnza_bricks_a_diff.png");
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_defaultTexture.getTexture());
    CheckGlError();

    //_shadow.init(1024, 1024);
    _shadow.init(512, 512);
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
    m_debugDraw.m_program.bind();
    m_debugDraw.m_program.setPMatrix(camera().getProjection());
    m_debugDraw.m_program.setVMatrix(camera().getView());
    m_debugDraw.m_program.setMMatrix(glm::mat4( 1.0f ));

    m_program->bind();
    m_program->setPMatrix(camera().getProjection());
    m_program->setVMatrix(camera().getView());
    m_program->setMMatrix(glm::mat4(1.0f));
    m_program->setLightPos(_lightPos);
    
    m_debugDraw.drawGrid();
}

void Renderer::endRender()
{
    glDisable(GL_DEPTH_TEST);
    glm::vec4 cube[] = { 
        glm::vec4(-1.f, -1.f, -1.f, 1.f),  glm::vec4(-1.f, -1.f, 1.f, 1.f),
        glm::vec4(-1.f, 1.f, -1.f, 1.f),  glm::vec4(-1.f, 1.f, 1.f, 1.f),
        glm::vec4(1.f, 1.f, -1.f, 1.f),  glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec4(1.f, -1.f, -1.f, 1.f),  glm::vec4(1.f, -1.f, 1.f, 1.f) };

    glm::mat4 inverse = glm::inverse(_shadow.getLightMatrix());
    for (int i = 0; i < 8; i++) {
        cube[i] = inverse * cube[i];
    }

    m_debugDraw.drawCube(glm::vec3(_shadow._pos));

    m_debugDraw.addLine(glm::vec3(cube[0]) / cube[0].w, glm::vec3(cube[1]) / cube[1].w, c_red, c_green);
    m_debugDraw.addLine(glm::vec3(cube[2]) / cube[2].w, glm::vec3(cube[3]) / cube[3].w, c_red, c_green);
    m_debugDraw.addLine(glm::vec3(cube[4]) / cube[4].w, glm::vec3(cube[5]) / cube[5].w, c_red, c_green);
    m_debugDraw.addLine(glm::vec3(cube[6]) / cube[6].w, glm::vec3(cube[7]) / cube[7].w, c_red, c_green);

    m_debugDraw.addLine(glm::vec3(cube[1]) / cube[1].w, glm::vec3(cube[3]) / cube[3].w, c_green, c_green);
    m_debugDraw.addLine(glm::vec3(cube[3]) / cube[3].w, glm::vec3(cube[5]) / cube[5].w, c_green, c_green);
    m_debugDraw.addLine(glm::vec3(cube[5]) / cube[5].w, glm::vec3(cube[7]) / cube[7].w, c_green, c_green);
    m_debugDraw.addLine(glm::vec3(cube[7]) / cube[7].w, glm::vec3(cube[1]) / cube[1].w, c_green, c_green);

    m_debugDraw.m_element.updateVbo();
    renderElement(m_debugDraw.m_program, m_debugDraw.m_element);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::renderElement_dep(const ShaderProgram& program, const RenderElement& element, const glm::mat4& transform)
{
    if (element._count == 0)
        return;

    program.bind();
    program.setMMatrix(transform);
    program.setLMatrix(_shadow.getLightMatrix());
    if (element.textures[0] == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, m_defaultTexture.getTexture());
    }

    element.render();
}

void Renderer::renderElement(const ShaderProgram& program, const RenderElement& element)
{
    if (element._count == 0)
        return;

    program.bind();
    program.setLMatrix(_shadow.getLightMatrix());
    if (element.textures[0] == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, m_defaultTexture.getTexture());
    }

    element.render();
}

void Renderer::renderCube(const glm::mat4& transform)
{
    //m_debugDraw.drawCube(glm::vec3(transform[3]));
    renderElement_dep(*m_program, m_test, transform);
}

void Renderer::setLightPos(glm::vec3 pos)
{
    _lightPos = pos;
}

Camera& Renderer::camera()
{
    return m_camera;
}
