#include "SSAOBlurRenderPass.h"

#include "AmbientOcclusionMapVolume.h"

#include "Debug/Statistics/StatisticsManager.h"
#include "Debug/Statistics/SSAOStatisticsObject.h"

bool SSAOBlurRenderPass::IsAvailable (const RenderScene* renderScene, const Camera* camera,
	const RenderSettings& settings, const RenderVolumeCollection* rvc) const
{
	/*
	 * Check if screen space ambient occlusion blur is enabled
	*/

	return settings.ssao_enabled && settings.ssao_blur_enabled;
}

std::string SSAOBlurRenderPass::GetPostProcessFragmentShaderPath () const
{
	return "Assets/Shaders/AmbientOcclusion/screenSpaceAmbientOcclusionBlurFragment.glsl";
}

std::string SSAOBlurRenderPass::GetPostProcessVolumeName () const
{
	return "SSAOMapVolume";
}

glm::ivec2 SSAOBlurRenderPass::GetPostProcessVolumeResolution (const RenderSettings& settings) const
{
	if (_postProcessMapVolume != nullptr) {
		StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSAOStatisticsObject");
		SSAOStatisticsObject* ssaoStatisticsObject = nullptr;

		if (stat == nullptr) {
			stat = new SSAOStatisticsObject ();
			StatisticsManager::Instance ()->SetStatisticsObject ("SSAOStatisticsObject", stat);
		}

		ssaoStatisticsObject = dynamic_cast<SSAOStatisticsObject*> (stat);

		ssaoStatisticsObject->ssaoMapVolume = _postProcessMapVolume;
	}

	return glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.ssao_scale);
}

PostProcessMapVolume* SSAOBlurRenderPass::CreatePostProcessVolume () const
{
	AmbientOcclusionMapVolume* ssaoBlurMapVolume = new AmbientOcclusionMapVolume ();

	return ssaoBlurMapVolume;
}

std::vector<PipelineAttribute> SSAOBlurRenderPass::GetCustomAttributes (const Camera* camera,
	const RenderSettings& settings, RenderVolumeCollection* rvc)
{
	/*
	 * Attach post process volume attributes to pipeline
	*/

	std::vector<PipelineAttribute> attributes = PostProcessRenderPass::GetCustomAttributes (camera, settings, rvc);

	/*
	 * Attach screen space ambient occlusion attributes to pipeline
	*/

	PipelineAttribute ssaoResolution;

	ssaoResolution.type = PipelineAttribute::AttrType::ATTR_2F;

	ssaoResolution.name = "ssaoResolution";

	glm::ivec2 resolution = glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.ssao_scale);

	ssaoResolution.value = glm::vec3 (resolution, 0.0f);

	attributes.push_back (ssaoResolution);

	return attributes;
}
