#include "pch.h"
#include "GlHeaders.h"
#include "ShaderProgram.h"
#include "RenderShadow.h"
#include "Application.h"

ShadowCubeRt::ShadowCubeRt()
{
    _target[0] = glm::vec3(1.0, 0.0, 0.0);
    _target[1] = glm::vec3(-1.0, 0.0, 0.0);
    _target[2] = glm::vec3(0.0, 1.0, 0.0);
    _target[3] = glm::vec3(0.0, -1.0, 0.0);
    _target[4] = glm::vec3(0.0, 0.0, 1.0);
    _target[5] = glm::vec3(0.0, 0.0, -1.0);

    _up[0] = glm::vec3(0.0, -1.0, 0.0);
    _up[1] = glm::vec3(0.0, -1.0, 0.0);
    _up[2] = glm::vec3(0.0, 0.0, 1.0);
    _up[3] = glm::vec3(0.0, 0.0, -1.0);
    _up[4] = glm::vec3(0.0, -1.0, 0.0);
    _up[5] = glm::vec3(0.0, -1.0, 0.0);
}

void ShadowCubeRt::init(int width, int height)
{
    glGenTextures(1, &_texture);
    CheckGlError();
    _width = width;
    _height = height;
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glGenFramebuffers(1, &_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float aspect = (float)width / (float)height;
    float near = 1.0f;
    float far = 25.0f;
    _projection = glm::perspective(glm::radians(90.0f), aspect, near, far);
}

glm::mat4 ShadowCubeRt::getMvp(int face, glm::vec3 pos)
{
    return _projection * glm::lookAt(pos, pos + _target[face], _up[face]);
}

void ShadowCubeRt::bindRt(int face)
{
    if (face == 0)
    {
        glViewport(0, 0, _width, _height);
        glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, _texture, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowCubeRt::unbindRt()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowCubeRt::bindTexture()
{

}

const char shadowVertex[] = R"glsl(
        attribute vec3 v_position;

        uniform mat4 v_mMatrix;
        uniform mat4 v_vMatrix;
        uniform mat4 v_pMatrix;

        void main()
        {
            gl_Position = v_pMatrix * v_vMatrix * v_mMatrix * vec4(v_position, 1.0);
        }
        )glsl";

const char shadowFragment[] = R"glsl(      
        void main()
        {
        }
        )glsl";

ShadowRt::ShadowRt()
    :_program(new ShaderProgram)
{

}
void ShadowRt::init(int width, int height)
{
    _width = width;
    _height = height;

    glGenTextures(1, &_texture);
    CheckGlError();
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float aspect = (float)width / (float)height;
    float near = 1.0f;
    float far = 100.0f;
    //_projection = glm::perspective(glm::radians(90.0f), aspect, near, far);
    float near_plane = 1.0f, far_plane = 200.f;
    _projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    _program->init(shadowVertex, shadowFragment);
}

void ShadowRt::bindRt()
{
    glViewport(0, 0, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glClear(GL_DEPTH_BUFFER_BIT);
    _program->bind();
    _program->setPMatrix(_projection);
    _program->setVMatrix(_transform);
    _program->setMMatrix(glm::mat4(1.0f));
}

void ShadowRt::unbindRt()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glm::vec2 winSize = Application::get()->getWindowSize();
    glViewport(0, 0, (int)winSize.x, (int)winSize.y);
}

void ShadowRt::setPos(glm::vec3 pos)
{
    _transform = glm::lookAt(pos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void ShadowRt::bindTexture()
{

}