#include "SSDOShadowRenderPass.h"

#include "SSDOShadowVolume.h"

#include "RenderPasses/VolumetricLightVolume.h"

#include "Debug/Statistics/StatisticsManager.h"
#include "Debug/Statistics/SSDOStatisticsObject.h"

bool SSDOShadowRenderPass::IsAvailable (const RenderScene* renderScene, const Camera* camera,
	const RenderSettings& settings, const RenderVolumeCollection* rvc) const
{
	/*
	 * Always execute screen space shadow render pass
	*/

	return settings.ssdo_ray_shadow;
}

std::string SSDOShadowRenderPass::GetPostProcessFragmentShaderPath () const
{
	return "Assets/Shaders/ScreenSpaceDirectionalOcclusion/screenSpaceShadowFragment.glsl";
}

std::string SSDOShadowRenderPass::GetPostProcessVolumeName () const
{
	return "SSDOShadowVolume";
}

glm::ivec2 SSDOShadowRenderPass::GetPostProcessVolumeResolution (const RenderSettings& settings) const
{
	if (_postProcessMapVolume != nullptr) {
		StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSDOStatisticsObject");
		SSDOStatisticsObject* ssdoStatisticsObject = nullptr;

		if (stat == nullptr) {
			stat = new SSDOStatisticsObject ();
			StatisticsManager::Instance ()->SetStatisticsObject ("SSDOStatisticsObject", stat);
		}

		ssdoStatisticsObject = dynamic_cast<SSDOStatisticsObject*> (stat);

		ssdoStatisticsObject->ssdoShadowVolume = _postProcessMapVolume;
	}

	return glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.ssdo_shadow_scale);
}

PostProcessMapVolume* SSDOShadowRenderPass::CreatePostProcessVolume () const
{
	SSDOShadowVolume* ssdoShadowVolume = new SSDOShadowVolume ();

	return ssdoShadowVolume;
}

std::vector<PipelineAttribute> SSDOShadowRenderPass::GetCustomAttributes (const Camera* camera,
	const RenderSettings& settings, RenderVolumeCollection* rvc)
{
	/*
	 * Attach post process volume attributes to pipeline
	*/

	std::vector<PipelineAttribute> attributes = PostProcessRenderPass::GetCustomAttributes (camera, settings, rvc);

	/*
	 * Get volumetric light volume from render volume collection
	*/

	VolumetricLightVolume* volume = (VolumetricLightVolume*)rvc->GetRenderVolume ("SubpassVolume");

	/*
	 * Get volumetric light from volumetric light volume
	*/

	RenderLightObject* renderLightObject = volume->GetRenderLightObject ();

	/*
	 * Attach screen space shadow attributes to pipeline
	*/

	PipelineAttribute ssdoShadowResolution;
	PipelineAttribute ssdoShadowStride;
	PipelineAttribute lightDirection;

	ssdoShadowResolution.type = PipelineAttribute::AttrType::ATTR_2F;
	ssdoShadowStride.type = PipelineAttribute::AttrType::ATTR_1I;
	lightDirection.type = PipelineAttribute::AttrType::ATTR_3F;

	ssdoShadowResolution.name = "ssdoShadowResolution";
	ssdoShadowStride.name = "ssdoShadowStride";
	lightDirection.name = "lightDirection";

	glm::vec3 lightDir = renderLightObject->GetTransform ()->GetRotation () * glm::vec3 (0, 0, -1);

	glm::mat3 viewMatrix = glm::mat3_cast (camera->GetRotation ());
	lightDir = glm::normalize (viewMatrix * lightDir);

	glm::ivec2 resolution = glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.ssdo_shadow_scale);

	ssdoShadowResolution.value = glm::vec3 (resolution, 0.0f);
	ssdoShadowStride.value.x = settings.ssdo_shadow_stride;
	lightDirection.value = lightDir;

	attributes.push_back (ssdoShadowResolution);
	attributes.push_back (ssdoShadowStride);
	attributes.push_back (lightDirection);

	return attributes;
}
