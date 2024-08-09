#include "Shader_Defines.hlsli"

matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;

Texture2D g_DiffuseTexture; //: register(t0);
Texture2D g_MaskTexture; //: register(t1);
Texture2D g_MaskTextures[18]; //: register(t2);
Texture2D g_NoiseTexture; //: register(t3);
Texture2D g_NormalTexture; //: register(t4);
Texture2D g_DissolveTexture; //: register(t5);
Texture2D g_DepthTexture; //: register(t6);

int g_iColorBlendType;
float4 g_vColor;

float2 g_vDiffuseUV;
float2 g_vMaskUV;
float2 g_vNoiseUV;

float3 g_vWeight;

bool g_bMaskInverse;

bool g_bDiffuseClamp;
bool g_bMaskClamp;
bool g_bNoiseClamp;

bool g_bSprite;
int g_iSpriteType;
float2 g_vSpriteColRow;

float3 g_vDistortionValue;
float g_fDistortionScale;
float g_fDistortionBias;
float g_fDistortionWeight;;

float g_fDissolveAmount;
float g_fDissolveGradiationDistance;
float3 g_vDissolveGradiationStartColor;
float3 g_vDissolveGradiationEndColor;

float g_fAlpha;
float g_fTimeDelta;
bool g_bSoft;
bool g_bSolidColor;

//-------------

float			g_fUVStart;
bool            g_bMoveUV_Y;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = vector(In.vPosition, 1.f);
    vector vNormal = vector(In.vNormal, 0.f);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)); //X축 (World적용) 
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f)); //Y축 (World상의 x,z축으로 y축 구함)

    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;

};

struct PS_OUT_EFFECT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
    float4 vRadialBlur : SV_TARGET2;
    float4 vSolidColor : SV_TARGET3;
};

float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor, int iColorMode)
{
    float4 vResault = vDiffuse;
    
    [branch]
    if (0 == iColorMode)
    {
      // 곱하기
        vResault = vResault * vBlendColor;
        vResault.a = vBlendColor.a;

    }
    else if (1 == iColorMode)
    {
      // 스크린
        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
        vResault.a = vBlendColor.a;
    }
    else if (2 == iColorMode)
    {
      // 오버레이
        vResault = max(vResault, vBlendColor);
        vResault.a = vBlendColor.a;
    }
    else if (3 == iColorMode)
    {
      // 더하기
        vResault = vResault + vBlendColor;
        vResault.a = vBlendColor.a;
    }
    else if (4 == iColorMode)
    {
      // 번(Burn)
        vResault = vResault + vBlendColor - 1.f;
        vResault.a = vBlendColor.a;
    }
    else if (5 == iColorMode)
    {
        // 생생한 라이트
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] < 0.5f) ? (1.f - (1.f - vDiffuse[i]) / (2.f * vBlendColor[i]))
            : (vDiffuse[i] / (2.f * (1.f - vBlendColor[i])));

        }
        
        vResault.a = vBlendColor.a;
    }
    else if (6 == iColorMode)
    {
        // 소프트 라이트
        for (int i = 0; i < 3; ++i)
        {
            if (vBlendColor[i] < 0.5f)
            {
                vResault[i] = 2.f * vDiffuse[i] * vBlendColor[i] +
                    vDiffuse[i] * vDiffuse[i] * (1.f - 2.f * vBlendColor[i]);
            }
            else
            {
                vResault[i] = 2.f * vDiffuse[i] * (1.f - vBlendColor[i]) +
                    sqrt(vDiffuse[i]) * (2.f * vBlendColor[i] - 1.f);
            }
        }
        
        vResault.a = vBlendColor.a;
    }
    else if (7 == iColorMode)
    {
        // 하드 라이트
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] < 0.5f) ? (2.f * vDiffuse[i] * vBlendColor[i]) :
                (1.f - 2.f * (1.f - vDiffuse[i]) * (1.f - vBlendColor[i]));
        }
        
        vResault.a = vBlendColor.a;
    }
    else if (8 == iColorMode)
    {
        // 컬러 닷지
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
                min(vDiffuse[i] / (1.f - vBlendColor[i]), 1.f);

        }
        vResault.a = vBlendColor.a;
    }
    else if (9 == iColorMode)
    {
        // 혼합 번
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
                max(1.f - ((1.f - vDiffuse[i]) / vBlendColor[i]), 0.f);

        }
        vResault.a = vBlendColor.a;
    }
   
 
    return vResault;
};

float4 Clamp_Judgment(Texture2D tex, float2 texcoord, bool bJudgment)
{
    return lerp(tex.Sample(LinearSampler, texcoord), tex.Sample(ClampSampler, texcoord), bJudgment);
};

float2 Get_DistortionUV(float2 vUV)
{
    float2 vTmpCoord = vUV;
    vTmpCoord.y += g_fTimeDelta * g_fDistortionWeight;
	
    float4 vNoise1, vNoise2, vNoise3;

    vNoise1 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.x + g_vNoiseUV, g_bNoiseClamp);
    vNoise2 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.y + g_vNoiseUV, g_bNoiseClamp);
    vNoise3 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.z + g_vNoiseUV, g_bNoiseClamp);
		
    vNoise1 = (vNoise1 - 0.5f) * 2.f;
    vNoise2 = (vNoise2 - 0.5f) * 2.f;
    vNoise3 = (vNoise3 - 0.5f) * 2.f;
	
    float4 vResultNoise = vNoise1 + vNoise2 + vNoise3;
	
    float perturb = ((1.f - vUV.xy) * g_fDistortionScale) + g_fDistortionBias;
	
    return (vResultNoise.xy * perturb) + vUV;
}

PS_OUT_EFFECT PS_MAIN(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.x + g_vDiffuseUV);
    vector vMtrlMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.y + g_vMaskUV);
    vector vMtrlNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.z + g_vNoiseUV);

    Out.vDiffuse = Calculation_ColorBlend(vMtrlDiffuse, g_vColor, g_iColorBlendType);
    Out.vDiffuse.rgb *= vMtrlNoise.rgb;
	
    Out.vDiffuse.a *= lerp(vMtrlMask.x, 1.f - vMtrlMask.x, (float) g_bMaskInverse);
	
    if (Out.vDiffuse.a <= g_fAlpha)
        discard;
	
    Out.vDistortion = 0.f;

	return Out;
}

PS_OUT_EFFECT PS_MAIN_CLAMP(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

	if(!g_bMoveUV_Y)
		In.vTexcoord.x = (g_fUVStart - 0.4f) - g_fTimeDelta + In.vTexcoord.x * 0.4f;
	else
		In.vTexcoord.y = (g_fUVStart - 0.4f) - g_fTimeDelta + In.vTexcoord.y * 0.4f;

    vector vMtrlDiffuse, vMtrlMask, vMtrlNoise;

    vMtrlDiffuse = Clamp_Judgment(g_DiffuseTexture, In.vTexcoord * g_vWeight.x + g_vDiffuseUV, g_bDiffuseClamp);
    vMtrlMask = Clamp_Judgment(g_MaskTexture, In.vTexcoord * g_vWeight.y + g_vMaskUV, g_bMaskClamp);
    vMtrlNoise = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.z + g_vNoiseUV, g_bNoiseClamp);
    
    Out.vDiffuse = Calculation_ColorBlend(vMtrlDiffuse, g_vColor,g_iColorBlendType);
    Out.vDiffuse.rgb *= vMtrlNoise.rgb;
	
    if (g_bMaskInverse)
        Out.vDiffuse.a *= 1.f - vMtrlMask.x;
    else
        Out.vDiffuse.a *= vMtrlMask.x;
	
    if (Out.vDiffuse.a <= g_fAlpha)
		discard;

    Out.vDistortion = 0.f;
	
	return Out;
}

PS_OUT_EFFECT PS_MAIN_DISTORTION(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT Out = (PS_OUT_EFFECT)0;

    float4 vMask,vNoise;
	
    vMask = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.y + g_vMaskUV, g_bMaskClamp);
    vNoise = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.z + g_vNoiseUV, g_bNoiseClamp);
    
    Out.vDistortion = Calculation_ColorBlend(vNoise, g_vColor, g_iColorBlendType);
    Out.vDistortion.a = lerp(vMask.r, 1.f - vMask.r, (float) g_bMaskInverse);
	
    if (Out.vDistortion.a <= g_fAlpha)
        discard;
	
    Out.vDistortion.a = g_vColor.a; // 알파 값을 이용해서 디스토션 값 조절.
    
    Out.vDiffuse = 0.f;
	
	return Out;
}

PS_OUT_EFFECT PS_MAIN_DISSOLVE(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT)0;

    vector vMtrlDiffuse, vMtrlMask, vMtrlNoise;
    
    vMtrlDiffuse = Clamp_Judgment(g_DiffuseTexture, In.vTexcoord * g_vWeight.x + g_vDiffuseUV, g_bDiffuseClamp);
    vMtrlMask = Clamp_Judgment(g_MaskTexture, In.vTexcoord * g_vWeight.y + g_vMaskUV, g_bMaskClamp);
    vMtrlNoise = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.z + g_vNoiseUV, g_bNoiseClamp);
    
    vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);

    float fDissolveDesc = vMtrlDissolve.r;

    clip(fDissolveDesc - g_fDissolveAmount);
	
    if (g_fDissolveAmount + g_fDissolveGradiationDistance >= fDissolveDesc)
    {
        float fLerpRatio = (fDissolveDesc - g_fDissolveAmount) / g_fDissolveGradiationDistance;
        Out.vDiffuse = vector(lerp(g_vDissolveGradiationStartColor, g_vDissolveGradiationEndColor, fLerpRatio), 1.f);

    }
    else
    {
        Out.vDiffuse = vMtrlDiffuse;
    }
    
    Out.vDiffuse = Calculation_ColorBlend(Out.vDiffuse, g_vColor, g_iColorBlendType);
    Out.vDiffuse.rgb *= vMtrlNoise.rgb;
	
    if (g_bMaskInverse)
        Out.vDiffuse.a *= 1.f - vMtrlMask.x;
    else
        Out.vDiffuse.a *= vMtrlMask.x;
	
    if (Out.vDiffuse.a <= g_fAlpha)
        discard;
	
    Out.vDistortion = 0.f;
    
    return Out;
}

PS_OUT_EFFECT PS_MAIN_RADIALBLUR(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    vector vMtrlDiffuse, vMtrlMask, vMtrlNoise;
    
    vMtrlDiffuse = Clamp_Judgment(g_DiffuseTexture, In.vTexcoord * g_vWeight.x + g_vDiffuseUV, g_bDiffuseClamp);
    vMtrlMask = Clamp_Judgment(g_MaskTexture, In.vTexcoord * g_vWeight.y + g_vMaskUV, g_bMaskClamp);
    vMtrlNoise = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.z + g_vNoiseUV, g_bNoiseClamp);
    
    Out.vRadialBlur = Calculation_ColorBlend(vMtrlDiffuse, g_vColor, g_iColorBlendType);
    Out.vRadialBlur.rgb *= vMtrlNoise.rgb;
	
    if (g_bMaskInverse)
        Out.vRadialBlur.a *= 1.f - vMtrlMask.x;
    else
        Out.vRadialBlur.a *= vMtrlMask.x;
	
    if (Out.vRadialBlur.a <= g_fAlpha)
        discard;
	
    Out.vDiffuse = 0.f;
    Out.vDistortion = 0.f;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Effect_Clamp // 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CLAMP();
	}

	pass Effect_Distortion // 2
	{
	SetRasterizerState(RS_Cull_None);
	SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
    SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

	VertexShader = compile vs_5_0 VS_MAIN();
	GeometryShader = NULL;
	HullShader = NULL;
	DomainShader = NULL;
	PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

    pass Effect_Dissolve // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass Effect_RadialBlur // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }
}
