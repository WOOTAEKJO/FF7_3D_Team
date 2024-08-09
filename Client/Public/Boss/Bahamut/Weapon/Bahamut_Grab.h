#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

#include "Client_Manager.h"

BEGIN(Client)

class CBahamut_Grab : public CBullet
{
	INFO_CLASS(CBahamut_Grab, CBullet)

private:
	CBahamut_Grab(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CBahamut_Grab(const CBahamut_Grab& rhs);
	virtual ~CBahamut_Grab() = default;

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
	GETSET_EX2(_bool, m_bGrabCol, GrabCol, GET, SET)
	GETSET_EX2(PLAYER_TYPE, m_eCol_PlayerType, Col_PlayerType, GET, SET)

private:
	wstring		m_strBoneName;
	_bool		m_bGrabCol = { false }; // 충돌했는지 판단

private:
	PLAYER_TYPE	m_eCol_PlayerType = { PLAYER_TYPE::PLAYER_END };

public:
	static shared_ptr<CBahamut_Grab> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END