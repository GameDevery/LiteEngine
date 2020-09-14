uniform sampler3D voxelTexture[8];

uniform vec3 minVertex;
uniform vec3 maxVertex;
uniform ivec3 volumeSize;
uniform int volumeMipmapLevels;

uniform float originBias;

float GetInterpolatedComp (float comp, float minValue, float maxValue)
{
	return ((comp - minValue) / (maxValue - minValue));
}

vec3 GetPositionInVolume (vec3 origin)
{
	vec3 positionInVolume;

	positionInVolume.x = GetInterpolatedComp (origin.x, minVertex.x, maxVertex.x);
	positionInVolume.y = GetInterpolatedComp (origin.y, minVertex.y, maxVertex.y);
	positionInVolume.z = GetInterpolatedComp (origin.z, minVertex.z, maxVertex.z);

	return positionInVolume;// + vec3 (1.0 / volumeSize.x);
}

float minVoxelDiameter = 1.0 / volumeSize.x;
float minVoxelDiameterInv = volumeSize.x;

// origin, dir, and maxDist are in texture space
// dir should be normalized
// coneRatio is the cone diameter to height ratio (2.0 for 90-degree cone)
vec4 voxelTraceCone(vec3 origin, vec3 dir, float coneRatio, float maxDist)
{
	vec3 accum = vec3(0.0);
	float alpha = 0.0;

	// the starting sample diameter
	float minDiameter = minVoxelDiameter;

	// push out the starting point to avoid self-intersection
	float startDist = minDiameter * 3;
	
	float dist = startDist;
	while (dist <= maxDist && alpha < 1.0)
	{
		// ensure the sample diameter is no smaller than the min
		// desired diameter for this cone (ensuring we always
		// step at least minDiameter each iteration, even for tiny
		// cones - otherwise lots of overlapped samples)
		float sampleDiameter = max(minDiameter, coneRatio * dist);
		
		// convert diameter to LOD
		// for example:
		// log2(1/256 * 256) = 0
		// log2(1/128 * 256) = 1
		// log2(1/64 * 256) = 2
		float sampleLOD = log2(sampleDiameter * minVoxelDiameterInv);
		
		vec3 samplePos = origin + dir * dist;

		ivec3 face = ivec3 (
			dir.x < 0 ? 0 : 1,
			dir.y < 0 ? 2 : 3,
			dir.z < 0 ? 4 : 5
		);

		vec3 weight = dir * dir;

		/*
		 * Current mipmap level
		*/

		int mipLevel = min (int (sampleLOD), volumeMipmapLevels - 1);

		vec4 sampleValue1 = vec4 (0.0);

		if (mipLevel == 0) {
			sampleValue1 = texture (voxelTexture [mipLevel], samplePos);
		} else {
			sampleValue1 =
				weight.x * texture (voxelTexture [mipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.x, 0, 0)) +
				weight.y * texture (voxelTexture [mipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.y, 0, 0)) +
				weight.z * texture (voxelTexture [mipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.z, 0, 0));
		}

		/*
		 * Next mipmap level
		*/

		int nextMipLevel = min (mipLevel + 1, volumeMipmapLevels - 1);

		vec4 sampleValue2 =
			weight.x * texture (voxelTexture [nextMipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.x, 0, 0)) +
			weight.y * texture (voxelTexture [nextMipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.y, 0, 0)) +
			weight.z * texture (voxelTexture [nextMipLevel], samplePos / vec3 (6, 1, 1) + vec3 (1.0 / 6.0 * face.z, 0, 0));

		/*
		 * Mipmap filter
		*/

		vec4 sampleValue = mix (sampleValue1, sampleValue2, fract(sampleLOD));

		accum = accum + (1.0 - alpha) * sampleValue.a * sampleValue.rgb;
		alpha = alpha + (1.0 - alpha) * sampleValue.a;

		dist += sampleDiameter;
	}
	
	// decompress color range to decode limited HDR
	// accum *= 2.0;
	
	return vec4 (accum, alpha);
}

float voxelTraceConeOcclusion(vec3 origin, vec3 dir, float coneRatio, float maxDist)
{
	float occlusion = 0.0;
	float alpha = 0.0;

	// the starting sample diameter
	float minDiameter = minVoxelDiameter;

	// push out the starting point to avoid self-intersection
	float startDist = minDiameter;
	
	float dist = startDist;
	while (dist <= maxDist && alpha < 1.0)
	{
		float sampleDiameter = max(minDiameter, coneRatio * dist);
		
		float sampleLOD = log2(sampleDiameter * minVoxelDiameterInv);
		
		vec3 samplePos = origin + dir * originBias + dir * dist;

		int mipLevel = int (min (sampleLOD, volumeMipmapLevels - 1.0));

		samplePos.x /= mipLevel == 0 ? 1.0 : 6.0;

		vec4 sampleValue = texture (voxelTexture [mipLevel], samplePos);

		occlusion += ((1.0 - alpha) * sampleValue.a) / (1.0 + 0.03 * sampleDiameter);

		alpha = alpha + (1.0 - alpha) * sampleValue.a;

		dist += sampleDiameter;
	}
	
	return occlusion;
}
