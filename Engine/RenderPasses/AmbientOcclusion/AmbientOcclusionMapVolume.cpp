#include "AmbientOcclusionMapVolume.h"

#include "Wrappers/OpenGL/GL.h"

#include "Core/Console/Console.h"

bool AmbientOcclusionMapVolume::Init (std::size_t bufferWidth, std::size_t bufferHeight)
{
	/*
	 * Create framebuffer
	*/

	GL::GenFramebuffers (1, &_fbo);
	GL::BindFramebuffer (GL_FRAMEBUFFER, _fbo);

	/*
	 * Create the 2D color buffer texture
	*/

	GL::GenTextures (1, &_colorBuffer);
	GL::BindTexture (GL_TEXTURE_2D, _colorBuffer);

	GL::TexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GL::TexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL::TexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GL::TexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GL::TexImage2D (GL_TEXTURE_2D, 0, GL_RED, bufferWidth, bufferHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
		
	GL::FramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0);

	/*
	 * Get framebuffer status
	*/

	GLenum status = GL::CheckFramebufferStatus (GL_FRAMEBUFFER);

	/*
	 * Unbind framebuffer
	*/

	GL::BindFramebuffer (GL_FRAMEBUFFER, 0);

	/*
	 * Check that framebuffer was successfully initialized
	*/

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		Console::LogError ("Framebuffer status error: " + status);
		return false;
	}

	return true;
}

void AmbientOcclusionMapVolume::BindForReading ()
{
	/*
	 * Do nothing
	*/
}

std::vector<PipelineAttribute> AmbientOcclusionMapVolume::GetCustomAttributes () const
{
	std::vector<PipelineAttribute> attributes;

	PipelineAttribute ambientOcclusionMap;

	ambientOcclusionMap.type = PipelineAttribute::AttrType::ATTR_TEXTURE_2D;

	ambientOcclusionMap.name = "ambientOcclusionMap";

	ambientOcclusionMap.value.x = _colorBuffer;

	attributes.push_back (ambientOcclusionMap);

	return attributes;
}
