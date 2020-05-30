#include "pch.h"
#include "Resource.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

ResourceManager* resourceManager = new ResourceManager();

#if defined(BUILD_TOOLS)

bool Resource::addFileDep(const char* filename)
{
    struct stat info;
    if (stat(filename, &info) != 0)
    {
        LOG("file not found");
        return false;
    }

    Resource::dep_t dep = { 0 };
    dep.type = Resource::dep_t::File;
    strcpy(dep.filename, filename);
    dep.lastModified = info.st_mtime;
    _deps.push_back(dep);

    return true;
}

void Resource::clearDeps()
{
    _deps.clear();
}
#endif//BUILD_TOOLS

Resource::~Resource()
{
    resourceManager->removeResource(this);
}

bool Resource::init(const char* filename)
{
    _name = filename;
    resourceManager->addResource(this);

    return true;
}

void ResourceManager::addResource(Resource* resource)
{
    assert(_resources.find(resource->_name) == _resources.end());
    _resources[resource->_name] = resource;
}

void ResourceManager::removeResource(Resource* resource)
{
    _resources.erase(resource->_name);
}

#if defined(BUILD_TOOLS)
bool ResourceManager::checkDepStall(Resource::dep_t& dep)
{
    switch (dep.type) {
    case Resource::dep_t::File:
        struct stat info;
        if (stat(dep.filename, &info) != 0)
        {
            dep.type = Resource::dep_t::None;
            LOG("file not found");
            return false;
        }

        if (info.st_mtime > dep.lastModified)
        {
            if (dep.pendingModified < info.st_mtime)
            {
                dep.pendingModified = info.st_mtime;
            }
            else
            {
                dep.pendingModified = 0;
                return true;
            }
            
        }

        return false;
    default:
        return false;
    }
}
#endif//BUILD_TOOLS


void ResourceManager::checkForUpdates()
{
#if defined(BUILD_TOOLS)
    for (const auto& p : _resources)
    {
        for (Resource::dep_t& dep : p.second->_deps)
        {
            if (checkDepStall(dep))
            {
                if (strcmp(p.first.c_str(), dep.filename) == 0)
                {
                    LOG("%s stalled, refreshing", p.first.c_str());
                }
                else
                {
                    LOG("%s(%s) stalled, refreshing", p.first.c_str(), dep.filename);
                }
                p.second->refresh();
                break;
            }
        }
    }
#endif//BUILD_TOOLS
}