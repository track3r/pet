#pragma once

#include "ecs3/ecs3pch.h"
#include <stdlib.h>
#include <stdio.h>


class ObjReader
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    struct face_t
    {
        uint32_t vertices[3];
        uint32_t texcoords[3];
        uint32_t normals[3];
    };
    std::vector<face_t> faces;
    struct group_t
    {
        char name[64] = { 0 };
        char material[64] = { 0 };
        uint32_t startFace = 0;
        uint32_t endFace = 0;
    };
    std::vector<group_t> groups;

    bool parse(const char* filename);
};

class ObjMtlreader
{
public:
    struct material_t
    {
        char name[32];
        char texture[128];
    };
    std::vector<material_t> materials;

    bool parse(const char* filename);
};