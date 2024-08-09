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
		// ������ ����ؾ� ����
		// �������� ���� ���̴� ������ ����

		_float3		vWeight = { 1.f,1.f,1.f };

		_bool		bMaskInverse = { false };
		_bool		bDiffuseClamp = {true};
		_bool		bMaskClamp = { true };
		_bool		bNoiseClamp = { true };
		
		// ----- socket -------
		//OWNER_TYPE eOwnerType = { OWNER_TYPE::OBJ_TYPE_END }; // ������ Ÿ��
		_bool	bSocket = { false }; // ���� ����ϴ���
		CBoneGroup* pBoneGroup = { nullptr }; // �� �׷�
		wstring	  strBoneName; // �� �̸�

		// ------ Diffuse -------
		_bool		bDiffuseUse = { false }; // ������ ����� ������.

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
	/* ������ü�� �����Ѵ�. */
	static shared_ptr<CTrail_Buffer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CTrail_Buffer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* �纻��ü�� �����Ѵ�. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END