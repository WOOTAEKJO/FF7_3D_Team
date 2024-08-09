#pragma once

#include "Effect.h"

BEGIN(Engine)

END

BEGIN(Client)

class CSprite final  : public CEffect
{
public:
	typedef struct tagEffect_Rect_Desc : public CEffect::EFFECT_DESC
	{
	
	}EFFECT_RECT_DESC;

private:
	CSprite(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CSprite(const CSprite& rhs);
	virtual ~CSprite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(string strFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual	void	Reset_Effect(_bool bGroup = false) override;

public:
	GETSET_EX2(EFFECT_RECT_DESC, m_tEffect_Desc, Effect_Desc, GET, SET)

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

private:
	EFFECT_RECT_DESC	m_tEffect_Desc;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CSprite> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static  shared_ptr<CSprite> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const string& strFilePath);
	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END