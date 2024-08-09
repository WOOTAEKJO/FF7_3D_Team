#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
};


Texture2D g_DiffuseTexture;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;
Texture2D g_DepthTexture;
float4 g_vColor;

float2 g_vDiffuseUV;
float2 g_vMaskUV;
float2 g_vNoiseUV;

float g_fAlpha;
float g_fTimeDelta;

float3 g_vDistortionValue;
float g_fDistortionScale;
float g_fDistortionBias;


bool g_bSprite;
int g_iSpriteType;
float2 g_vSpriteColRow;

Texture2D g_Texture;
float g_fNoiseWeight;
float g_fRatioY;
float2			g_vSpriteIndex;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
    float4      vDistortion : SV_TARGET1;
};

float2 GetSpriteUV(float2 spriteIndx)
{
    float2 SpriteSize = 1.f / g_vSpriteColRow;
    return spriteIndx * SpriteSize;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vTexcoord.x >= g_fRatioY)
		discard;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct VS_OUT_EFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT_EFFECT		Out = (VS_OUT_EFFECT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_EFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;

    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);
	
    if (g_bSprite)
    {
        float2 vSpriteUV = GetSpriteUV(g_vSpriteIndex);
		
        if (g_iSpriteType == 0) // Diffuse
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow)
            + g_vDiffuseUV);
            vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord + g_vMaskUV);
        }
        else if (g_iSpriteType == 1) // Mask
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
            vMask = g_MaskTexture.Sample(LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow)
            + g_vMaskUV);
        }
		
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
        vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord + g_vMaskUV);
    }
	
    float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
		
    Out.vColor = vDiffuse;
	
    Out.vColor *= g_vColor;
    Out.vColor.rgb *= vNoise.rgb;
    Out.vColor.a *= vMask.r;

    if (Out.vColor.a <= g_fAlpha)
        discard;
    
    Out.vDistortion = 0.f;
	
    return Out;
}

PS_OUT PS_MAIN_EFFECT_DEPTH(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
    float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord + g_vMaskUV);
    float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
		
    Out.vColor = vDiffuse;
	
    Out.vColor *= g_vColor;
    Out.vColor.rgb *= vNoise.rgb;
    Out.vColor.a *= vMask.r;
	
    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);

    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;

    if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
        discard;
    if (Out.vColor.a <= g_fAlpha)
        discard;

    Out.vDistortion = 0.f;
    
    return Out;
}

PS_OUT PS_MAIN_TRAIL(PS_IN_EFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
	

	Out.vColor.a = vMtrlDiffuse.r;
	Out.vColor.a *= g_vColor.a;
	Out.vColor.rgb = g_vColor.rgb + (vMtrlDiffuse.rgb);

	float2	vDepthTexcoord;
	vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4	vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);

	Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;

	if (Out.vColor.a <= 0.f)
		discard;

    Out.vDistortion = 0.f;
    
	return Out;
}

PS_OUT PS_MAIN_EFFECT_FIRE(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTmpCoord = In.vTexcoord;
    vTmpCoord.y += g_fTimeDelta * g_fNoiseWeight;
	
    float4 vNoise1 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.x + g_vNoiseUV);
    float4 vNoise2 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.y + g_vNoiseUV);
    float4 vNoise3 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.z + g_vNoiseUV);
		
    vNoise1 = (vNoise1 - 0.5f) * 2.f;
    vNoise2 = (vNoise2 - 0.5f) * 2.f;
    vNoise3 = (vNoise3 - 0.5f) * 2.f;
	
    float4 vResultNoise = vNoise1 + vNoise2 + vNoise3;
	
    float perturb = (((1.f - In.vTexcoord) * g_fDistortionScale) + g_fDistortionBias).x;
	
    float2 NoiseCoord = (vResultNoise.xy * perturb) + In.vTexcoord;
	
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);
	
    if (g_bSprite)
    {
        float2 vSpriteUV = GetSpriteUV(g_vSpriteIndex);
		
        if (g_iSpriteType == 0) // Diffuse
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, (vSpriteUV + NoiseCoord / g_vSpriteColRow)
            + g_vDiffuseUV);
            vMask = g_MaskTexture.Sample(LinearSampler, NoiseCoord + g_vMaskUV);
        }
        else if (g_iSpriteType == 1) // Mask
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, NoiseCoord + g_vDiffuseUV);
            vMask = g_MaskTexture.Sample(LinearSampler, (vSpriteUV + NoiseCoord / g_vSpriteColRow)
            + g_vMaskUV);
        }
		
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, NoiseCoord + g_vDiffuseUV);
        vMask = g_MaskTexture.Sample(LinearSampler, NoiseCoord + g_vMaskUV);
    }
	
    Out.vColor = vDiffuse;
	
    Out.vColor.a = vMask.r;
	
    if (Out.vColor.a <= g_fAlpha)
        discard;

    Out.vDistortion = 0.f;
    
    return Out;
}


technique11 DefaultTechnique
{

	pass Effect //0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass VIBufferTrail //1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRAIL();
	}

    pass Effect_Depth //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_DEPTH();
    }

    pass Effect_Fire //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_FIRE();
    }

}
