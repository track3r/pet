#include "pch.h"
#include "sponza.h"
#include <SDL2/SDL.h>
#include "ecs3/SampleRenderSystem.h"
#include "ecs3/PlayerSystem.h"
#include "ecs3/MeshRenderSystem.h"
#include "ecs3/TransformAnimationSystem.h"
#include "render/RenderWorld.h"
#include "ObjReader.h"
#include "Job.h"
#include "Utils.h"

#define WIN32_EXTRALEAN
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void CreateTestComponents(ecs3::World* _world)
{
	ecs3::Configuration singleConf;
	singleConf.addComponent<ecs3::TransformComponent>();
	_world->createEntity(singleConf);

	/*ecs3::Configuration configuration;
	configuration.addComponent<ecs3::TransformComponent>();
	configuration.addComponent<ecs3::SampleComponent>();
	_world->createEntity(configuration);

	ecs3::EntitityPrefab prefab;
	prefab.addComponent(ecs3::TransformComponent(glm::vec3(1.0f, 1.0f, 1.0f)));
	prefab.addComponent(ecs3::SampleComponent());
	_world->createEntity(prefab);*/

	ecs3::EntitityPrefab playerConf;
	playerConf.addComponent(ecs3::TransformComponent(glm::vec3(10.f, 0.f, 0.f)));
	playerConf.addComponent(ecs3::SampleComponent());
	playerConf.addComponent(PlayerComponent());
	_world->createEntity(playerConf);
}



void CreateLight(ecs3::World* _world, ecs3::Id lightId, int startPoint = 0)
{
	ecs3::EntitityPrefab conf;
	conf.addComponent(ecs3::TransformComponent(glm::vec3(0.f, 20.f, 0.f)));
	conf.addComponent(LightComponent(lightId));

	AnimationDescription descr;
	const glm::vec3 points[] =
	{
		glm::vec3(-90, 60, 50),
		glm::vec3(80, 60, 50),
		glm::vec3(80, 60, -50),
		glm::vec3(-90, 60, -50),
		//glm::vec3(-90, 60, 50),
	};

	const int numPoints = sizeof(points) / sizeof(glm::vec3);
	for (int i = 0; i < numPoints + 1; i++)
	{
		descr.addPoint(points[(i + startPoint) % numPoints]);
	}
	descr._speed = 30;

	TransformAnimationComponent anim(descr);
	conf.addComponent(anim);
	_world->createEntity(conf);
}

void CreateLight2(ecs3::World* _world, ecs3::Id lightId)
{
	ecs3::EntitityPrefab conf;
	conf.addComponent(ecs3::TransformComponent(glm::vec3(-80, 20, 0)));
	conf.addComponent(LightComponent(lightId));
	_world->createEntity(conf);
}

void LoadTestScene(RenderContext* context, ecs3::World* _world, std::vector<ecs3::Id>& ids)
{
	RenderWorld* renderWorld = _world->get<RenderSingleton>().world;
	RenderGeometryManager& geometryManager = renderWorld->getGeometryManager();
	ObjMtlReader mtlReader;
	const char* sponzaMtl = "..\\assets\\sponza\\sponza.mtl";
	if (!mtlReader.parse(sponzaMtl))
	{
		LOG("Failed to read %s", sponzaMtl);
		return;
	}

	job_t job = { 0 };
	job.granularity = 1;
	job.numElements = (int)mtlReader.materials.size();
	std::vector<textureData_t> textureDataArray(job.numElements);

	auto func = [&](job_t& _job)
	{
		int index = InterlockedAdd(&_job.counter, _job.granularity) - _job.granularity;
		while (index < (int)_job.numElements)
		{
			int bound = min(index + _job.granularity, (int)job.numElements);
			int done = 0;
			for (int i = index; i < bound; i++)
			{
				if (mtlReader.materials[i].texture[0] == 0)
				{
					continue;
				}
				std::string filename = "..\\assets\\sponza\\";
				filename += mtlReader.materials[i].texture;

				loadTextureData(filename.c_str(), textureDataArray[i]);
				done++;
			}
			index = InterlockedAdd(&_job.counter, _job.granularity) - _job.granularity;
		}
	};

	std::unordered_map<std::string, Texture*> textures;
	for (int i = 0; i < mtlReader.materials.size(); i++)
	{
		if (mtlReader.materials[i].texture[0] == 0)
		{
			continue;
		}

		Texture* texture = new Texture();
		textures[mtlReader.materials[i].name] = texture;
	}

	LOG("Starting textures MT jobs...");
	JobRunner jobs;
	jobs.init(5);
	jobs.startJob(job, func);

	ObjReader reader;
	const char* sponza = "..\\assets\\sponza\\sponza.obj";
	const char* cube = "..\\assets\\cube.obj";
	if (!reader.parse(sponza))
	{
		LOG("Failed to read %s", sponza);
		return;
	}

	MeshComponent meshComp;
	ecs3::EntitityPrefab meshConf;

	meshConf.addComponent(ecs3::TransformComponent(glm::vec3(0.f)));
	meshConf.addComponent(meshComp);
	LOG("Uploading meshes...");
	uint32_t offset = 0;
	
	for (const ObjReader::group_t& group : reader.groups)
	{
		//meshConf.addComponent(ecs3::TransformComponent(glm::vec3(10.f * randfun(), 0.f * randfun(), 10.f * randfun())));
		//meshConf.addComponent(ecs3::TransformComponent(glm::vec3(0.0f, -20.f, 0.0f)));
#if 0
		static const char* meshFilter[] = { "sponza_117","sponza_366", "sponza_367", "sponza_12", "sponza_16", "sponza_370", "sponza_371" };
		bool skip = true;
		for (int i = 0; i < sizeof(meshFilter) / sizeof(meshFilter[0]); i++)
		{
			if (strcmp(group.name, meshFilter[i]) == 0)
			{
				skip = false;
				break;
			}
		}
		if (skip)
		{
			continue;
		}
#endif
		int faces = group.endFace - group.startFace;
		//LOG(">>Mesh %s", group.name);
		RenderElement alloc;
		if (!geometryManager.allocate(alloc, faces * 3, faces * 3))
		{
			LOG("geometry allocation failed");
			break;
		}

		uint32_t index = 0;
		for (uint32_t f = group.startFace; f < group.endFace; f++)
		{
			const ObjReader::face_t& face = reader.faces[f];
			for (int v = 0; v < 3; v++)
			{
				alloc.m_vertices->value<glm::vec3, VertexAttributeIndex::Pos>(index) = reader.positions[face.vertices[v] - 1];
				alloc.m_vertices->value<glm::vec2, VertexAttributeIndex::Uv>(index) = reader.texcoords[face.texcoords[v] - 1];
				alloc.m_vertices->value<glm::vec3, VertexAttributeIndex::Normal>(index) = reader.normals[face.normals[v] - 1];
				alloc.m_indices->intPointer()[index] = index + alloc._offset;
				index++;
			}
		}

		alloc.textures[0] = textures[group.material];
		//if (element.textures[0] != nullptr)
		//{
			//element._transparent = element.textures[0]->hasAlpha;
		//}
		if (alloc.textures[0] != nullptr)
		{
			alloc._transparent = alloc.textures[0]->hasAlpha;
		}
		offset += faces * 3;

		alloc.updateVbo(context);
		meshComp.mesh = _world->get<RenderSingleton>().world->createMesh(alloc);
		meshConf._data.addComponent(meshComp);
		ecs3::Id id = _world->createEntity(meshConf);
		ids.push_back(id);
	}
	LOG("Finalising texture jobs");
	jobs.assist(job, func);
	LOG("Uploading textures");
	for (int i = 0; i < mtlReader.materials.size(); i++)
	{
		if (mtlReader.materials[i].texture[0] == 0)
		{
			continue;
		}

		Texture* texture = textures[mtlReader.materials[i].name];
		texture->init(textureDataArray[i]);

		glObjectLabel(GL_TEXTURE, texture->m_texture, -1, mtlReader.materials[i].name);
	}

	for (int i = 0; i < textureDataArray.size(); i++)
	{
		freeTextureData(textureDataArray[i]);
	}
	TRACE();
}

void SponzaApplication::subUpdate()
{
	if (_world->get<ecs3::InputSingleton>().isReleased(SDLK_DELETE))
	{
		_world->deleteEntity(_entities[0]);
		_entities.erase(begin(_entities));
	}
}

void SponzaApplication::subInit()
{
	RenderWorld* renderWorld = _world->get<RenderSingleton>().world;

	CreateTestComponents(_world);

	CreateLight(_world, renderWorld->createLight(RenderLight()), 0);
	
	CreateLight(_world, renderWorld->createLight(RenderLight()), 1);
	CreateLight(_world, renderWorld->createLight(RenderLight()), 2);
	CreateLight(_world, renderWorld->createLight(RenderLight()), 3);
	
	CreateLight2(_world, renderWorld->createLight(RenderLight()));
	LoadTestScene(&_renderer.getRenderContext(), _world, _entities);
}
