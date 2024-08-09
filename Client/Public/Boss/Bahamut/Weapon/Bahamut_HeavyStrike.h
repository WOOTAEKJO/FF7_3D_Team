#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CBahamut_HeavyStrike : public CBullet
{
	INFO_CLASS(CBahamut_HeavyStrike, CBullet)

private:
	CBahamut_HeavyStrike(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CBahamut_HeavyStrike(const CBahamut_HeavyStrike& rhs);
	virtual ~CBahamut_HeavyStrike() = default;

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

private:
	wstring		m_strBoneName;

private:
	_bool				m_bIndependent = { false };	// 독립 판단

private:
	_float m_fAccChaseValue = { 0.f };		// 추적 각도 크기. 점점 커지도록 만든다.
	_float m_fSpeed = { 3.2f };

public:
	static shared_ptr<CBahamut_HeavyStrike> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END