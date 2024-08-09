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

//--------------------
bool			g_bDiffuseUse;

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

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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

PS_OUT_EFFECT PS_MAIN(PS_IN In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = Calculation_ColorBlend(Out.vDiffuse, g_vColor, g_iColorBlendType);

    return Out;
}

PS_OUT_EFFECT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT			Out = (PS_OUT_EFFECT)0;

    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
    float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord + g_vMaskUV);	
    float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
	
    Out.vDiffuse = Calculation_ColorBlend(Out.vDiffuse, g_vColor, g_iColorBlendType);
	
    Out.vDiffuse.rgb *= vNoise.rgb;
    Out.vDiffuse.a *= vMask.r;

    if (Out.vDiffuse.a <= g_fAlpha)
        discard;
	
    Out.vDistortion = 0.f;
	
    return Out;
}

PS_OUT_EFFECT PS_MAIN_TRAIL(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT			Out = (PS_OUT_EFFECT)0;
	
    float4 vDiffuse, vMask, vNoise;

    vDiffuse = Clamp_Judgment(g_DiffuseTexture, In.vTexcoord * g_vWeight.x + g_vDiffuseUV, g_bDiffuseClamp);
    vMask = Clamp_Judgment(g_MaskTexture, In.vTexcoord * g_vWeight.y + g_vMaskUV, g_bMaskClamp);
    vNoise = Clamp_Judgment(g_NoiseTexture, In.vTexcoord * g_vWeight.z + g_vNoiseUV, g_bNoiseClamp);

    Out.vDiffuse = Calculation_ColorBlend(vDiffuse, g_vColor, g_iColorBlendType);
	
    Out.vDiffuse.rgb *= vNoise.rgb;
	
    if (g_bMaskInverse)
        Out.vDiffuse.a *= 1.f - vMask.r;
    else
        Out.vDiffuse.a *= vMask.r;

    if (Out.vDiffuse.a <= g_fAlpha)
		discard;

    Out.vDistortion = 0.f;
	
	return Out;
}

PS_OUT_EFFECT PS_MAIN_DISTORTION(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);

    Out.vDistortion = lerp(g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.z + g_vNoiseUV),
		g_NoiseTexture.Sample(ClampSampler, In.vTexcoord * g_vWeight.z + g_vNoiseUV),
		g_bNoiseClamp);
	
    vMask = lerp(g_MaskTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.y + g_vMaskUV),
				g_MaskTexture.Sample(ClampSampler, In.vTexcoord * g_vWeight.y + g_vMaskUV),
				g_bMaskClamp);
    
    if (g_bDiffuseUse)
    {
        float4 vDiffuse = lerp(g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vWeight.x + g_vDiffuseUV),
					g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord * g_vWeight.x + g_vDiffuseUV),
					g_bDiffuseClamp);
		
        Out.vDiffuse = Calculation_ColorBlend(Out.vDiffuse, g_vColor, g_iColorBlendType);
	
        if (g_bMaskInverse)
            Out.vDiffuse.a = 1.f - vMask.r;
        else
            Out.vDiffuse.a = vMask.r;
	
        if (Out.vDiffuse.a <= g_fAlpha)
            discard;
    }else
        Out.vDiffuse = 0.f;

    Out.vDistortion = Calculation_ColorBlend(Out.vDistortion,g_vColor,g_iColorBlendType);
	
    if (g_bMaskInverse)
        Out.vDistortion.a = 1.f - vMask.r;
    else
        Out.vDistortion.a = vMask.r;
	
    if (Out.vDistortion.a <= g_fAlpha)
        discard;

    Out.vDistortion.a = g_vColor.a; // 알파 값을 이용해서 디스토션 값 조절.
    
    return Out;
}

technique11 DefaultTechnique
{

	pass Effect //0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass VIBufferTrail //1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRAIL();
	}

    pass Effect_Distortion //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

}
