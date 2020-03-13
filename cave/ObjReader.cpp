#include "ObjReader.h"

bool ObjReader::parse(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        return false;
    }
    positions.reserve(200000);
    texcoords.reserve(200000);
    faces.reserve(200000);
    char line[513];
    while (fgets(line, 512, f) != NULL)
    {
        glm::vec3 pos;
        if (sscanf(line, "v %f %f %f", &pos.x, &pos.y, &pos.z) == 3)
        {
            positions.push_back(pos);
            continue;
        }
        glm::vec2 texcoord;
        if (sscanf(line, "vt %f %f", &texcoord.x, &texcoord.y) == 2)
        {
            texcoords.push_back(texcoord);
            continue;
        }

        face_t nextFace;
        if (sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i",
            &nextFace.vertices[0], &nextFace.texcoords[0], &nextFace.normals[0],
            &nextFace.vertices[1], &nextFace.texcoords[1], &nextFace.normals[1],
            &nextFace.vertices[2], &nextFace.texcoords[2], &nextFace.normals[2]) == 9)
        {
            faces.push_back(nextFace);
            continue;
        }

        if (line[0] == '#' || isspace(line[0]))
        {
            continue;
        }

        group_t nextGroup;
        if (sscanf(line, "g %63s", nextGroup.name) == 1)
        {
            nextGroup.startFace = (uint32_t)faces.size();
            if (!groups.empty())
            {
                groups.back().endFace = (uint32_t)faces.size();
            }
            groups.push_back(nextGroup);
            continue;
        }

        if (!groups.empty() && sscanf(line, "usemtl %63s", nextGroup.material) == 1)
        {
            if (strlen(groups.back().material) == 0)
            {
                strcpy(groups.back().material, nextGroup.material);
                continue;
            }

            strcpy(nextGroup.name, groups.back().name);
            nextGroup.startFace = (uint32_t)faces.size();
            groups.back().endFace = (uint32_t)faces.size();
            groups.push_back(nextGroup);
            continue;
        }

    }

    if (!groups.empty())
    {
        groups.back().endFace = (uint32_t)faces.size();
    }

    return true;
}
