#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CBahamut_Meteor : public CBullet
{
	INFO_CLASS(CBahamut_Meteor, CBullet)

private:
	CBahamut_Meteor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CBahamut_Meteor(const CBahamut_Meteor& rhs);
	virtual ~CBahamut_Meteor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	GETSET_EX2(wstring, m_strBoneName, BoneName, GET, SET)
	GETSET_EX2(_bool, m_bFinished, Finished, GET, SET)
private:
	wstring		m_strBoneName;
	_bool		m_bFinished = { false };
	_bool		m_bTest = { true };

private:
	FTimeChecker	m_TimeChecker_Tail;

private:
	shared_ptr<CParticle>	m_pAlwaysParticle[3] = {};
	shared_ptr<class CLight> m_pLight = { nullptr };

public:
	static shared_ptr<CBahamut_Meteor> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END