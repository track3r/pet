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

    _up[0] = glm::vec3(0.0, 1.0, 0.0);
    _up[1] = glm::vec3(0.0, 1.0, 0.0);
    _up[2] = glm::vec3(0.0, 0.0, 1.0);
    _up[3] = glm::vec3(0.0, 0.0, -1.0);
    _up[4] = glm::vec3(0.0, 1.0, 0.0);
    _up[5] = glm::vec3(0.0, 1.0, 0.0);
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
    _projection = glm::perspective(glm::radians(95.001f), aspect, near, far);

    const float bias[16] = {
         };

   
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


ShadowPass::ShadowPass()
    :_program(new ShaderProgram())
{

}
void ShadowPass::init(int width, int height)
{
    _width = width;
    _height = height;

    glGenTextures(1, &_texture);
    CheckGlError();
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

#if 1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*float aspect = (float)_width / (float)_height;
    float near = 0.1f;
    float far = 1000.0f;
    _projection = glm::perspective(glm::radians(90.0f), aspect, near, far);
    //_projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near, far);
    _bias = glm::mat4(0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);*/

    _program->init("../assets/shaders/shadow_buffer.glsl");
}

void ShadowPass::begin()
{
    glViewport(0, 0, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glClear(GL_DEPTH_BUFFER_BIT);
    //_program->setPMatrix(_projection);
    //_program->setVMatrix(_transform);
    //_program->setMMatrix(glm::mat4(1.0f));
    glCullFace(GL_FRONT);
    //glCullFace(GL_BACK);
    //glDisable(GL_CULL_FACE);
}

void ShadowPass::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glm::vec2 winSize = Application::get()->getWindowSize();
    glViewport(0, 0, (int)winSize.x, (int)winSize.y);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

/*void ShadowPass::setPos(glm::vec3 pos)
{
    _pos = pos;
    _transform = glm::lookAt(pos,
        pos + glm::vec3(1.f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}*/

void ShadowPass::bindTexture()
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glActiveTexture(GL_TEXTURE0);
}

/*glm::mat4 ShadowRt::getLightMatrix()
{
    return  _projection * _transform;
}*/
