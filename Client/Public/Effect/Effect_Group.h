#pragma once
#include "Effect.h"

BEGIN(Engine)

END

BEGIN(Client)

class CEffect_Group final  : public CEffect
{
public:
	typedef struct tag_Effect_Group_Desc
	{
		shared_ptr<CEffect> pEffect = {nullptr};
		_bool		bEnd = { false };
		EFFECT_TYPE	eEffectType = { EFFECT_TYPE_END };
		string		strFilePath;
		string		strFileName;
		_float		fStartTime = { 0.f };
		_float4x4	matWorld; 

	}EFFECT_GROUP_DESC;

private:
	CEffect_Group(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CEffect_Group(const CEffect_Group& rhs);
	virtual ~CEffect_Group() = default;

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

public:
	HRESULT	Add_Effect(EFFECT_TYPE eType,const string& strFilePath, weak_ptr<CEffect> pEffect);
	void	Delete_Effect(_uint iIndex);
	void	Reset_Group();

	void	Update_Effect(_cref_time fTimeDelta);
	

	void	Replicate_Effect();
	void	All_Add_Time(_float fTime);

public:
	vector<EFFECT_GROUP_DESC>* Get_GroupList() { return &m_vecGroup; }
	GETSET_EX2(_bool, m_bUpdate, Update, GET, SET)
	GETSET_EX2(CEffect::USE_TYPE, m_eOwnerType, OwnerType, GET, SET)

private:
	vector<EFFECT_GROUP_DESC> m_vecGroup;

private:
	_bool				m_bUpdate = { true };

private:
	CEffect::USE_TYPE	m_eOwnerType = { CEffect::USE_TYPE::USE_NONE}; // 오너 타입에 따라 계산식을 다르게 한다.
	// 계산식에만 영향을 받는다.

private:
	void	Create_Effect();
	void	Tool_Create_Effect();
	void	Judge_Dead();

	void	Update_GroupEffect_Matrix();

	void	Connection_Owner();

public:
	virtual void	Reset_Effect(_bool bActivate = false) override;
	virtual	void	Reset_Prototype_Data() override;

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

	void	Write_Data(json& Out_Json,_uint iIndex);
	void	Load_Data(const json& In_Json,_uint iIndex);

public:
	static	shared_ptr<CEffect_Group> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static	shared_ptr<CEffect_Group> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const string& strFilePath);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END