#pragma once

#include "Application.h"

class SponzaApplication : public Application
{
    void subInit() override;
    void subUpdate() override;
private:
    std::vector<ecs3::Id> _entities;
};