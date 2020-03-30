#pragma once

#if 0
class RNSpline
{
public:
    void Init() { nodeCount = 0; }
    void AddNode(const glm::vec3& pos);
    void BuildSpline();
    glm::vec3 GetPosition(float time);

    struct splineData_t
    {
        glm::vec3 position;
        glm::vec3 velocity;
        float distance;
    };
    splineData_t node[100];
    float maxDistance;
    int nodeCount;

protected:
    glm::vec3 GetStartVelocity(int index);
    glm::vec3 GetEndVelocity(int index);
};

class SNSpline : public RNSpline
{
public:
    void BuildSpline() { RNSpline::BuildSpline(); Smooth(); Smooth(); Smooth(); }
    void Smooth();
};

class TNSpline : public SNSpline
{
public:
    void AddNode(const glm::vec3& pos, float timePeriod);
    void BuildSpline() { RNSpline::BuildSpline(); Smooth(); Smooth(); Smooth(); }
    void Smooth() { SNSpline::Smooth(); Constrain(); }
    void Constrain();
};
#endif