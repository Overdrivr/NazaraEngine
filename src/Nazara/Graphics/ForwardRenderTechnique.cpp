// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	class LightManager
	{
		public:
			LightManager() = default;
			~LightManager() = default;

			unsigned int FindClosestLights(const NzLight** lights, unsigned int lightCount, const NzSpheref& object)
			{
				for (Light& light : m_lights)
				{
					light.light = nullptr;
					light.score = std::numeric_limits<unsigned int>::max(); // Nous jouons au Golf
				}

				for (unsigned int i = 0; i < lightCount; ++i)
				{
					const NzLight* light = *lights;

					unsigned int score = std::numeric_limits<unsigned int>::max();
					switch (light->GetLightType())
					{
						case nzLightType_Directional:
							score = 0; // Lumière choisie d'office
							break;

						case nzLightType_Point:
						{
							NzSpheref lightSphere(light->GetPosition(), light->GetRadius());

							if (lightSphere.Intersect(object))
								score = static_cast<unsigned int>(light->GetPosition().SquaredDistance(object.GetPosition())*1000.f);

							break;
						}

						case nzLightType_Spot:
						{
							NzSpheref lightSphere(light->GetPosition(), light->GetRadius());

							///TODO: Attribuer bonus/malus selon l'angle du spot ?
							if (lightSphere.Intersect(object))
								score = static_cast<unsigned int>(light->GetPosition().SquaredDistance(object.GetPosition())*1000.f);

							break;
						}
					}

					if (score < m_lights[0].score)
					{
						unsigned int j;
						for (j = 1; j < 3; ++j) ///TODO: Constante
						{
							if (score > m_lights[j].score)
								break;
						}

						j--; // Position de la nouvelle lumière

						// Décalage
						std::memcpy(&m_lights[0], &m_lights[1], j*sizeof(Light));

						m_lights[j].light = light;
						m_lights[j].score = score;
					}

					lights++;
				}

				unsigned int i;
				for (i = 0; i < 3; ++i) ///TODO: Constante
				{
					if (m_lights[i].light)
						break;
				}

				return 3-i; ///TODO: Constante
			}

			const NzLight* GetLight(unsigned int i) const
			{
				///TODO: Constante
				return m_lights[3-i-1].light; // Les lumières sont stockées dans l'ordre inverse (De la plus éloignée à la plus proche)
			}

		private:
			struct Light
			{
				const NzLight* light;
				unsigned int score;
			};

			Light m_lights[3]; ///TODO: Constante
	};
}

NzForwardRenderTechnique::NzForwardRenderTechnique() :
m_maxLightsPerObject(3) // Valeur totalement arbitraire
{
}

void NzForwardRenderTechnique::Clear(const NzScene* scene)
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererClear_Depth);

	NzAbstractBackground* background = scene->GetBackground();
	if (background)
		background->Draw(scene);
}

void NzForwardRenderTechnique::Draw(const NzScene* scene)
{
	///TODO: Regrouper les activations par méthode
	LightManager lightManager;

	const NzCamera* camera = scene->GetActiveCamera();
	const NzShader* lastShader = nullptr;

	// Rendu des modèles opaques
	for (auto& matIt : m_renderQueue.opaqueModels)
	{
		NzForwardRenderQueue::SkeletalMeshContainer& skeletalContainer = matIt.second.first;
		NzForwardRenderQueue::StaticMeshContainer& staticContainer = matIt.second.second;

		if (!skeletalContainer.empty() || !staticContainer.empty())
		{
			const NzMaterial* material = matIt.first;

			// On commence par récupérer le shader du matériau
			const NzShader* shader = material->GetShader(nzShaderTarget_Model, 0);

			unsigned int lightCount = 0;

			// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
			if (shader != lastShader)
			{
				NzRenderer::SetShader(shader);

				// Couleur ambiante de la scène
				shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
				// Position de la caméra
				shader->SendVector(shader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

				// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
				lightCount = m_renderQueue.directionnalLights.size();
				for (unsigned int i = 0; i < lightCount; ++i)
					m_renderQueue.directionnalLights[i]->Enable(shader, i);

				lastShader = shader;
			}

			material->Apply(shader);

			// Meshs squelettiques
			/*if (!skeletalContainer.empty())
			{
				NzRenderer::SetVertexBuffer(m_skinningBuffer); // Vertex buffer commun
				for (auto& subMeshIt : container)
				{
					///TODO
				}
			}*/

			// Meshs statiques
			for (auto& subMeshIt : staticContainer)
			{
				const NzStaticMesh* mesh = subMeshIt.first;
				std::vector<NzForwardRenderQueue::StaticData>& staticData = subMeshIt.second;

				if (!staticData.empty())
				{
					const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
					const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

					// Gestion du draw call avant la boucle de rendu
					std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> drawFunc;
					unsigned int indexCount;

					if (indexBuffer)
					{
						drawFunc = NzRenderer::DrawIndexedPrimitives;
						indexCount = indexBuffer->GetIndexCount();
					}
					else
					{
						drawFunc = NzRenderer::DrawPrimitives;
						indexCount = vertexBuffer->GetVertexCount();
					}

					NzRenderer::SetIndexBuffer(indexBuffer);
					NzRenderer::SetVertexBuffer(vertexBuffer);

					for (const NzForwardRenderQueue::StaticData& data : staticData)
					{
						// Calcul des lumières les plus proches
						if (lightCount < m_maxLightsPerObject && !m_renderQueue.lights.empty())
						{
							unsigned int count = lightManager.FindClosestLights(&m_renderQueue.lights[0], m_renderQueue.lights.size(), data.aabb.GetBoundingSphere());
							count -= lightCount;

							for (unsigned int i = 0; i < count; ++i)
								lightManager.GetLight(i)->Enable(shader, lightCount++);
						}

						for (unsigned int i = lightCount; i < 3; ++i) ///TODO: Constante sur le nombre maximum de lumières
							NzLight::Disable(shader, i);

						NzRenderer::SetMatrix(nzMatrixType_World, data.transformMatrix);
						drawFunc(mesh->GetPrimitiveMode(), 0, indexCount);
					}
					staticData.clear();
				}
			}
		}
	}

	for (const std::pair<unsigned int, bool>& pair : m_renderQueue.transparentsModels)
	{
		// Matériau
		NzMaterial* material = (pair.second) ?
		                       m_renderQueue.transparentStaticModels[pair.first].material :
		                       m_renderQueue.transparentSkeletalModels[pair.first].material;

		// On commence par récupérer le shader du matériau
		const NzShader* shader = material->GetShader(nzShaderTarget_Model, 0);

		unsigned int lightCount = 0;

		// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
		if (shader != lastShader)
		{
			NzRenderer::SetShader(shader);

			// Couleur ambiante de la scène
			shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
			// Position de la caméra
			shader->SendVector(shader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			lightCount = m_renderQueue.directionnalLights.size();
			for (unsigned int i = 0; i < lightCount; ++i)
				m_renderQueue.directionnalLights[i]->Enable(shader, i);

			lastShader = shader;
		}

		material->Apply(shader);

		// Mesh
		if (pair.second)
		{
			NzForwardRenderQueue::TransparentStaticModel& staticModel = m_renderQueue.transparentStaticModels[pair.first];

			const NzMatrix4f& matrix = staticModel.transformMatrix;
			NzStaticMesh* mesh = staticModel.mesh;

			const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
			const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> drawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				drawFunc = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = NzRenderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
			}

			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(vertexBuffer);

			// Calcul des lumières les plus proches
			if (lightCount < m_maxLightsPerObject && !m_renderQueue.lights.empty())
			{
				unsigned int count = lightManager.FindClosestLights(&m_renderQueue.lights[0], m_renderQueue.lights.size(), staticModel.aabb.GetBoundingSphere());
				count -= lightCount;

				for (unsigned int i = 0; i < count; ++i)
					lightManager.GetLight(i)->Enable(shader, lightCount++);
			}

			for (unsigned int i = lightCount; i < 3; ++i) ///TODO: Constante sur le nombre maximum de lumières
				NzLight::Disable(shader, i);

			NzRenderer::SetMatrix(nzMatrixType_World, matrix);
			drawFunc(mesh->GetPrimitiveMode(), 0, indexCount);
		}
		else
		{
			///TODO
		}
	}

	// Les billboards
	/*if (!m_renderQueue.billboards.empty())
	{
		//NzRenderer::SetIndexBuffer(m_billboardIndexBuffer);
		NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
		NzRenderer::SetShader(m_billboardShader);
		NzRenderer::SetVertexBuffer(m_billboardVertexBuffer);

		m_billboardShader->SendVector(s_cameraForwardLocation, camera->GetForward());
		m_billboardShader->SendVector(s_cameraUpLocation, camera->GetUp());
		m_billboardShader->SendVector(s_worldUpLocation, NzVector3f::Up());

		// Couleur ambiante de la scène
		m_billboardShader->SendColor(m_billboardShader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		// Position de la caméra
		m_billboardShader->SendVector(m_billboardShader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

		lightCount = 0;

		// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
		m_renderQueue.lights[0]->Apply(m_billboardShader, 0);

		for (auto& matIt : m_renderQueue.billboards)
		{
			const NzMaterial* material = matIt.first;
			auto& billboards = matIt.second;

			material->Apply(m_billboardShader);

			unsigned int billboardCount = billboards.size();
			const NzForwardRenderQueue::BillboardData* data = &billboards[0];
			while (billboardCount > 0)
			{
				unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboards);
				billboardCount -= renderedBillboardCount;

				m_billboardVertexBuffer->FillVertices(data, 0, renderedBillboardCount, true);
				data += renderedBillboardCount;

				NzRenderer::DrawPrimitives(nzPrimitiveMode_PointList, 0, renderedBillboardCount);
			}
			billboards.clear();
		}
	}*/

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();
}

unsigned int NzForwardRenderTechnique::GetMaxLightsPerObject() const
{
	return m_maxLightsPerObject;
}

NzAbstractRenderQueue* NzForwardRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

void NzForwardRenderTechnique::SetMaxLightsPerObject(unsigned int lightCount)
{
	m_maxLightsPerObject = lightCount; ///TODO: Vérifier par rapport à la constante
}