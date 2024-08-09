#ifndef SHADER_EFFECT_DEFINES_HLSLI
#define SHADER_EFFECT_DEFINES_HLSLI
#include "Shader_Defines.hlsli"

// 상수버퍼 바인딩을 따로 만들어야 함.
// 일반적으로 바인딩했던 방식이 아닌 다른 방식으로 데이터를 바인딩시켜야 한다.

//cbuffer WVP //: register(b1)
//{
//    matrix g_WorldMatrix;
//    matrix g_ViewMatrix;
//    matrix g_ProjMatrix;
//};

//cbuffer Color
//{
//    int g_iColorBlendType;
//    float4 g_vColor;
//};

//cbuffer UV
//{
//    float2 g_vDiffuseUV;
//    float2 g_vMaskUV;
//    float2 g_vNoiseUV;

//    float3 g_vWeight;

//    bool g_bMaskInverse;

//    bool g_bDiffuseClamp;
//    bool g_bMaskClamp;
//    bool g_bNoiseClamp;
//};

//cbuffer Sprite
//{
//    bool g_bSprite;
//    int g_iSpriteType;
//    float2 g_vSpriteColRow;
//};

//cbuffer Distortion
//{
//    float3 g_vDistortionValue;
//    float g_fDistortionScale;
//    float g_fDistortionBias;
//    float g_fDistortionWeight;
//};

//cbuffer Dissolve
//{
//    float g_fDissolveAmount;
//    float g_fDissolveGradiationDistance;
//    float3 g_vDissolveGradiationStartColor;
//    float3 g_vDissolveGradiationEndColor;
//};

//cbuffer ETC
//{
//    float g_fAlpha;
//    float g_fTimeDelta;
//    bool g_bSoft;
//    bool g_bSolidColor;
    

//};

//Texture2D g_DiffuseTexture; //: register(t0);
//Texture2D g_MaskTexture; //: register(t1);
//Texture2D g_MaskTextures[18]; //: register(t2);
//Texture2D g_NoiseTexture; //: register(t3);
//Texture2D g_NormalTexture; //: register(t4);
//Texture2D g_DissolveTexture; //: register(t5);
//Texture2D g_DepthTexture; //: register(t6);

//struct PS_OUT_EFFECT
//{
//    float4 vDiffuse : SV_TARGET0;
//    float4 vDistortion : SV_TARGET1;
//    float4 vRadialBlur : SV_TARGET2;
//    float4 vSolidColor : SV_TARGET3;
//};

//float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor, int iColorMode)
//{
//    float4 vResault = vDiffuse;
   
//    if (0 == iColorMode)
//    {
//      // 곱하기
//        vResault = vResault * vBlendColor;
//        vResault.a = vBlendColor.a;

//    }
//    else if (1 == iColorMode)
//    {
//      // 스크린
//        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (2 == iColorMode)
//    {
//      // 오버레이
//        vResault = max(vResault, vBlendColor);
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (3 == iColorMode)
//    {
//      // 더하기
//        vResault = vResault + vBlendColor;
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (4 == iColorMode)
//    {
//      // 번(Burn)
//        vResault = vResault + vBlendColor - 1.f;
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (5 == iColorMode)
//    {
//        // 생생한 라이트
//        for (int i = 0; i < 3; ++i)
//        {
//            vResault[i] = (vBlendColor[i] < 0.5f) ? (1.f - (1.f - vDiffuse[i]) / (2.f * vBlendColor[i]))
//            : (vDiffuse[i] / (2.f * (1.f - vBlendColor[i])));

//        }
        
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (6 == iColorMode)
//    {
//        // 소프트 라이트
//        for (int i = 0; i < 3; ++i)
//        {
//            if (vBlendColor[i] < 0.5f)
//            {
//                vResault[i] = 2.f * vDiffuse[i] * vBlendColor[i] +
//                    vDiffuse[i] * vDiffuse[i] * (1.f - 2.f * vBlendColor[i]);
//            }
//            else
//            {
//                vResault[i] = 2.f * vDiffuse[i] * (1.f - vBlendColor[i]) +
//                    sqrt(vDiffuse[i]) * (2.f * vBlendColor[i] - 1.f);
//            }
//        }
        
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (7 == iColorMode)
//    {
//        // 하드 라이트
//        for (int i = 0; i < 3; ++i)
//        {
//            vResault[i] = (vBlendColor[i] < 0.5f) ? (2.f * vDiffuse[i] * vBlendColor[i]) :
//                (1.f - 2.f * (1.f - vDiffuse[i]) * (1.f - vBlendColor[i]));
//        }
        
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (8 == iColorMode)
//    {
//        // 컬러 닷지
//        for (int i = 0; i < 3; ++i)
//        {
//            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
//                min(vDiffuse[i] / (1.f - vBlendColor[i]), 1.f);

//        }
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
//    else if (9 == iColorMode)
//    {
//        // 혼합 번
//        for (int i = 0; i < 3; ++i)
//        {
//            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
//                max(1.f - ((1.f - vDiffuse[i]) / vBlendColor[i]), 0.f);

//        }
//        //vResault.a = vResault.a * vBlendColor.a;
//        vResault.a = vBlendColor.a;
//    }
   
 
//    return vResault;
//};

//float4 Clamp_Judgment(Texture2D tex, float2 texcoord, bool bJudgment)
//{
//    return lerp(tex.Sample(LinearSampler, texcoord), tex.Sample(ClampSampler, texcoord), bJudgment);
//};

//float2 Get_DistortionUV(float2 vUV)
//{
//    float2 vTmpCoord = vUV;
//    vTmpCoord.y += g_fTimeDelta * g_fDistortionWeight;
	
//    float4 vNoise1, vNoise2, vNoise3;

//    vNoise1 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.x + g_vNoiseUV, g_bNoiseClamp);
//    vNoise2 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.y + g_vNoiseUV, g_bNoiseClamp);
//    vNoise3 = Clamp_Judgment(g_NoiseTexture, vTmpCoord * g_vDistortionValue.z + g_vNoiseUV, g_bNoiseClamp);
		
//    vNoise1 = (vNoise1 - 0.5f) * 2.f;
//    vNoise2 = (vNoise2 - 0.5f) * 2.f;
//    vNoise3 = (vNoise3 - 0.5f) * 2.f;
	
//    float4 vResultNoise = vNoise1 + vNoise2 + vNoise3;
	
//    float perturb = ((1.f - vUV.xy) * g_fDistortionScale) + g_fDistortionBias;
	
//    return (vResultNoise.xy * perturb) + vUV;
//};

//float Soft(float4 vDiffuse, float4 vProjPos)
//{
//    if (!g_bSoft)
//        return vDiffuse.a;
   
//    float2 vTexUV;
    
//    vTexUV.x = vProjPos.x / vProjPos.w;
//    vTexUV.y = vProjPos.y / vProjPos.w;
    
//    vTexUV.x = vTexUV.x * 0.5f + 0.5f;
//    vTexUV.y = vTexUV.y * -0.5f + 0.5f;
    
//    float4 Depth = g_DepthTexture.Sample(LinearSampler, vTexUV);
//    float fViewZ = Depth.y * 1000.f;
    
//    return vDiffuse.a * saturate(fViewZ - vProjPos.w);
//};

//float2 GetSpriteUV(float2 spriteIndx)
//{
//    float2 SpriteSize = 1.f / g_vSpriteColRow;
//    return spriteIndx * SpriteSize;
//};

//float4 GetMaskPixel(uint iMaskIndex, sampler sampl, float2 texcoord)
//{
//    // 텍스쳐 배열에 변수가 들어가면 안됨 무조건 리터럴 상수만 가능하다.
//    //vMask = g_MaskTextures[iMaskIndex].Sample(LinearSampler, In.vTexcoord + g_vMaskUV); 이런거 안됨
//    // 하드 코딩해야 함
//    switch (iMaskIndex)
//    {
//        case 0:
//            return g_MaskTextures[0].Sample(sampl, texcoord);
//        case 1:
//            return g_MaskTextures[1].Sample(sampl, texcoord);
//        case 2:
//            return g_MaskTextures[2].Sample(sampl, texcoord);
//        case 3:
//            return g_MaskTextures[3].Sample(sampl, texcoord);
//        case 4:
//            return g_MaskTextures[4].Sample(sampl, texcoord);
//        case 5:
//            return g_MaskTextures[5].Sample(sampl, texcoord);
//        case 6:
//            return g_MaskTextures[6].Sample(sampl, texcoord);
//        case 7:
//            return g_MaskTextures[7].Sample(sampl, texcoord);
//        case 8:
//            return g_MaskTextures[8].Sample(sampl, texcoord);
//        case 9:
//            return g_MaskTextures[9].Sample(sampl, texcoord);
//        case 10:
//            return g_MaskTextures[10].Sample(sampl, texcoord);
//        case 11:
//            return g_MaskTextures[11].Sample(sampl, texcoord);
//        case 12:
//            return g_MaskTextures[12].Sample(sampl, texcoord);
//        case 13:
//            return g_MaskTextures[13].Sample(sampl, texcoord);
//        case 14:
//            return g_MaskTextures[14].Sample(sampl, texcoord);
//        case 15:
//            return g_MaskTextures[15].Sample(sampl, texcoord);
//        case 16:
//            return g_MaskTextures[16].Sample(sampl, texcoord);
//        case 17:
//            return g_MaskTextures[17].Sample(sampl, texcoord);
//        default:
//            return g_MaskTextures[0].Sample(sampl, texcoord);
		
//    };
//};

#endif