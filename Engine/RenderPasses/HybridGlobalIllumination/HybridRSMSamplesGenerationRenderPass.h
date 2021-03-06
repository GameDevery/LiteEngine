#ifndef HYBRIDRSMSAMPLESGENERATIONRENDERPASS_H
#define HYBRIDRSMSAMPLESGENERATIONRENDERPASS_H

#include "RenderPasses/Container/ContainerRenderSubPassI.h"

#include "HGISamplesVolume.h"

class ENGINE_API HybridRSMSamplesGenerationRenderPass : public ContainerRenderSubPassI
{
	DECLARE_RENDER_PASS(HybridRSMSamplesGenerationRenderPass)

protected:
	HGISamplesVolume* _hgiSamplesVolume;

public:
	HybridRSMSamplesGenerationRenderPass ();

	virtual void Init (const RenderSettings& settings);
	RenderVolumeCollection* Execute (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, RenderVolumeCollection* rvc);

	bool IsAvailable (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, const RenderVolumeCollection* rvc) const;

	void Clear ();
protected:
	void UpdateSamplesVolume (const RenderSettings& settings);
};

#endif
