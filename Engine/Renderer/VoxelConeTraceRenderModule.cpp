#include "VoxelConeTraceRenderModule.h"

#include "Lighting/LightsManager.h"

#include "Wrappers/OpenGL/GL.h"

#include "Debug/Profiler/Profiler.h"

VoxelConeTraceRenderModule::VoxelConeTraceRenderModule ()
{

}

VoxelConeTraceRenderModule::~VoxelConeTraceRenderModule ()
{

}

void VoxelConeTraceRenderModule::RenderScene (Scene* scene, Camera* camera)
{
	/*
	 * Send Camera to Pipeline
	*/

	DeferredRenderModule::UpdateCamera (camera);

	/*
	 * Voxelize the scene
	*/

	static bool firstTime = true;

	if (firstTime) {
		VoxelizePass (scene);
		firstTime = false;
	}

	/*
	 * Deferred Rendering Pass
	*/

	DeferredPass (scene, camera);

	/*
	 * Forward Rendering Pass
	*/

	ForwardPass (scene);
}

void VoxelConeTraceRenderModule::DeferredPass (Scene* scene, Camera* camera)
{
	PROFILER_LOGGER ("Deferred Pass");

	/*
	 * Update GBuffer if needed
	*/

	UpdateGBuffer ();

	/*
	 * Deferred Rendering: Prepare for rendering
	*/

	PrepareDrawing ();

	/*
	 * Deferred Rendering: Geometry Pass
	*/

	GeometryPass (scene, camera);

	/*
	 * Deferred Rendering: Light Pass (atm)
	*/

	LightPass (scene, camera);

	/*
	 * Deferred Rendering: Decorations Pass
	*/

	SkyboxPass (camera);

	/*
	 * Deferred Rendering: End Drawing
	*/

	EndDrawing ();
}

void VoxelConeTraceRenderModule::LightPass (Scene* scene, Camera* camera)
{
	/*
	 * Start cone trace pass
	*/

	StartLightPass ();

	/*
	 * Directional light pass, the only light pass with integrated
	 * cone tracing pass at the moment
	*/

	DirectionalLightPass (scene, camera);
}

void VoxelConeTraceRenderModule::StartLightPass ()
{
	_voxelVolume->StartConeTracePass ();
}

void VoxelConeTraceRenderModule::DirectionalLightPass (Scene* scene, Camera* camera)
{
	GL::Disable(GL_DEPTH_TEST);
	GL::BlendFunc (GL_ONE, GL_ZERO);

	for (std::size_t i=0;i<LightsManager::Instance ()->GetDirectionalLightsCount ();i++) {
		VolumetricLight* volumetricLight = LightsManager::Instance ()->GetDirectionalLight (i);

		if (!volumetricLight->IsActive ()) {
			continue;
		}

		volumetricLight->GetLightRenderer ()->Draw (scene, camera, _frameBuffer, _voxelVolume);
	}

	GL::Disable(GL_BLEND);
	GL::Enable (GL_DEPTH_TEST);
}