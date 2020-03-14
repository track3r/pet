#include "pch.h"
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
        glm::vec2 texcoord;
        face_t nextFace;
        group_t nextGroup;
        uint32_t qvertices[4];
        uint32_t qtexcoords[4];
        uint32_t qnormals[4];
        switch (line[0])
        {
        case 'v':
            if (sscanf(line, "v %f %f %f", &pos.x, &pos.y, &pos.z) == 3)
            {
                positions.push_back(pos / 10.f);
                continue;
            }

            if (sscanf(line, "vt %f %f", &texcoord.x, &texcoord.y) == 2)
            {
                texcoords.push_back(texcoord);
                continue;
            }
            break;
        case 'f':
            if (sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
                &qvertices[0], &qtexcoords[0], &qnormals[0],
                &qvertices[1], &qtexcoords[1], &qnormals[1],
                &qvertices[2], &qtexcoords[2], &qnormals[2],
                &qvertices[3], &qtexcoords[3], &qnormals[3]) == 12)
            {
                nextFace.vertices[0] = qvertices[0];
                nextFace.vertices[1] = qvertices[1];
                nextFace.vertices[2] = qvertices[2];
                
                nextFace.normals[0] = qnormals[0];
                nextFace.normals[1] = qnormals[1];
                nextFace.normals[2] = qnormals[2];

                nextFace.texcoords[0] = qtexcoords[0];
                nextFace.texcoords[1] = qtexcoords[1];
                nextFace.texcoords[2] = qtexcoords[2];
                faces.push_back(nextFace);
                nextFace.vertices[0] = qvertices[2];
                nextFace.vertices[1] = qvertices[3];
                nextFace.vertices[2] = qvertices[0];

                nextFace.normals[0] = qnormals[2];
                nextFace.normals[1] = qnormals[3];
                nextFace.normals[2] = qnormals[0];

                nextFace.texcoords[0] = qtexcoords[2];
                nextFace.texcoords[1] = qtexcoords[3];
                nextFace.texcoords[2] = qtexcoords[0];
                faces.push_back(nextFace);
                continue;
            }
            else if (sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i",
                &nextFace.vertices[0], &nextFace.texcoords[0], &nextFace.normals[0],
                &nextFace.vertices[1], &nextFace.texcoords[1], &nextFace.normals[1],
                &nextFace.vertices[2], &nextFace.texcoords[2], &nextFace.normals[2]) == 9)
            {
                faces.push_back(nextFace);
            }
            break;
        }
        

        
       

        if (line[0] == '#' || isspace(line[0]))
        {
            continue;
        }
        
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
