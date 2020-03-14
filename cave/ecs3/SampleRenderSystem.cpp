#include "pch.h"
#include "../Application.h"
#include "SampleRenderSystem.h"

namespace ecs3
{
    SampleRenderSystem::SampleRenderSystem(World* world)
        :System(world)
    {

    }

    void SampleRenderSystem::onRegister()
    {
        addComponent<SampleComponent>();
        addComponent<TransformComponent>();
    }

    void SampleRenderSystem::onUpdate(BlockIterator& iterator)
    {
        //printf("Render system update, size: %i\n", (int)iterator.size());
        Id* ids = iterator.getEntities();
        SampleComponent* components = iterator.getComponents<SampleComponent>();
        TransformComponent* transfroms = iterator.getComponents<TransformComponent>();
        for (int i = 0; i < iterator.size(); i++)
        {
            Id id = ids[i];
            SampleComponent* comp = components + i;
            //printf("\tent: %u,\n\t\t test: %i, test2: %i\n", (unsigned int)id.index, comp->test, comp->test2);
            TransformComponent* trans = transfroms + i;
            /*printf("\t\tmatrix: {%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f}\n",
                trans->matrix[0][0], trans->matrix[0][1], trans->matrix[0][2], trans->matrix[0][3],
                trans->matrix[1][0], trans->matrix[1][1], trans->matrix[1][2], trans->matrix[1][3],
                trans->matrix[2][0], trans->matrix[2][1], trans->matrix[2][2], trans->matrix[2][3],
                trans->matrix[3][0], trans->matrix[3][1], trans->matrix[3][2], trans->matrix[3][3]);*/

            Application::getRenderer()->renderCube(trans->matrix);

            //trans->matrix = glm::translate(trans->matrix, glm::vec3(0.0, 0.01f, 0.0f));
            
        }
    }
}
