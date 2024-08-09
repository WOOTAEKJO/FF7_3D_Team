#pragma once

#include "Effect.h"

BEGIN(Engine)

END

BEGIN(Client)

class CTrail_Effect final : public CEffect
{
public:
	typedef struct tagTrailDesc : public CEffect::EFFECT_DESC
	{		
		wstring		strModelTag = {};

		_bool		bFollowOwner = {};
		_bool		bPlayAnimation = { false };
		_bool		bMaskInverse = { false };
		_bool		bMoveUV_Y = { false };
		_float3		vAdjustPosition = {};
		_float3		vAdjustRotation = {-90.f,0.f,0.f};

		_bool		bClamp = {};
		_float		fUVStart = {1.f};
		_float		fUVSpeed = {1.f};
		_float		fUVForce = {0.f};

		_float3		vMove_Speed = {0.f,0.f,0.f};
		_float3		vMove_Force = { 0.f,0.f,0.f };
		_float3		vMove_MinSpeed = { -100.f,-100.f,-100.f };
		_float3		vMove_MaxSpeed = { 100.f,100.f,100.f };

		_float3		vScale = {1.f,1.f,1.f } ;
		_float3		vScale_Speed = { 0.f,0.f,0.f };
		_float3		vScale_Force = { 0.f,0.f,0.f };
		_float3		vScale_Min = { 0.f,0.f,0.f };
		_float3		vScale_Max = { 1000.f,1000.f,1000.f };

		_float3		vRotation = {0.f,0.f,0.f};
		_float3		vRotation_Speed = { 0.f,0.f,0.f };
		_float3		vRotation_Force = { 0.f,0.f,0.f };
		_float3		vRotation_Min = { -100.f,-100.f,-100.f };
		_float3		vRotation_Max = { 100.f,100.f,100.f };

		_float4		vColor = { 1.f,1.f,1.f,1.f };
		_float4		vColor_Speed = { 0.f,0.f,0.f,0.f };
		_float4		vColor_Force = { 0.f,0.f,0.f,0.f };
		_float4		vColor_Max = { 1.f,1.f,1.f,1.f };

		_float3		vWeight = { 1.f,1.f,1.f };

		// ------ Dissolve -------
		wstring			strDissolve;

		_float			fDissolveAmountSpeed = { 1.f };
		_float			fDissolveGradiationDistanceSpeed = { 1.f };	// 각각 스피드

		_float3			vDissolveGradiationStartColor = _float3(1.f,1.f,1.f);
		_float3			vDissolveGradiationEndColor= _float3(1.f, 1.f, 1.f);

		// ------ TIme --------
		_float			fLifeTime = { 10.f };

		// ------ DissolveParticle -------
		_bool			bDissolveParticle = { false };
		wstring			strDissolveParticleTag;
		CVIBuffer_Instancing::MESH_START_TYPE	eDissolveParticleStartType
			= { CVIBuffer_Instancing::MESH_START_TYPE::RANDOM_POS };

		// ----- Clam ------
		_bool		bDiffuseClamp = { false };
		_bool		bMaskClamp = { false };
		_bool		bNoiseClamp = { false };

		// ------ Rotation --------
		_bool		bRotation = { false };
		_bool		bCurAxisUse = { false }; // 현재 회전한 값으로 축을 만들어서 사용.
		_float3		vAxisDir = { 0.f,1.f,0.f }; // 임의의 축 설정.
		_float		fAxiAngleSpeed = { 0.f }; // 회전 스피드.
		_float		fAxiAngleForce = { 0.f }; // 회전 파워.
		_float2		vAxisAngleMinMax = { 0.f,0.f }; // Min, Max.

	}TRAIL_DESC;

private:
	CTrail_Effect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTrail_Effect(const CTrail_Effect& rhs);
	virtual ~CTrail_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(string strFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Play(_cref_time fTimeDelta);
	void	Reset();

public:
	virtual	void	Reset_Effect(_bool bActivate = false) override;
	virtual	void	Reset_Prototype_Data() override;

public:
	virtual void OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc) override;
	void			DissolveParticle();
	void			AxisRot(_cref_time fTimeDelta);

private:
	_float3		m_vAccPosition = {};

	_float3 m_pScales = {  };
	_float3 m_pRotations = {  };
	_float4 m_pColors = {  };
	_float3 m_pMoveSpeeds = {  };
	_float3 m_pScaleSpeeds = {  };
	_float3 m_pRotationSpeeds = {  };
	_float4 m_pColorSpeeds = {  };
	_float  m_fUVSpeeds = {};

	TRAIL_DESC  m_TrailDesc = {};
	_bool		m_bPlay = { true };

private:
	_bool		m_bDissovleStart = { false }; // 죽음 조건이 처리되면 디졸브 실행

	_float		m_fDissolveAmout = { 0.f };
	_float		m_fDissolveGradiationDistance = { 0.f };

private:
	_float	m_fLifeTimeAcc = { 0.f };

private:
	_float		m_fAxisAngleAcc = { 0.f }; // 임의의 축으로 돌릴 때 사용.
	_float		m_fAxisAngleSpeedAcc = { 0.f }; // 임의의 축으로 돌릴 때 사용.
	_bool		m_bAxisFinishRot = { false };

public:
	void Set_TrailDesc(TRAIL_DESC& NewDesc) { m_TrailDesc = NewDesc;}
	TRAIL_DESC* Get_TrailDesc() { return &m_TrailDesc; }
	GETSET_EX2(_bool, m_bPlay, Play, GET, SET)

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CTrail_Effect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	static  shared_ptr<CTrail_Effect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END