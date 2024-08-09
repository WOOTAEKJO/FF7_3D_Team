#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CBahamut_FlareBall : public CBullet
{
	INFO_CLASS(CBahamut_FlareBall, CBullet)

private:
	CBahamut_FlareBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CBahamut_FlareBall(const CBahamut_FlareBall& rhs);
	virtual ~CBahamut_FlareBall() = default;

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

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	GETSET_EX2(wstring, m_strBoneName, BoneName, GET, SET)
	GETSET_EX2(_bool, m_bIndependent, Independent, GET, SET)
	GETSET_EX2(_bool, m_bFinished, Finished, GET, SET)

private:
	wstring				m_strBoneName;
	FTimeChecker		m_TimeChecker;

private:
	_bool				m_bIndependent = { false };
	_bool				m_bActivateBullet = { true }; // 활동하는지
	_bool				m_bFinished = { false }; // 이펙트까지 끝났는지

private:
	shared_ptr<CEffect_Group>	m_pHitEffect = { nullptr };

public:
	static shared_ptr<CBahamut_FlareBall> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END