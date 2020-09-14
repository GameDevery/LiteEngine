#ifndef LPVVOLUME_H
#define LPVVOLUME_H

#include "Renderer/RenderVolumeI.h"

#include <glm/glm.hpp>
#include <vector>

#include "Renderer/PipelineAttribute.h"

#define LIGHT_PROPAGATION_VOLUME_TEXTURE_NOT_INIT 390

class LPVVolume : public RenderVolumeI
{
protected:
	unsigned int _volumeTextures [3];
	unsigned int _volumeFbo;
	std::size_t _volumeSize;

	glm::vec3 _minVertex;
	glm::vec3 _maxVertex;

	std::vector<PipelineAttribute> _attributes;

public:
	LPVVolume ();
	~LPVVolume ();

	virtual bool Init (std::size_t size);

	virtual void BindForWriting ();
	virtual const std::vector<PipelineAttribute>& GetCustomAttributes () const;

	virtual void ClearVolume();
	virtual void UpdateBoundingBox (const glm::vec3& minVertex, const glm::vec3& maxVertex);

	std::size_t GetVolumeSize () const;
	glm::vec3 GetMinVertex () const;
	glm::vec3 GetMaxVertex () const;

	virtual void Clear ();
};

#endif