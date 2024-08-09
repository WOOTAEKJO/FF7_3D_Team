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

//-----------------
vector  g_vCamLook;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major float4x4	TransformMatrix : WORLD;
	float4		vColor : COLOR0;
    float4		vDir : TEXCOORD0;
    float2		vTextureUV : TEXCOORD1;
    uint		iMaskIndex : TEXCOORD2;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;

	float4		vColor : COLOR0;
	
	float3		vRight : TEXCOORD0;
	float3		vUp	   : TEXCOORD1;
	
    float4		vDir : TEXCOORD2;
    float2		vTextureUV : TEXCOORD3;
    uint		iMaskIndex : TEXCOORD4;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);

	float3 vRight = float3(In.TransformMatrix._11, In.TransformMatrix._12, In.TransformMatrix._13);
	float3 vUp = float3(In.TransformMatrix._21, In.TransformMatrix._22, In.TransformMatrix._23);

	Out.vPSize = float2(In.vPSize.x * length(vRight), In.vPSize.y * length(vUp));
    Out.vRight = normalize(mul(vRight, (float3x3) g_WorldMatrix));
    Out.vUp = normalize(mul(vUp, (float3x3) g_WorldMatrix));

	Out.vColor = In.vColor;
    //Out.vDir = In.vDir;
    Out.vDir = mul(In.vDir, g_WorldMatrix);
    Out.vTextureUV = In.vTextureUV;
    Out.iMaskIndex = In.iMaskIndex;
	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;

	float4		vColor : COLOR0;

	float3		vRight : TEXCOORD0;
	float3		vUp	   : TEXCOORD1;
	
    float4		vDir : TEXCOORD2;
    float2		vTextureUV : TEXCOORD3;
    uint		iMaskIndex : TEXCOORD4;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vColor : COLOR0;
    float2		vTextureUV : TEXCOORD1;
    uint		iMaskIndex : TEXCOORD2;
    
    float4 vProjPos : TEXCOORD3;
};

/* 지오메트리 쉐이더 : 셰이더안에서 정점을 추가적으로 생성해 준다. */

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	float4		vLook = normalize(-g_vCamLook);
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;
    Out[0].vTextureUV = In[0].vTextureUV;
    Out[0].iMaskIndex = In[0].iMaskIndex;
    Out[0].vProjPos = Out[0].vPosition;
	
	Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;
    Out[1].vTextureUV = In[0].vTextureUV;
    Out[1].iMaskIndex = In[0].iMaskIndex;
    Out[1].vProjPos = Out[1].vPosition;
	
	Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;
    Out[2].vTextureUV = In[0].vTextureUV;
    Out[2].iMaskIndex = In[0].iMaskIndex;
    Out[2].vProjPos = Out[2].vPosition;
	
	Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;
    Out[3].vTextureUV = In[0].vTextureUV;
    Out[3].iMaskIndex = In[0].iMaskIndex;
    Out[3].vProjPos = Out[3].vPosition;

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_NONBILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	float3		vRight = normalize(In[0].vRight) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(In[0].vUp) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;
    Out[0].vTextureUV = In[0].vTextureUV;
    Out[0].iMaskIndex = In[0].iMaskIndex;
    Out[0].vProjPos = Out[0].vPosition;
	
	Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;
    Out[1].vTextureUV = In[0].vTextureUV;
    Out[1].iMaskIndex = In[0].iMaskIndex;
    Out[1].vProjPos = Out[1].vPosition;
	
	Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;
    Out[2].vTextureUV = In[0].vTextureUV;
    Out[2].iMaskIndex = In[0].iMaskIndex;
    Out[2].vProjPos = Out[2].vPosition;
	
	Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;
    Out[3].vTextureUV = In[0].vTextureUV;
    Out[3].iMaskIndex = In[0].iMaskIndex;
    Out[3].vProjPos = Out[3].vPosition;
	
	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}


[maxvertexcount(6)]
void GS_MAIN_DIRECTION(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    float4 vLook = normalize(-g_vCamLook);
    float3 vRight = normalize(cross(In[0].vDir.xyz, vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;
    Out[0].vTextureUV = In[0].vTextureUV;
    Out[0].iMaskIndex = In[0].iMaskIndex;
    Out[0].vProjPos = Out[0].vPosition;

    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;
    Out[1].vTextureUV = In[0].vTextureUV;
    Out[1].iMaskIndex = In[0].iMaskIndex;
    Out[1].vProjPos = Out[1].vPosition;
				
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;
    Out[2].vTextureUV = In[0].vTextureUV;
    Out[2].iMaskIndex = In[0].iMaskIndex;
    Out[2].vProjPos = Out[2].vPosition;

    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;
    Out[3].vTextureUV = In[0].vTextureUV;
    Out[3].iMaskIndex = In[0].iMaskIndex;
    Out[3].vProjPos = Out[3].vPosition;

    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vColor : COLOR0;
    float2		vTextureUV : TEXCOORD1;
    uint		iMaskIndex : TEXCOORD2;
    
    float4      vProjPos : TEXCOORD3;
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
    else if (10 == iColorMode)
    {
      // 좀 더 강한 오버레이
        vResault = max(2 * vResault * vBlendColor, 0.f);
        vResault.a = vBlendColor.a;
    }
   
 
    return vResault;
};

float4 Clamp_Judgment(Texture2D tex, float2 texcoord, bool bJudgment)
{
    return lerp(tex.Sample(LinearSampler, texcoord), tex.Sample(ClampSampler, texcoord), bJudgment);
};

float2 GetSpriteUV(float2 spriteIndx)
{
    float2 SpriteSize = 1.f / g_vSpriteColRow;
    return spriteIndx * SpriteSize;
};

float Soft(float4 vDiffuse, float4 vProjPos)
{
    if (!g_bSoft)
        return vDiffuse.a;
   
    float2 vTexUV;
    
    vTexUV.x = vProjPos.x / vProjPos.w;
    vTexUV.y = vProjPos.y / vProjPos.w;
    
    vTexUV.x = vTexUV.x * 0.5f + 0.5f;
    vTexUV.y = vTexUV.y * -0.5f + 0.5f;
    
    float4 Depth = g_DepthTexture.Sample(LinearSampler, vTexUV);
    float fViewZ = Depth.y * 1000.f;
    
    return vDiffuse.a * saturate(fViewZ - vProjPos.w);
};

float4 GetMaskPixel(uint iMaskIndex, sampler sampl, float2 texcoord)
{
    // 텍스쳐 배열에 변수가 들어가면 안됨 무조건 리터럴 상수만 가능하다.
    //vMask = g_MaskTextures[iMaskIndex].Sample(LinearSampler, In.vTexcoord + g_vMaskUV); 이런거 안됨
    // 하드 코딩해야 함
    switch (iMaskIndex)
    {
        case 0:
            return g_MaskTextures[0].Sample(sampl, texcoord);
        case 1:
            return g_MaskTextures[1].Sample(sampl, texcoord);
        case 2:
            return g_MaskTextures[2].Sample(sampl, texcoord);
        case 3:
            return g_MaskTextures[3].Sample(sampl, texcoord);
        case 4:
            return g_MaskTextures[4].Sample(sampl, texcoord);
        case 5:
            return g_MaskTextures[5].Sample(sampl, texcoord);
        case 6:
            return g_MaskTextures[6].Sample(sampl, texcoord);
        case 7:
            return g_MaskTextures[7].Sample(sampl, texcoord);
        case 8:
            return g_MaskTextures[8].Sample(sampl, texcoord);
        case 9:
            return g_MaskTextures[9].Sample(sampl, texcoord);
        case 10:
            return g_MaskTextures[10].Sample(sampl, texcoord);
        case 11:
            return g_MaskTextures[11].Sample(sampl, texcoord);
        case 12:
            return g_MaskTextures[12].Sample(sampl, texcoord);
        case 13:
            return g_MaskTextures[13].Sample(sampl, texcoord);
        case 14:
            return g_MaskTextures[14].Sample(sampl, texcoord);
        case 15:
            return g_MaskTextures[15].Sample(sampl, texcoord);
        case 16:
            return g_MaskTextures[16].Sample(sampl, texcoord);
        case 17:
            return g_MaskTextures[17].Sample(sampl, texcoord);
        default:
            return g_MaskTextures[0].Sample(sampl, texcoord);
		
    };
};


/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT_EFFECT PS_MAIN(PS_IN In)
{
	PS_OUT_EFFECT		Out = (PS_OUT_EFFECT)0;

    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);
	
    if (g_bSprite)
    {
        float2 vSpriteUV = GetSpriteUV(In.vTextureUV);
		
        if (g_iSpriteType == 0) // Diffuse
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow)
            + g_vDiffuseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, In.vTexcoord + g_vMaskUV);

        }
        else if (g_iSpriteType == 1) // Mask
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow) + g_vMaskUV);
        }
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vDiffuseUV);
        vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, In.vTexcoord + g_vMaskUV);
    }
	
    float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
		
    float4 vResult;
    vResult = Calculation_ColorBlend(vDiffuse, In.vColor, g_iColorBlendType);
    vResult.rgb *= vNoise.rgb;
    vResult.a *= vMask.r;

    vResult.a = Soft(vResult, In.vProjPos);
       
    if (vResult.a <= g_fAlpha)
        discard;
    
    if (g_bSolidColor)
        Out.vSolidColor = vResult;
    else
        Out.vDiffuse = vResult;
  
        return Out;
}

PS_OUT_EFFECT PS_MAIN_EFFECT_DISTORTION(PS_IN In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);
	
    
    if (g_bSprite)
    {
        float2 vSpriteUV = GetSpriteUV(In.vTextureUV);
		
        if (g_iSpriteType == 0) // Noise
        {
            Out.vDistortion = g_NoiseTexture.Sample(LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow) + g_vNoiseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, In.vTexcoord + g_vMaskUV);
        }
        else if (g_iSpriteType == 1) // Mask
        {
            Out.vDistortion = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, (vSpriteUV + In.vTexcoord / g_vSpriteColRow) + g_vMaskUV);
        }
    }
    else
    {
        Out.vDistortion = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vNoiseUV);
        vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, In.vTexcoord + g_vMaskUV);
    }
        
    Out.vDistortion *= In.vColor;
	
    Out.vDistortion.a = vMask.r;

    if (Out.vDistortion.a <= g_fAlpha)
        discard;
	
    Out.vDistortion.a = g_vColor.a; // 알파 값을 이용해서 디스토션 값 조절.
    
    Out.vDiffuse = 0.f;
    
    return Out;
}

PS_OUT_EFFECT PS_MAIN_EFFECT_FIRE(PS_IN In)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    float2 vTmpCoord = In.vTexcoord;
    vTmpCoord.y += g_fTimeDelta * g_fDistortionWeight;
	
    float4 vNoise1 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.x + g_vNoiseUV);
    float4 vNoise2 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.y + g_vNoiseUV);
    float4 vNoise3 = g_NoiseTexture.Sample(LinearSampler, vTmpCoord * g_vDistortionValue.z + g_vNoiseUV);
		
    vNoise1 = (vNoise1 - 0.5f) * 2.f;
    vNoise2 = (vNoise2 - 0.5f) * 2.f;
    vNoise3 = (vNoise3 - 0.5f) * 2.f;
	
    float4 vResultNoise = vNoise1 + vNoise2 + vNoise3;
	
    float perturb = (((1.f - In.vTexcoord.xy) * g_fDistortionScale) + g_fDistortionBias).x;
	
    float2 NoiseCoord = (vResultNoise.xy * perturb) + In.vTexcoord;
	
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);;
    float4 vMask = float4(0.f, 0.f, 0.f, 0.f);;
	
    if (g_bSprite)
    {
        float2 vSpriteUV = GetSpriteUV(In.vTextureUV);
		
        if (g_iSpriteType == 0) // Diffuse
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, (vSpriteUV + NoiseCoord / g_vSpriteColRow)
            + g_vDiffuseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, NoiseCoord + g_vMaskUV);
        }
        else if (g_iSpriteType == 1) // Mask
        {
            vDiffuse = g_DiffuseTexture.Sample(LinearSampler, NoiseCoord + g_vDiffuseUV);
            vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, (vSpriteUV + NoiseCoord / g_vSpriteColRow) + g_vMaskUV);
        }
		
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, NoiseCoord + g_vDiffuseUV);
        vMask = GetMaskPixel(In.iMaskIndex, LinearSampler, NoiseCoord + g_vMaskUV);
    }
    
    Out.vDiffuse = Calculation_ColorBlend(vDiffuse, In.vColor, g_iColorBlendType);
    Out.vDiffuse.a *= vMask.r;

    Out.vDiffuse.a = Soft(Out.vDiffuse, In.vProjPos);
    
    if (Out.vDiffuse.a <= g_fAlpha)
        discard;

    Out.vDistortion = 0.f;
    
    return Out;
}

technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle_BillBoard //0
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_NonBillBoard //1
	{
		SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_NONBILLBOARD();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Particle_BillBoard_Direction //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_DIRECTION();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	pass Particle_Fire //3
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_FIRE();
    }

    pass Particle_Fire_Direction //4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_DIRECTION();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_FIRE();
    }

    pass Particle_Distortion //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_DISTORTION();
    }

    pass Particle_Distortion_Direction //6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlendEffect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_DIRECTION();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_DISTORTION();
    }
}
