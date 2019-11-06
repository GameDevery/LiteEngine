#ifndef RENDERPOINTLIGHTOBJECT_H
#define RENDERPOINTLIGHTOBJECT_H

#include "Renderer/RenderLightObject.h"

class RenderPointLightObject : public RenderLightObject
{
protected:
	float _lightRange;

public:
	RenderPointLightObject ();

	void SetLightRange (float lightRange);
protected:
	std::vector<PipelineAttribute> GetCustomAttributes () const;
};

#endif