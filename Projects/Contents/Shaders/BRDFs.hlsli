/* Source: http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html */
#include "Common.hlsli"

/* Normal Distribution Functions(NDFs) */
float BlinnPhongNDF(float3 normal, float3 halfway, float roughness)
{
	float a = roughness * roughness;
	float y = dot(normal, halfway);
	float x = 1.0 / (a * a);
	float power = (2.0 * x) - 2.0;
	
	return (x * (1.0 / PI)) * pow(y, power);
}

float BeckmannNDF(float3 normal, float3 halfway, float roughness)
{
	float a = roughness * roughness;
	float y = dot(normal, halfway);
	float yy = y * y;
	float z = 1.0 / yy;
	float x = 1.0 / (a * a);

	return (x * (z * z) * (1.0 / PI)) * exp((yy - 1) / ((a * a) * yy));

}

float TrowbridgeReitzNDF(float3 normal, float3 halfway, float roughness)
{
	float a = roughness * roughness;
	float aa = a * a;
	float y = max(dot(normal, halfway), 0.0);
	float yy = y * y;
	float k = (yy * (aa - 1.0) + 1.0);

	float nom = aa;
	float denom = (y * (aa - 1.0) + 1.0);
	denom = (PI * (k * k));
	
	return nom / denom;
}

/* Geometric Shadowing Terms */
float ImplicitGS(float3 light, float3 view, float3 normal)
{
	return (dot(normal, light) * dot(normal, view));
}

float NeumannGS(float3 light, float3 view, float3 normal)
{
	float nDotl = dot(normal, light);
	float nDotv = dot(normal, view);
	
	float nom = (nDotl * nDotv);
	float denom = max(nDotl, nDotv);

	return nom / denom;
}

float CookTorranceGS(float3 light, float3 view, float3 normal, float3 halfway)
{
	float vDoth = dot(view, halfway);
	float nDoth = dot(normal, halfway);
	float nDotv = dot(normal, view);
	float nDotl = dot(normal, light);
	float x = (2.0 * nDoth * nDotv) / vDoth;
	float y = (2.0 * nDoth * nDotl) / vDoth;

	return min(1.0, min(x, y));
}

float KelemenGS(float3 light, float3 view, float3 normal, float3 halfway)
{
	float vDoth = dot(view, halfway);
	
	float nom = (dot(normal, light) * dot(normal, view));
	float denom = (vDoth * vDoth);
	
	return nom / denom;
}

float BeckMannG1(float3 normal, float3 view, float roughness)
{
	float nDotv = dot(normal, view);
	float a = roughness * roughness;
	float c = (nDotv) / (a * sqrt(1.0 - (nDotv * nDotv)));

	if (c < 1.6)
	{
		float cc = c * c;
		
		float nom = ((3.535 * c) + (2.181 * cc));
		float denom = (1 + (2.276 * c) + (2.577 * cc));
		return nom / denom;
	}
	
	return 1.0;
}

float SchlickBeckmannG1(float3 normal, float3 view, float roughness)
{
	float a = roughness * roughness;
	float k = a * sqrt(2.0 / PI);
	float nDotv = dot(normal, view);
	
	float nom = nDotv;
	float denom = (nDotv * (1.0 - k)) + k;
	
	return nom / denom;
}

/* Setup remapping factor as 2.0 for IBL or 8.0 for Direct lighting. */
float SchlickG1(float3 normal, float3 view, float roughness, float remappingFactor)
{
	float r = roughness + 1.0;
	float a = r * r;
	float k = a / remappingFactor;
	float nDotv = max(dot(normal, view), 0.0);
	
	float nom = nDotv;
	float denom = (nDotv * (1.0 - k)) + k;
	
	return nom / denom;
}

/* Fresnel Terms */
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + ((1.0 - F0) * pow(1.0 - cosTheta, 5.0));
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
