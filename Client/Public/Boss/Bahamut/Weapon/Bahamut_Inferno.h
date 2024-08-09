#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CBahamut_Inferno : public CBullet
{
	INFO_CLASS(CBahamut_Inferno, CBullet)

private:
	CBahamut_Inferno(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CBahamut_Inferno(const CBahamut_Inferno& rhs);
	virtual ~CBahamut_Inferno() = default;

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

private:
	wstring		m_strBoneName;				// �� �̸� ����

private:
	_float m_fAccChaseValue = { 0.f };		// ���� ���� ũ��. ���� Ŀ������ �����.
	_float m_fSpeed = { 3.2f };				// �ӵ�
	_float m_fRenewPosTime = { 0.f };		// Ÿ�� ��ġ�� ���ʿ� �� ���� ���ŵǴ���
	_uint  m_iRenewCount = { 0 };			// ���� ���ŵ� Ƚ��
	_float3 m_fTargetPos = { 0.f,0.f,0.f };	// Ÿ�� ��ġ ����

	_float m_fStartTime = { 0.f };			// Ÿ�� �ٽ� ���� ���� �ð�

private:
	FTimeChecker	m_TimeChecker;

public:
	static shared_ptr<CBahamut_Inferno> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END