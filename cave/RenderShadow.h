#pragma once

class ShadowCubeRt
{
public:
    ShadowCubeRt();
    void init(int width, int height);
    void bindRt(int face);
    glm::mat4 getMvp(int face, glm::vec3 pos);
    void unbindRt();
    void bindTexture();

    GLuint _texture = 0;
    GLuint _fb = 0;
    int _width = 0;
    int _height = 0;
    glm::mat4 _transforms[6];
    glm::mat4 _projection;

    glm::vec3 _target[6];
    glm::vec3 _up[6];
};

class ShaderProgram;

class ShadowRt
{
public:
    ShadowRt();
    void init(int width, int height);
    void bindRt();
    void unbindRt();
    void setPos(glm::vec3 pos);
    void bindTexture();

    GLuint _texture = 0;
    GLuint _fb = 0;
    int _width = 0;
    int _height = 0;

    glm::mat4 _transform;
    glm::mat4 _projection;

    ShaderProgram* _program;
};