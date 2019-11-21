#ifndef RSMTEMPORALFILTERRENDERPASS_H
#define RSMTEMPORALFILTERRENDERPASS_H

#include "RenderPasses/PostProcess/PostProcessRenderPass.h"

#include <glm/mat4x4.hpp>

class ENGINE_API RSMTemporalFilterRenderPass : public PostProcessRenderPass
{
	DECLARE_RENDER_PASS(RSMTemporalFilterRenderPass)

public:
	bool IsAvailable (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, const RenderVolumeCollection* rvc) const;
protected:
	std::string GetPostProcessFragmentShaderPath () const;
	std::string GetPostProcessVolumeName () const;
	glm::ivec2 GetPostProcessVolumeResolution (const RenderSettings& settings) const;
	PostProcessMapVolume* CreatePostProcessVolume () const;

	std::vector<PipelineAttribute> GetCustomAttributes (const Camera* camera,
		const RenderSettings& settings, RenderVolumeCollection* rvc);
};

#endif