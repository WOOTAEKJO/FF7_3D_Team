#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

/// <summary>
/// ����������� �������Ǳ�
/// </summary>
class CAirBurster_EMField : public CBullet
{
	INFO_CLASS(CAirBurster_EMField, CBullet)

private:
	CAirBurster_EMField(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CAirBurster_EMField(const CAirBurster_EMField& rhs);
	virtual ~CAirBurster_EMField() = default;

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
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;

public:
	static shared_ptr<CAirBurster_EMField> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	_bool m_bIsChaseEnd = { false };		// ü�̽� �����ϸ� �� ����
	_float3	m_vTargetPos = {};				// ��ǥ Ÿ������.
	_float m_fAccChaseValue = { 0.f };		// ���� ���� ũ��. ���� Ŀ������ �����.
	_float m_fSpeed = { 2.f };
	FTimeChecker	m_fDamageTick = { 0.3f };
};

END