#include "pch.h"
#include "Renderer.h"

void CheckGlError()
{
    int err = glGetError();
    assert(err == GL_NO_ERROR);
}


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::init()
{
    const char vShaderStr[] =
        "attribute vec4 v_position; \n"
        "attribute vec3 v_uv; \n"
        "uniform mat4 vp_matrix; \n"
        "varying vec3 pos;\n"
        "varying vec2 f_texcoord0;\n"
        "void main() \n"
        "{ \n"
        " pos = (vp_matrix * v_position).xyz;"
        " f_texcoord0 = v_uv;\n"
        " gl_Position = vp_matrix * v_position; \n"
        "} \n";

    const char fShaderStr[] =
        "//precision mediump float; \n"
        "varying vec3 pos;\n"
        "varying vec2 f_texcoord0;\n"
        "uniform sampler2D texture0;\n"
        "void main() \n"
        "{ \n"
        //" gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0); \n"
        
        " gl_FragColor = texture(texture0, f_texcoord0); \n"
        "} \n";

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    m_program = std::make_unique<ShaderProgram>(vShaderStr, fShaderStr);

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

        m_debugDraw.addVec(pos, glm::normalize(pos));
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
    m_debugDraw.drawGrid();
}

void Renderer::endRender()
{
    glDisable(GL_DEPTH_TEST);
    m_debugDraw.m_element.updateVbo();
    renderElement(m_debugDraw.m_program, m_debugDraw.m_element);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::renderElement(const ShaderProgram& program, const RenderElement& element, const glm::mat4& transform)
{
    if (element._count == 0)
        return;

    program.bind();
    program.setTexture(0);
    program.setVpMatrix(m_camera.getVpMatrix() * transform);
    if (element.textures[0] == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, m_defaultTexture.getTexture());
    }

    element.render();
}

void Renderer::renderCube(const glm::mat4& transform)
{
    m_debugDraw.drawCube(glm::vec3(transform[3]));
    renderElement(*m_program, m_test, transform);
}

Camera& Renderer::camera()
{
    return m_camera;
}
