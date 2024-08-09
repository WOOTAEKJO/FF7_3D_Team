#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Trail;
class CBoneGroup;
END

BEGIN(Client)

class CTrail_Buffer : public CEffect
{
public:
	typedef struct tagTrailBuffer_Desc : CEffect::EFFECT_DESC
	{
		_bool		bUpdate = { true };

		_float3     vPos_0 = {0.f,0.f,0.f};
		_float3     vPos_1 = { 0.f,1.f,0.f };
		_uint      iMaxCnt = {1};
		_uint		iLerpPointNum = { 12 };

		shared_ptr<CGameObject> pOwner = { nullptr };
		// 저장을 어떻게해야 하지
		// 저장하지 말고 붙이는 식으로 하자

		_float3		vWeight = { 1.f,1.f,1.f };

		_bool		bMaskInverse = { false };
		_bool		bDiffuseClamp = {true};
		_bool		bMaskClamp = { true };
		_bool		bNoiseClamp = { true };
		
		// ----- socket -------
		//OWNER_TYPE eOwnerType = { OWNER_TYPE::OBJ_TYPE_END }; // 오너의 타입
		_bool	bSocket = { false }; // 뼈를 사용하는지
		CBoneGroup* pBoneGroup = { nullptr }; // 뼈 그룹
		wstring	  strBoneName; // 뼈 이름

		// ------ Diffuse -------
		_bool		bDiffuseUse = { false }; // 재질을 사용할 것인지.

	}TRAILBUFFER_DESC;

protected:
	CTrail_Buffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTrail_Buffer(const CTrail_Buffer& rhs);
	virtual ~CTrail_Buffer() = default;

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

	void Update(_fmatrix matOwner);
	void Reset(_fmatrix matOwner);
	void Trail_Pos_Reset() { m_bIsReset = true; }

public:
	virtual	void	Reset_Effect(_bool bActivate = false) override;
	virtual	void	Reset_Prototype_Data() override;

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

private:
	TRAILBUFFER_DESC m_tTrailBuffer_Desc;
	_uint				m_iBoneIndex = { 0 };

private:
	_bool				m_bIsReset = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void	Judge_Dead(EFFECT_DESC EffectDesc) override;
	void	Update_WorldMatrix();

public:
	GETSET_EX2(TRAILBUFFER_DESC, m_tTrailBuffer_Desc, TrailBuffer_Desc, GET, SET)

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CTrail_Buffer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CTrail_Buffer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END