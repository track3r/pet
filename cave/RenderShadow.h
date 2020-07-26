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

class ShadowPass
{
public:
    ShadowPass();
    void init(int width, int height);
    void begin();
    void end();
    //void setPos(glm::vec3 pos);
    void bindTexture();
    GLuint _texture = 0;
    GLuint _fb = 0;
    int _width = 0;
    int _height = 0;

    //glm::mat4 _transform;
    //glm::mat4 _projection;
    //glm::mat4 _bias;

    ShaderProgram* _program;
};

class SimpleAtlas
{
public:
    SimpleAtlas()
    {

    }
    void init(glm::ivec2 size, glm::ivec2 elementSize)
    {
        _size = size;
        _elementSize = elementSize;
        _capacity = _size / _elementSize;
        assert(_capacity.x > 0 && _capacity.y > 0);
        _border = (_size - (_capacity - glm::ivec2(1,1) * _elementSize) ) / _capacity;
        //_map.resize(_capacity.x * _capacity.y, false);
    }

    glm::ivec2 getPos(uint16_t handle)
    {
        //assert(_map[hande]);
        int x = handle % _capacity.x;
        int y = handle / _capacity.y;
        glm::ivec2 logic(handle % _capacity.x, handle / _capacity.y);
        return glm::ivec2(logic * (_elementSize + _border));
    }

    uint16_t alloc()
    {
        if (_num == _capacity.x * _capacity.y - 1)
        {
            return -1;
        }

        return _num++;
    }

    void reset()
    {
        _num = 0;
    }

    glm::ivec2 _size = {};
    glm::ivec2 _elementSize = {};
    glm::ivec2 _border = {};
    glm::ivec2 _capacity = {};
    uint16_t   _num = 0;
    //std::vector<bool> _map;
};

struct shadowView_t
{
    glm::ivec2 pos;
    uint16_t handle;
};

class ShadowManager
{
public:
    ShadowManager()
    {

    }
    
    bool init(uint16_t width, uint16_t height, uint16_t atlasWidth, uint16_t atlasHeight)
    {
        _atlas.init(glm::vec2(atlasWidth, atlasHeight), glm::ivec2(width, height));
        _renderPass.init(width, height);
        
        const float aspect = (float)width / (float)height;
        const float near = 0.1f;
        const float far = 1000.0f;
        _projection = glm::perspective(glm::radians(90.0f), aspect, near, far);

        return true;
    }

    void beginShadowPasses()
    {
        _atlas.reset();
        _renderPass.begin();
    }

    void endShadowPasses()
    {
        _renderPass.end();
    }

    uint16_t beginShadowVew()
    {
        uint16_t handle = _atlas.alloc();
        if (handle == -1)
        {
            return -1;
        }

        glm::ivec2 pos = _atlas.getPos(handle);
        glViewport(pos.x, pos.y, _atlas._elementSize.x, _atlas._elementSize.y);
    }

    glm::vec4 getAtlasCoords(uint16_t handle)
    {
        glm::ivec2 pos = _atlas.getPos(handle);
        float x1 = pos.x / (float)_atlas._size.x;
        float y1 = pos.y / (float)_atlas._size.y;

        float x2 = pos.x + 
        return glm::vec4();
    }

    SimpleAtlas _atlas;
    ShadowPass _renderPass;
    glm::mat4 _projection;
};