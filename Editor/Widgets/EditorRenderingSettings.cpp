#include "EditorRenderingSettings.h"

#include <experimental/filesystem>
#include <glm/vec2.hpp>
#include "Systems/GUI/ImGui/imgui.h"

#include "Systems/Settings/SettingsManager.h"
#include "Systems/Window/Window.h"
#include "Systems/Input/Input.h"

#include "Managers/RenderSettingsManager.h"

#include "Resources/Resources.h"

#include "Renderer/RenderSystem.h"
#include "Renderer/RenderManager.h"

#include "Utils/Files/FileSystem.h"

#include "Utils/Extensions/MathExtend.h"

#include "Debug/Statistics/StatisticsManager.h"
#include "Debug/Statistics/RSMStatisticsObject.h"
#include "Debug/Statistics/LPVStatisticsObject.h"
#include "Debug/Statistics/VCTStatisticsObject.h"
#include "Debug/Statistics/SSDOStatisticsObject.h"
#include "Debug/Statistics/SSAOStatisticsObject.h"
#include "Debug/Statistics/SSRStatisticsObject.h"
#include "Debug/Statistics/SSSubsurfaceScatteringStatisticsObject.h"

namespace fs = std::experimental::filesystem;

EditorRenderingSettings::EditorRenderingSettings () :
	_lutTexture (nullptr),
	_lutTextureView (nullptr),
	_continuousVoxelizationReset (false),
	_dialog (ImGuiFs::Dialog ())
{

}

void EditorRenderingSettings::Show ()
{
	bool visible = SettingsManager::Instance ()->GetValue<bool> ("menu_show_rendering_settings", false);

	if (visible) {
		std::string path = SettingsManager::Instance ()->GetValue<std::string> ("menu_show_rendering_settings_path", "");

		if (path == std::string ()) {
			return;
		}

		_settings = RenderSettingsManager::Instance ()->GetRenderSettings (path);

		if (_settings == nullptr) {
			_settings = Resources::LoadRenderSettings (path);
			RenderSettingsManager::Instance ()->AddRenderSettings (_settings);
		}

		if (_settings == nullptr) {
			return;
		}

		ShowRenderingSettingsWindow ();
	}
}

void EditorRenderingSettings::ShowRenderingSettingsWindow ()
{
	std::size_t windowHeight = Window::GetHeight ();

	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350, windowHeight - 30), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags windowFlags = 0;

	if (!ImGui::Begin("Rendering Settings", NULL, windowFlags)) {
		ImGui::End();

		return;
	}

	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

	std::map<std::string, int> renderModes;
	renderModes ["SceneRenderModule"] = 0;
	renderModes ["VoxelConeTracingRenderModule"] = 1;
	renderModes ["ReflectiveShadowMappingRenderModule"] = 2;
	renderModes ["TemporalReflectiveShadowMappingRenderModule"] = 3;
	renderModes ["LightPropagationVolumesRenderModule"] = 4;
	renderModes ["ScreenSpaceDirectionalOcclusionRenderModule"] = 5;

	int lastRenderMode = renderModes [_settings->renderMode];
	int renderMode = lastRenderMode;

	const char* items[] = { "Direct Light",
		"Voxel Cone Tracing",
		"Reflective Shadow Mapping",
		"Temporal Reflective Shadow Mapping",
		"Light Propagation Volumes",
		"Screen Space Global Illumination"
	};

	ImGui::Combo("Render Module", &renderMode, items, 6);

	const char* srenderModes[] = {
		"SceneRenderModule",
		"VoxelConeTracingRenderModule",
		"ReflectiveShadowMappingRenderModule",
		"TemporalReflectiveShadowMappingRenderModule",
		"LightPropagationVolumesRenderModule",
		"ScreenSpaceDirectionalOcclusionRenderModule"
	};

	if (lastRenderMode != renderMode) {
		_settings->renderMode = srenderModes [renderMode];
	}

	ImGui::Spacing ();

	if (ImGui::CollapsingHeader ("Voxel Cone Tracing")) {
		if (_continuousVoxelizationReset == true) {
			_settings->vct_continuous_voxelization = _lastContinuousVoxelization;
			_continuousVoxelizationReset = false;
		}

		std::size_t lastVoxelVolumeSize = _settings->vct_voxels_size;
		bool lastVoxelBordering = _settings->vct_bordering;
		std::size_t lastVolumeMipmapLevels = _settings->vct_mipmap_levels;

		ImGui::InputScalar ("Voxel Volume Size", ImGuiDataType_U32, &_settings->vct_voxels_size);
		ImGui::Checkbox ("Continuous Voxelization", &_settings->vct_continuous_voxelization);
		ImGui::Checkbox ("Voxel Volume Bordering", &_settings->vct_bordering);

		std::size_t speed = 1;
		ImGui::InputScalar ("Volume Mipmap Levels", ImGuiDataType_U32, &_settings->vct_mipmap_levels, &speed);
		_settings->vct_mipmap_levels = Extensions::MathExtend::Clamp (
			_settings->vct_mipmap_levels, (std::size_t) 1,
			(std::size_t) std::log2 (_settings->vct_voxels_size));

		ImGui::Separator ();

		ImGui::InputFloat ("Indirect Diffuse Light Intensity", &_settings->vct_indirect_diffuse_intensity, 0.1f);
		ImGui::InputFloat ("Indirect Specular Light Intensity", &_settings->vct_indirect_specular_intensity, 0.1f);
		ImGui::InputFloat ("Refractive Indirect Light Intensity", &_settings->vct_indirect_refractive_intensity, 0.1f);

        ImGui::Separator();

		ImGui::SliderFloat ("Diffuse Cone Distance", &_settings->vct_diffuse_cone_distance, 0.0f, 1.0f);

        ImGui::Separator();

		ImGui::SliderFloat ("Specular Cone Distance", &_settings->vct_specular_cone_distance, 0.0f, 1.0f);

        ImGui::Separator();

		ImGui::SliderFloat ("Refractive Cone Ratio", &_settings->vct_refractive_cone_ratio, 0.0f, 1.0f, "%3f", 10.0f);
		ImGui::SliderFloat ("Refractive Cone Distance", &_settings->vct_refractive_cone_distance, 0.0f, 1.0f);

        ImGui::Separator();

		ImGui::SliderFloat ("Shadow Cone Ratio", &_settings->vct_shadow_cone_ratio, 0.0f, 1.0f, "%3f", 10.0f);
		ImGui::SliderFloat ("Shadow Cone Distance", &_settings->vct_shadow_cone_distance, 0.0f, 1.0f);

        ImGui::Separator();

		ImGui::SliderFloat ("Origin Bias", &_settings->vct_origin_bias, 0.0f, 1.0f, "%5f", 10.0f);

		if (lastVoxelVolumeSize != _settings->vct_voxels_size ||
			lastVoxelBordering != _settings->vct_bordering ||
			lastVolumeMipmapLevels != _settings->vct_mipmap_levels) {
			_lastContinuousVoxelization = _settings->vct_continuous_voxelization;
			_continuousVoxelizationReset = true;

			_settings->vct_continuous_voxelization = true;
		}

        ImGui::Separator();

		if (ImGui::TreeNode ("Debug")) {

			ImGui::Checkbox ("Show Voxels", &_settings->vct_debug_show_voxels);

			std::size_t speed = 1;
			ImGui::InputScalar ("Mipmap Level", ImGuiDataType_U32, &_settings->vct_debug_volume_mipmap_level, &speed);
			_settings->vct_debug_volume_mipmap_level = Extensions::MathExtend::Clamp (
				_settings->vct_debug_volume_mipmap_level, (std::size_t) 0,
				(std::size_t) _settings->vct_mipmap_levels - 1);

			StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("VCTStatisticsObject");
			VCTStatisticsObject* vctStat = nullptr;

			if (stat != nullptr) {
				vctStat = dynamic_cast<VCTStatisticsObject*> (stat);
			}

			if (ImGui::TreeNode ("Indirect Light")) {
				if (vctStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* vctIndirectDiffuseMapVolume = vctStat->vctIndirectDiffuseMapVolume;
					FrameBuffer2DVolume* vctIndirectSpecularMapVolume = vctStat->vctIndirectSpecularMapVolume;
					FrameBuffer2DVolume* vctAmbientOcclusionMapVolume = vctStat->vctAmbientOcclusionMapVolume;
					FrameBuffer2DVolume* vctSubsurfaceScatteringMapVolume = vctStat->vctSubsurfaceScatteringMapVolume;

					glm::ivec2 vctMapSize = vctIndirectDiffuseMapVolume->GetSize ();

					int vctMapWidth = windowWidth;
					int vctMapHeight = ((float) vctMapSize.y / vctMapSize.x) * vctMapWidth;

					ImGui::Text ("Indirect Diffuse Light Map");
					ShowImage (vctIndirectDiffuseMapVolume->GetColorTextureID (), glm::ivec2 (vctMapWidth, vctMapHeight));

					ImGui::Text ("Indirect Specular Light Map");
					ShowImage (vctIndirectSpecularMapVolume->GetColorTextureID (), glm::ivec2 (vctMapWidth, vctMapHeight));

					ImGui::Text ("Subsurface Scattering Map");
					ShowImage (vctSubsurfaceScatteringMapVolume->GetColorTextureID (), glm::ivec2 (vctMapWidth, vctMapHeight));

					ImGui::Text ("Ambient Occlusion Map");
					ShowImage (vctAmbientOcclusionMapVolume->GetColorTextureID (), glm::ivec2 (vctMapWidth, vctMapHeight));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

    ImGui::Spacing();

	if (ImGui::CollapsingHeader ("Reflective Shadow Mapping")) {

		float scale = _settings->rsm_scale;
		ImGui::InputFloat ("Scale", &scale);
		if (scale > 0) {
			_settings->rsm_scale = scale;
		}

		std::size_t limit1 = 1, limit2 = 500;
		ImGui::SliderScalar ("Samples Size", ImGuiDataType_U32, &_settings->rsm_samples, &limit1, &limit2);

		ImGui::SliderFloat ("Sample Radius", &_settings->rsm_radius, 0.001, 1.0);

		ImGui::PushID ("RSMIndirect Light Intensity");
		ImGui::InputFloat ("Indirect Light Intensity", &_settings->rsm_intensity, 0.1);
		ImGui::PopID ();

		ImGui::InputFloat ("Indirect Specular Light Intensity", &_settings->rsm_specular_intensity, 0.1);
		ImGui::InputFloat ("Indirect Refractive Light Intensity", &_settings->rsm_indirect_refractive_intensity, 0.1);

		ImGui::Separator();

		ImGui::InputFloat ("Thickness", &_settings->rsm_thickness, 0.1);

		ImGui::Separator();

		ImGui::Checkbox ("Shown Non Interpolated Pixels", &_settings->rsm_debug_interpolation);

		float interpolationScale = _settings->rsm_interpolation_scale;
		ImGui::InputFloat ("Interpolation Scale", &interpolationScale);
		if (interpolationScale > 0) {
			_settings->rsm_interpolation_scale = interpolationScale;
		}

		ImGui::InputFloat ("Min Interpolation Distance", &_settings->rsm_min_interpolation_distance, 0.1);
		ImGui::InputFloat ("Min Interpolation Angle (deg)", &_settings->rsm_min_interpolation_angle, 0.1);

		ImGui::Separator();

		ImGui::PushID ("RSMDebug");

		if (ImGui::TreeNode ("Debug")) {

			StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("RSMStatisticsObject");
			RSMStatisticsObject* rsmStat = nullptr;

			if (stat != nullptr) {
				rsmStat = dynamic_cast<RSMStatisticsObject*> (stat);
			}

			if (ImGui::TreeNode ("Reflective Shadow Map")) {
				if (rsmStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.65f;

					ImGui::Text ("Depth Map");
					ShowImage (rsmStat->rsmDepthMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Position Map");
					ShowImage (rsmStat->rsmPosMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Normal Map");
					ShowImage (rsmStat->rsmNormalMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Flux Map");
					ShowImage (rsmStat->rsmFluxMapID, glm::ivec2 (windowWidth, windowWidth));
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode ("Indirect Light")) {
				if (rsmStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* rsmIndirectDiffuseMapVolume = rsmStat->rsmIndirectDiffuseMapVolume;
					FrameBuffer2DVolume* rsmIndirectSpecularMapVolume = rsmStat->rsmIndirectSpecularMapVolume;
					FrameBuffer2DVolume* rsmSubsurfaceScatteringMapVolume = rsmStat->rsmSubsurfaceScatteringMapVolume;
					FrameBuffer2DVolume* rsmAmbientOcclusionMapVolume = rsmStat->rsmAmbientOcclusionMapVolume;

					FrameBuffer2DVolume* rsmInterpolatedIndirectDiffuseMapVolume = rsmStat->rsmInterpolatedIndirectDiffuseMapVolume;

					glm::ivec2 rsmMapSize = rsmIndirectDiffuseMapVolume->GetSize ();

					int rsmMapWidth = windowWidth;
					int rsmMapHeight = ((float) rsmMapSize.y / rsmMapSize.x) * rsmMapWidth;

					ImGui::Text ("Screen Space Interpolation Indirect Diffuse Light Map");
					ShowImage (rsmInterpolatedIndirectDiffuseMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Indirect Diffuse Light Map");
					ShowImage (rsmIndirectDiffuseMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Indirect Specular Light Map");
					ShowImage (rsmIndirectSpecularMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Subsurface Scattering Map");
					ShowImage (rsmSubsurfaceScatteringMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Ambient Occlusion Map");
					ShowImage (rsmAmbientOcclusionMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::PopID ();
	}

    ImGui::Spacing();

	if (ImGui::CollapsingHeader ("Temporal Reflective Shadow Mapping")) {

		ImGui::Checkbox ("Temporal Filter Enabled", &_settings->trsm_temporal_filter_enabled);
		ImGui::Checkbox ("Blur Enabled", &_settings->trsm_blur_enabled);

		ImGui::Separator();

		ImGui::PushID ("TRSMDebug");
		if (ImGui::TreeNode ("Debug")) {

			StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("RSMStatisticsObject");
			RSMStatisticsObject* rsmStat = nullptr;

			if (stat != nullptr) {
				rsmStat = dynamic_cast<RSMStatisticsObject*> (stat);
			}

			if (ImGui::TreeNode ("Reflective Shadow Map")) {
				if (rsmStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.65f;

					ImGui::Text ("Depth Map");
					ShowImage (rsmStat->rsmDepthMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Position Map");
					ShowImage (rsmStat->rsmPosMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Normal Map");
					ShowImage (rsmStat->rsmNormalMapID, glm::ivec2 (windowWidth, windowWidth));

					ImGui::Text ("Flux Map");
					ShowImage (rsmStat->rsmFluxMapID, glm::ivec2 (windowWidth, windowWidth));
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode ("Indirect Light")) {
				if (rsmStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* rsmIndirectDiffuseMapVolume = rsmStat->rsmIndirectDiffuseMapVolume;
					FrameBuffer2DVolume* rsmIndirectSpecularMapVolume = rsmStat->rsmIndirectSpecularMapVolume;
					FrameBuffer2DVolume* rsmSubsurfaceScatteringMapVolume = rsmStat->rsmSubsurfaceScatteringMapVolume;
					FrameBuffer2DVolume* rsmAmbientOcclusionMapVolume = rsmStat->rsmAmbientOcclusionMapVolume;

					glm::ivec2 rsmMapSize = rsmIndirectDiffuseMapVolume->GetSize ();

					int rsmMapWidth = windowWidth;
					int rsmMapHeight = ((float) rsmMapSize.y / rsmMapSize.x) * rsmMapWidth;

					ImGui::Text ("Indirect Diffuse Light Map");
					ShowImage (rsmIndirectDiffuseMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Indirect Specular Light Map");
					ShowImage (rsmIndirectSpecularMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Subsurface Scattering Map");
					ShowImage (rsmSubsurfaceScatteringMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));

					ImGui::Text ("Ambient Occlusion Map");
					ShowImage (rsmAmbientOcclusionMapVolume->GetColorTextureID (), glm::ivec2 (rsmMapWidth, rsmMapHeight));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
		ImGui::PopID ();
	}

	ImGui::Spacing ();

	if (ImGui::CollapsingHeader ("Light Propagation Volumes")) {

		ImGui::InputScalar ("Volume Size", ImGuiDataType_U32, &_settings->lpv_volume_size);
		ImGui::InputScalar ("Iterations", ImGuiDataType_U32, &_settings->lpv_iterations);

		ImGui::InputFloat ("Light Injection Bias", &_settings->lpv_injection_bias, 0.1);

		ImGui::Checkbox ("Geometry Occlusion", &_settings->lpv_geometry_occlusion);

		ImGui::PushID ("LPVIndirect Light Intensity");
		ImGui::InputFloat ("Indirect Light Intensity", &_settings->lpv_intensity, 0.1);
		ImGui::PopID ();

		ImGui::InputFloat ("Indirect Refractive Intensity", &_settings->lpv_indirect_refractive_intensity, 0.1);

		ImGui::Separator();

		ImGui::PushID ("LPVDebug");

		if (ImGui::TreeNode ("Debug")) {

			StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("LPVStatisticsObject");
			LPVStatisticsObject* lpvStat = nullptr;

			if (stat != nullptr) {
				lpvStat = dynamic_cast<LPVStatisticsObject*> (stat);
			}

			if (lpvStat != nullptr) {

				int windowWidth = ImGui::GetWindowWidth() * 0.95f;

				FrameBuffer2DVolume* lpvIndirectDiffuseMapVolume = lpvStat->lpvIndirectDiffuseMapVolume;
				FrameBuffer2DVolume* lpvIndirectSpecularMapVolume = lpvStat->lpvIndirectSpecularMapVolume;
				FrameBuffer2DVolume* lpvSubsurfaceScatteringMapVolume = lpvStat->lpvSubsurfaceScatteringMapVolume;
				FrameBuffer2DVolume* lpvAmbientOcclusionMapVolume = lpvStat->lpvAmbientOcclusionMapVolume;

				glm::ivec2 lpvMapSize = lpvIndirectDiffuseMapVolume->GetSize ();

				int lpvMapWidth = windowWidth;
				int lpvMapHeight = ((float) lpvMapSize.y / lpvMapSize.x) * lpvMapWidth;

				ImGui::Text ("Indirect Diffuse Light Map");
				ShowImage (lpvIndirectDiffuseMapVolume->GetColorTextureID (), glm::ivec2 (lpvMapWidth, lpvMapHeight));

				ImGui::Text ("Indirect Specular Light Map");
				ShowImage (lpvIndirectSpecularMapVolume->GetColorTextureID (), glm::ivec2 (lpvMapWidth, lpvMapHeight));

				ImGui::Text ("Subsurface Scattering Map");
				ShowImage (lpvSubsurfaceScatteringMapVolume->GetColorTextureID (), glm::ivec2 (lpvMapWidth, lpvMapHeight));

				ImGui::Text ("Subsurface Scattering Map");
				ShowImage (lpvAmbientOcclusionMapVolume->GetColorTextureID (), glm::ivec2 (lpvMapWidth, lpvMapHeight));
			}

			ImGui::TreePop();
		}

		ImGui::PopID ();
	}

    ImGui::Spacing();

	if (ImGui::CollapsingHeader ("Post Processing")) {
		if (ImGui::TreeNode ("Screen Space Ambient Occlusion")) {

			ImGui::Checkbox ("Enabled", &_settings->ssao_enabled);

			float scale = _settings->ssao_scale;
			ImGui::InputFloat ("Scale", &scale);
			if (scale > 0) {
				_settings->ssao_scale = scale;
			}

			std::size_t limit1 = 0, limit2 = 200;
			ImGui::SliderScalar ("Samples Size", ImGuiDataType_U32, &_settings->ssao_samples, &limit1, &limit2);
			
			ImGui::InputScalar ("Noise Size", ImGuiDataType_U32, &_settings->ssao_noise_size);
			ImGui::InputFloat ("Radius", &_settings->ssao_radius, 0.1f);
			ImGui::InputFloat ("Bias", &_settings->ssao_bias, 0.1f);
			ImGui::Checkbox ("Blur Enabled", &_settings->ssao_blur_enabled);

			ImGui::Separator ();

			if (ImGui::TreeNode ("Debug")) {
				StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSAOStatisticsObject");
				SSAOStatisticsObject* ssaoStat = nullptr;

				if (stat != nullptr) {
					ssaoStat = dynamic_cast<SSAOStatisticsObject*> (stat);
				}

				if (ssaoStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* ssaoMapVolume = ssaoStat->ssaoMapVolume;

					glm::ivec2 ssaoMapSize = ssaoMapVolume->GetSize ();

					int ssaoMapWidth = windowWidth;
					int ssaoMapHeight = ((float) ssaoMapSize.y / ssaoMapSize.x) * ssaoMapWidth;

					ImGui::Text ("SSAO Map");
					ShowImage (ssaoMapVolume->GetColorTextureID (), glm::ivec2 (ssaoMapWidth, ssaoMapHeight));

					ImGui::Text ("SSAO Noise Map");
					ShowImage (ssaoStat->noiseMapID, glm::ivec2 (windowWidth, windowWidth));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("Screen Space Directional Occlusion")) {

			ImGui::Checkbox ("Enabled", &_settings->ssdo_enabled);
			ImGui::Checkbox ("Temporal Filter Enabled", &_settings->ssdo_temporal_filter_enabled);

			float scale = _settings->ssdo_scale;
			ImGui::InputFloat ("Scale", &scale);
			if (scale > 0) {
				_settings->ssdo_scale = scale;
			}

			std::size_t limit1 = 1, limit2 = 500;
			ImGui::SliderScalar ("Samples Size", ImGuiDataType_U32, &_settings->ssdo_samples, &limit1, &limit2);

			ImGui::InputFloat ("Radius", &_settings->ssdo_radius, 0.1f);
			ImGui::InputFloat ("Bias", &_settings->ssdo_bias, 0.1f);

			ImGui::InputFloat ("Indirect Light Intensity", &_settings->ssdo_indirect_intensity, 0.1f);

			ImGui::Separator ();

			ImGui::Checkbox ("Shadow 2D Ray Cast", &_settings->ssdo_ray_shadow);

			float shadowScale = _settings->ssdo_shadow_scale;
			ImGui::InputFloat ("Shadow Scale", &shadowScale);
			if (shadowScale > 0) {
				_settings->ssdo_shadow_scale = shadowScale;
			}

			std::size_t strideStep = 1;
			ImGui::InputScalar ("Shadow Stride", ImGuiDataType_U32, &_settings->ssdo_shadow_stride, &strideStep);

			ImGui::Separator ();

			if (ImGui::TreeNode ("Debug")) {
				StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSDOStatisticsObject");
				SSDOStatisticsObject* ssdoStat = nullptr;

				if (stat != nullptr) {
					ssdoStat = dynamic_cast<SSDOStatisticsObject*> (stat);
				}

				if (ssdoStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.9f;

					FrameBuffer2DVolume* ssdoMapVolume = ssdoStat->ssdoMapVolume;

					glm::ivec2 size = ssdoMapVolume->GetSize ();

					int width = windowWidth;
					int height = ((float) size.y / size.x) * width;

					ImGui::Text ("SSDO Map");
					ShowImage (ssdoMapVolume->GetColorTextureID (), glm::ivec2 (width, height));

					FrameBuffer2DVolume* ssdoShadowVolume = ssdoStat->ssdoShadowVolume;

					ImGui::Text ("SSDO Shadow Map");
					ShowImage (ssdoShadowVolume->GetColorTextureID (), glm::ivec2 (width, height));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("Screen Space Reflection")) {

			ImGui::Checkbox ("Enabled", &_settings->ssr_enabled);

			float scale = _settings->ssr_scale;
			ImGui::InputFloat ("Scale", &scale);
			if (scale > 0) {
				_settings->ssr_scale = scale;
			}

			ImGui::SliderFloat ("Roughness", &_settings->ssr_roughness, 0.0f, 1.0f);

			std::size_t step = 1;
			ImGui::InputScalar ("Sample Iterations", ImGuiDataType_U32, &_settings->ssr_iterations, &step);

			ImGui::SliderFloat ("Sample Thickness", &_settings->ssr_thickness, 0.0f, 10.0f);

			std::size_t strideStep = 1;
			ImGui::InputScalar ("Stride", ImGuiDataType_U32, &_settings->ssr_stride, &strideStep);

			ImGui::InputFloat ("Intensity", &_settings->ssr_intensity, 0.1f);

			ImGui::Separator ();

			if (ImGui::TreeNode ("Debug")) {
				StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSRStatisticsObject");
				SSRStatisticsObject* ssrStat = nullptr;

				if (stat != nullptr) {
					ssrStat = dynamic_cast<SSRStatisticsObject*> (stat);
				}

				if (ssrStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* ssrPositionMapVolume = ssrStat->ssrPositionMapVolume;
					FrameBuffer2DVolume* ssrMapVolume = ssrStat->ssrMapVolume;

					glm::ivec2 ssrMapSize = ssrPositionMapVolume->GetSize ();

					int ssrMapWidth = windowWidth;
					int ssrMapHeight = ((float) ssrMapSize.y / ssrMapSize.x) * ssrMapWidth;

					ImGui::Text ("SSR Position Map");
					ShowImage (ssrPositionMapVolume->GetColorTextureID (), glm::ivec2 (ssrMapWidth, ssrMapHeight));

					ImGui::Text ("SSR Map");
					ShowImage (ssrMapVolume->GetColorTextureID (), glm::ivec2 (ssrMapWidth, ssrMapHeight));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("Screen Space Subsurface Scattering")) {

			// ImGui::Checkbox ("Enabled", &_settings->ssr_enabled);

			// float scale = _settings->ssr_scale;
			// ImGui::InputFloat ("Scale", &scale);
			// if (scale > 0) {
			// 	_settings->ssr_scale = scale;
			// }

			// ImGui::SliderFloat ("Roughness", &_settings->ssr_roughness, 0.0f, 1.0f);

			// std::size_t step = 1;
			// ImGui::InputScalar ("Sample Iterations", ImGuiDataType_U32, &_settings->ssr_iterations, &step);

			// ImGui::SliderFloat ("Sample Thickness", &_settings->ssr_thickness, 0.0f, 10.0f);

			// std::size_t strideStep = 1;
			// ImGui::InputScalar ("Stride", ImGuiDataType_U32, &_settings->ssr_stride, &strideStep);

			// ImGui::InputFloat ("Intensity", &_settings->ssr_intensity, 0.1f);

			// ImGui::Separator ();

			if (ImGui::TreeNode ("Debug")) {
				StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("SSSubsurfaceScatteringStatisticsObject");
				SSSubsurfaceScatteringStatisticsObject* ssrStat = nullptr;

				if (stat != nullptr) {
					ssrStat = dynamic_cast<SSSubsurfaceScatteringStatisticsObject*> (stat);
				}

				if (ssrStat != nullptr) {

					int windowWidth = ImGui::GetWindowWidth() * 0.95f;

					FrameBuffer2DVolume* ssrMapVolume = ssrStat->ssrMapVolume;

					glm::ivec2 ssrMapSize = ssrMapVolume->GetSize ();

					int ssrMapWidth = windowWidth;
					int ssrMapHeight = ((float) ssrMapSize.y / ssrMapSize.x) * ssrMapWidth;

					ImGui::Text ("SSR Map");
					ShowImage (ssrMapVolume->GetColorTextureID (), glm::ivec2 (ssrMapWidth, ssrMapHeight));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("Temporal Anti-aliasing")) {

			ImGui::Checkbox ("Enabled", &_settings->taa_enabled);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("Bloom")) {

			ImGui::Checkbox ("Enabled", &_settings->bloom_enabled);
			ImGui::SliderFloat ("Threshold", &_settings->bloom_threshold, 0.0f, 10.0f);
			ImGui::InputFloat ("Intensity", &_settings->bloom_intensity, 0.1f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("High Dynamic Range")) {

			ImGui::Checkbox ("Enabled", &_settings->hdr_enabled);
			ImGui::InputFloat ("Exposure", &_settings->hdr_exposure, 0.1f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode ("LUT Texture")) {

			ImGui::Checkbox ("Enabled", &_settings->lut_enabled);

			ImGui::Spacing ();
			ImGui::Spacing ();

			std::string lutTexturePath = _settings->lut_texture_path;

			ImGui::Text ("Path: %s", lutTexturePath.c_str ());

			if (_lutTexture == nullptr || _lastLUTTexturePath != lutTexturePath) {
				_lutTexture = Resources::LoadTexture (lutTexturePath);
				_lutTextureView = RenderSystem::LoadTexture (_lutTexture);
				_lastLUTTexturePath = lutTexturePath;
			}

			int windowWidth = ImGui::GetWindowWidth() * 0.6f;

			auto imageSize = _lutTexture->GetSize ();
			imageSize.height = windowWidth * ((float) imageSize.height / imageSize.width);
			imageSize.width = windowWidth;
			ImGui::Image((void*)(intptr_t) _lutTextureView->GetGPUIndex (), ImVec2(imageSize.width, imageSize.height));

			ImGui::SameLine ();

			bool lastLoadTexture = ImGui::Button ("Load", ImVec2 (48, 18));

			const char* path = _dialog.chooseFileDialog(lastLoadTexture);

			if (strlen (path) > 0) {
				_settings->lut_texture_path = path;				
			}

			ImGui::SliderFloat ("Intensity", &_settings->lut_intensity, 0.0f, 1.0f);

			ImGui::TreePop ();
		}

		if (ImGui::TreeNode ("Gamma Correction")) {

			ImGui::Checkbox ("Enabled", &_settings->gamma_enabled);

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void EditorRenderingSettings::ShowImage (unsigned int textureID, const glm::ivec2& size)
{
	ImGui::Image((void*)(intptr_t) textureID, ImVec2(size.x, size.y), ImVec2 (0, 1), ImVec2 (1, 0));

    bool saveVolume = false;

    ImGui::PushID (textureID);

    if (ImGui::BeginPopupContextItem(std::to_string (textureID).c_str ())) {
		saveVolume = ImGui::MenuItem ("Save");

	    ImGui::EndPopup();
    }

	std::string volumePath = _dialog.saveFileDialog(saveVolume, "", "image.png", ".png");

	if (volumePath != std::string ()) {

		volumePath = FileSystem::Relative (volumePath, fs::current_path ().string ());

		Resource<TextureView> textureView (new TextureView (), "temp");
		textureView->SetGPUIndex (textureID);

		Resource<Texture> texture = RenderSystem::SaveTexture (textureView);

		textureView->SetGPUIndex (0);

		Resources::SaveTexture (texture, volumePath);
	}

	ImGui::PopID ();
}
