#ifndef FORWARDRENDERPASS_H
#define FORWARDRENDERPASS_H

#include "Renderer/RenderPassI.h"

class ForwardRenderPass : public RenderPassI
{
public:
	virtual void Init ();
	virtual RenderVolumeCollection* Execute (const Scene* scene, const Camera* camera, RenderVolumeCollection* rvc);

	void Clear ();
protected:
	void ForwardPass (const Scene*);
};

#endif