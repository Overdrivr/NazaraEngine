// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_GRAPHICS_HPP
#define NAZARA_ENUMS_GRAPHICS_HPP

enum nzBackgroundType
{
	nzBackgroundType_Color,   // NzColorBackground
	nzBackgroundType_Skybox,  // NzSkyboxBackground
	nzBackgroundType_Texture, // NzTextureBackground
	nzBackgroundType_User,

	nzBackgroundType_Max = nzBackgroundType_User
};

enum nzLightType
{
	nzLightType_Directional,
	nzLightType_Point,
	nzLightType_Spot,

	nzLightType_Max = nzLightType_Spot
};

enum nzMaterialUniform
{
	nzMaterialUniform_AlphaMap,
	nzMaterialUniform_AlphaThreshold,
	nzMaterialUniform_Ambient,
	nzMaterialUniform_Diffuse,
	nzMaterialUniform_DiffuseMap,
	nzMaterialUniform_EmissiveMap,
	nzMaterialUniform_HeightMap,
	nzMaterialUniform_NormalMap,
	nzMaterialUniform_Shininess,
	nzMaterialUniform_Specular,
	nzMaterialUniform_SpecularMap,

	nzMaterialUniform_Max = nzMaterialUniform_SpecularMap
};

enum nzParticleComponent
{
	nzParticleComponent_Unused = -1,

	nzParticleComponent_Color,
	nzParticleComponent_Life,
	nzParticleComponent_Mass,
	nzParticleComponent_Normal,
	nzParticleComponent_Position,
	nzParticleComponent_Radius,
	nzParticleComponent_Rotation,
	nzParticleComponent_Size,
	nzParticleComponent_Velocity,
	nzParticleComponent_Userdata0,
	nzParticleComponent_Userdata1,
	nzParticleComponent_Userdata2,
	nzParticleComponent_Userdata3,
	nzParticleComponent_Userdata4,
	nzParticleComponent_Userdata5,
	nzParticleComponent_Userdata6,
	nzParticleComponent_Userdata7,
	nzParticleComponent_Userdata8,

	nzParticleComponent_Max = nzParticleComponent_Userdata8
};

enum nzParticleLayout
{
	nzParticleLayout_Billboard,
	nzParticleLayout_Model,
	nzParticleLayout_Sprite,

	nzParticleLayout_Max = nzParticleLayout_Sprite
};

enum nzRenderPassType
{
	nzRenderPassType_AA,
	nzRenderPassType_Bloom,
	nzRenderPassType_DOF,
	nzRenderPassType_Final,
	nzRenderPassType_Fog,
	nzRenderPassType_Forward,
	nzRenderPassType_Lighting,
	nzRenderPassType_Geometry,
	nzRenderPassType_SSAO,

	nzRenderPassType_Max = nzRenderPassType_SSAO
};

enum nzRenderTechniqueType
{
	nzRenderTechniqueType_AdvancedForward, // NzAdvancedForwardRenderTechnique
	nzRenderTechniqueType_BasicForward,    // NzBasicForwardRenderTechnique
	nzRenderTechniqueType_DeferredShading, // NzDeferredRenderTechnique
	nzRenderTechniqueType_LightPrePass,    // NzLightPrePassRenderTechnique
	nzRenderTechniqueType_User,

	nzRenderTechniqueType_Max = nzRenderTechniqueType_User
};

enum nzSceneNodeType
{
	nzSceneNodeType_Light,           // NzLight
	nzSceneNodeType_Model,           // NzModel
	nzSceneNodeType_ParticleEmitter, // NzParticleEmitter
	nzSceneNodeType_Root,            // NzSceneRoot
	nzSceneNodeType_Sprite,          // NzSprite
	nzSceneNodeType_TextSprite,      // NzTextSprite
	nzSceneNodeType_User,

	nzSceneNodeType_Max = nzSceneNodeType_User
};

// Ces paramètres sont indépendants du matériau: ils peuvent être demandés à tout moment
enum nzShaderFlags
{
	nzShaderFlags_None = 0,

	nzShaderFlags_Billboard      = 0x01,
	nzShaderFlags_Deferred       = 0x02,
	nzShaderFlags_Instancing     = 0x04,
	nzShaderFlags_TextureOverlay = 0x08,
	nzShaderFlags_VertexColor    = 0x10,

	nzShaderFlags_Max = nzShaderFlags_VertexColor*2-1
};

#endif // NAZARA_ENUMS_GRAPHICS_HPP
