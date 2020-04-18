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
    const char vShaderStr[] = R"glsl(
        attribute vec3 v_position;
        attribute vec2 v_uv;
        attribute vec3 v_normal;

        uniform mat4 v_mMatrix;
        uniform mat4 v_vMatrix;
        uniform mat4 v_pMatrix;

        uniform mat4 v_lMatrix;

        uniform vec3 v_lightPos;
        
        varying vec2 f_texcoord0;
        varying vec3 f_lighDir;
        varying vec4 f_posLightspace;
        varying vec3 f_toLight;
        varying vec3 f_toCamera;
        varying vec3 f_normal;
        varying vec3 f_debug;
        void main()
        {
            
            f_texcoord0 = v_uv;
            gl_Position = v_pMatrix * v_vMatrix * v_mMatrix * vec4(v_position, 1.0);
        
            vec3 posWorld = v_mMatrix * vec4(v_position, 1.0);
            f_posLightspace = v_lMatrix * vec4(posWorld, 1.0);
            mat4 eyeMatrix = v_vMatrix * v_mMatrix;
            
            //mat4 eyeMatrix = v_mMatrix;
            f_debug = vec4(eyeMatrix * vec4(v_position, 0.0) ).xyz;
            vec4 eyePos = eyeMatrix * vec4(v_position, 1.0);
            f_toCamera = -eyePos.xyz;

            f_normal = vec4(eyeMatrix * vec4(v_normal, 0.0)).xyz;

            vec3 testLightDirWorld = vec3(0, 1, 0);
            
            //vec3 testLightPosWorld = vec3(0, 20, 0);
            vec3 lightPosEye = vec4(eyeMatrix * vec4(v_lightPos, 1.0)).xyz;
            f_toLight = lightPosEye - eyePos;
            
            f_lighDir = vec4(eyeMatrix * vec4(testLightDirWorld, 0.0)).xyz;
            
        }
        )glsl";

    const char fShaderStr[] = R"glsl(
        //precision mediump float;
        varying vec3 pos;
        
        uniform mat4 v_mMatrix;
        uniform mat4 v_vMatrix;
        uniform mat4 v_pMatrix;

        uniform sampler2D texture0;
        //uniform sampler2DShadow textureShadow;
        uniform sampler2D textureShadow;

        varying vec2 f_texcoord0;
        varying vec3 f_lighDir;
        varying vec4 f_posLightspace;
        varying vec3 f_toLight;
        varying vec3 f_toCamera;
        varying vec3 f_normal;
        varying vec3 f_debug;

        void main()
        {
            gl_FragColor.a = 1.0;

            vec3 normal = normalize(f_normal);
            float ndl = dot(normal,normalize(f_toLight));
            float distance = length(f_toLight);
            float att = 1.0 + distance*0.0004  + distance*distance*0.0004;
            ndl /= att;            

            vec3 projCoords = f_posLightspace.xyz / f_posLightspace.w; // perform perspective divide
            projCoords = projCoords * 0.5 + 0.5;// transform to [0,1] range
            
            //gl_FragColor.xyz = vec3(projCoords.y);
            //return;
            
            
            float closestDepth = 1 - texture(textureShadow, projCoords.xy).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            float currentDepth = 1 - projCoords.z;// get depth of current fragment from light's perspective
            
            //float shadow = texture(textureShadow, projCoords) > 0 ? 1.0 : 0.1;
            //f_posLightspace.z += 0.05;
            //float shadow = shadow2DProj(textureShadow, f_posLightspace);
            
            //gl_FragColor.xyz = vec3(f_posLightspace.w);
            //return;
            

            float shadow = currentDepth > closestDepth + 0.001 ? 1.0 : 0.1;

            

            //gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl),1.0);
            //gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl),1.0) * shadow;
            //gl_FragColor =  vec4(vec3(calcShadow_debug(f_posLightspace, bias)), 1.0);
            
            //ndl = 0.5;
            gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl * shadow) + vec3(0.2, 0.2, 0.2) ,1.0) ;
            //gl_FragColor = vec4(vec3(shadow) + vec3(0.1, 0.1, 0.1), 1.0);

            //gl_FragColor.xy = f_texcoord0;
            //gl_FragColor.a = 1.0;

        }
        )glsl";

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    m_program->init(vShaderStr, fShaderStr);

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

    _shadow.init(1024, 1024);
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
    program.setTextureShadow(1);
    program.setMMatrix(transform);
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
    renderElement(*m_program, m_test, transform);
}

void Renderer::setLightPos(glm::vec3 pos)
{
    _lightPos = pos;
}

Camera& Renderer::camera()
{
    return m_camera;
}
