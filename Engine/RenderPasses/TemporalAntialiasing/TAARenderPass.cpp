#include "TAARenderPass.h"

bool TAARenderPass::IsAvailable (const RenderScene* renderScene, const Camera* camera,
	const RenderSettings& settings, const RenderVolumeCollection* rvc) const
{
	/*
	 * Check if temporal anti-aliasing is enabled
	*/

	return settings.taa_enabled;
}
