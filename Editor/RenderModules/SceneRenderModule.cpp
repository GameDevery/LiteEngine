#include "SceneRenderModule.h"

#include "RenderPasses/ResultFrameBufferGenerationRenderPass.h"
#include "RenderPasses/DeferredGeometryRenderPass.h"
#include "RenderPasses/DeferredSkyboxRenderPass.h"
#include "RenderPasses/DeferredBlitRenderPass.h"
#include "RenderPasses/ForwardRenderPass.h"
#include "RenderPasses/GUI/GUIGizmosRenderPass.h"

#include "RenderPasses/Container/ContainerRenderPass.h"
#include "RenderPasses/IterateOverRenderVolumeCollection.h"

#include "RenderPasses/AmbientOcclusion/SSAOSamplesGenerationRenderPass.h"
#include "RenderPasses/AmbientOcclusion/SSAONoiseGenerationRenderPass.h"
#include "RenderPasses/AmbientOcclusion/SSAORenderPass.h"
#include "RenderPasses/AmbientOcclusion/SSAOBlurRenderPass.h"

#include "RenderPasses/ShadowMap/DirectionalLightShadowMapRenderPass.h"
// #include "RenderPasses/ShadowMap/DirectionalLightExponentialShadowMapRenderPass.h"
// #include "RenderPasses/ShadowMap/ExponentialShadowMapBlurRenderPass.h"
#include "RenderPasses/DeferredDirectionalLightRenderPass.h"
#include "RenderPasses/DirectionalLightContainerRenderVolumeCollection.h"

#include "RenderPasses/DeferredPointLightRenderPass.h"
#include "RenderPasses/PointLightContainerRenderVolumeCollection.h"

#include "RenderPasses/DeferredAmbientLightRenderPass.h"

#include "RenderPasses/IdleRenderPass.h"
#include "RenderPasses/ScreenSpaceReflection/SSRRenderPass.h"
#include "RenderPasses/ScreenSpaceReflection/SSRAccumulationRenderPass.h"
#include "RenderPasses/Bloom/BrightExtractionRenderPass.h"
#include "RenderPasses/Bloom/BloomHorizontalBlurRenderPass.h"
#include "RenderPasses/Bloom/BloomVerticalBlurRenderPass.h"
#include "RenderPasses/Bloom/BloomAccumulationRenderPass.h"
#include "RenderPasses/HighDynamicRange/HDRRenderPass.h"
#include "RenderPasses/GammaCorrection/GammaCorrectionRenderPass.h"

void SceneRenderModule::Init ()
{
	_renderPasses.push_back (new ResultFrameBufferGenerationRenderPass ());
	_renderPasses.push_back (new DeferredGeometryRenderPass ());	
	_renderPasses.push_back (ContainerRenderPass::Builder ()
		.Volume (new DirectionalLightContainerRenderVolumeCollection ())
		.Attach (new DirectionalLightShadowMapRenderPass ())
		// .Attach (new DirectionalLightExponentialShadowMapRenderPass ())
		// .Attach (new ExponentialShadowMapBlurRenderPass ())
		.Attach (new DeferredDirectionalLightRenderPass ())
		.Build ());
	_renderPasses.push_back (ContainerRenderPass::Builder ()
		.Volume (new PointLightContainerRenderVolumeCollection ())
		.Attach (new DeferredPointLightRenderPass ())
		.Build ());
	_renderPasses.push_back (ContainerRenderPass::Builder ()
		.Volume (new IterateOverRenderVolumeCollection (1))
		.Attach (new SSAOSamplesGenerationRenderPass ())
		.Attach (new SSAONoiseGenerationRenderPass ())
		.Attach (new SSAORenderPass ())
		.Attach (new SSAOBlurRenderPass ())
		.Build ());
	_renderPasses.push_back (ContainerRenderPass::Builder ()
		.Volume (new IterateOverRenderVolumeCollection (1))
		.Attach (new DeferredAmbientLightRenderPass ())
		.Build ());
	_renderPasses.push_back (new DeferredSkyboxRenderPass ());
	_renderPasses.push_back (ContainerRenderPass::Builder ()
		.Volume (new IterateOverRenderVolumeCollection (1))
		.Attach (new IdleRenderPass ())
		.Attach (ContainerRenderPass::Builder ()
			.Volume (new IterateOverRenderVolumeCollection (1))
			.Attach (new SSRRenderPass ())
			.Attach (new SSRAccumulationRenderPass ())
			.Build ())
		.Attach (ContainerRenderPass::Builder ()
			.Volume (new IterateOverRenderVolumeCollection (1))
			.Attach (new BrightExtractionRenderPass ())
			.Attach (ContainerRenderPass::Builder ()
				.Volume (new IterateOverRenderVolumeCollection (5))
				.Attach (new BloomHorizontalBlurRenderPass ())
				.Attach (new BloomVerticalBlurRenderPass ())
				.Build ())
			.Attach (new BloomAccumulationRenderPass ())
			.Build ())
		.Attach (new HDRRenderPass ())
		.Attach (new GammaCorrectionRenderPass ())
		.Build ());
	_renderPasses.push_back (new DeferredBlitRenderPass ());
	_renderPasses.push_back (new ForwardRenderPass ());
	_renderPasses.push_back (new GUIGizmosRenderPass ());
}
