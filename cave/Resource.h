#pragma once

#define BUILD_TOOLS

class Resource
{
public:
    virtual ~Resource();
#if defined(BUILD_TOOLS)
    struct dep_t
    {
        enum Type
        {
            File, None, Max
        };

        char type;
        char filename[256];
        time_t lastModified;
        time_t pendingModified;
    };

    std::vector<dep_t> _deps;

    bool addFileDep(const char* filename);
    void clearDeps(); 
    virtual void refresh() {}
#endif

    virtual bool init(const char* filename);
public:

    std::string _name;
};

class ResourceManager
{
public:
    void checkForUpdates();
    void addResource(Resource* resource);
    void removeResource(Resource* resource);

#if defined(BUILD_TOOLS)
    bool checkDepStall(Resource::dep_t& dep);
#endif
    std::unordered_map<std::string, Resource*> _resources;
};

extern ResourceManager* resourceManager;