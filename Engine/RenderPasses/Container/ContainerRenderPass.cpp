#include "ContainerRenderPass.h"

ContainerRenderPass::ContainerRenderPass (
	const std::vector<ContainerRenderSubPassI*>& renderSubPasses,
	ContainerRenderVolumeCollectionI* renderVolumeCollection) :
	_renderSubPasses (renderSubPasses),
	_renderVolumeCollection (renderVolumeCollection)
{

}

ContainerRenderPass::~ContainerRenderPass ()
{
	/*
	 * Free memory of container volume
	*/

	delete _renderVolumeCollection;

	/*
	 * Free memory of all sub passes
	*/

	for (auto renderSubPass : _renderSubPasses) {
		delete renderSubPass;
	}
}

void ContainerRenderPass::Init ()
{
	/*
	 * Iterate over every sub pass and initialize it
	*/

	for (auto renderSubPass : _renderSubPasses) {
		renderSubPass->Init ();
	}
}

RenderVolumeCollection* ContainerRenderPass::Execute (const Scene* scene, 
	const Camera* camera, RenderVolumeCollection* rvc)
{
	rvc->StartScope ();

	RenderVolumeI* volume = nullptr;

	/*
	 * Reset render container volume
	*/

	_renderVolumeCollection->Reset ();

	/*
	 * Iterate every render volume provided by specialization
	*/

	while ((volume = _renderVolumeCollection->GetNextVolume ()) != nullptr) {

		/*
		 * Execute all sub passes using provided volume
		*/

		IterateOverSubPasses (volume, scene, camera, rvc);
	}

	return rvc->ReleaseScope ();
}

bool ContainerRenderPass::IsAvailable (const Scene* scene, const Camera* camera, const RenderVolumeCollection* rvc) const
{
	/*
	 * Always execute container render pass
	*/

	return true;
}

void ContainerRenderPass::Clear ()
{
	/*
	 * Clear every subpass
	*/

	for (auto renderSubPass : _renderSubPasses) {
		renderSubPass->Clear ();
	}
}

ContainerRenderPassBuilder ContainerRenderPass::Builder ()
{
	ContainerRenderPassBuilder builder;

	return builder;
}

RenderVolumeCollection* ContainerRenderPass::IterateOverSubPasses (
	RenderVolumeI* volume, const Scene* scene, const Camera* camera,
	RenderVolumeCollection* rvc)
{
	/*
	 * Insert volume in render volume collection
	 * In this way it could be obtained at pass execution
	*/

	rvc->Insert ("SubpassVolume", volume, false);

	/*
	 * Iterate all sub passes
	*/

	for (auto renderSubPass : _renderSubPasses) {

		/*
		 * Check if sub pass admits current volume
		*/

		if (!renderSubPass->IsAvailable (scene, camera, rvc)) {
			continue;
		}

		/*
		 * Execute render sub pass
		*/

		rvc = renderSubPass->Execute (scene, camera, rvc);
	}

	return rvc;
}