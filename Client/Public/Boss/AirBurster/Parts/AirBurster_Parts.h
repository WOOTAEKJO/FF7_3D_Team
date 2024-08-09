#pragma once
#include "../Public/Boss/Boss_Parts.h"

BEGIN(Client)

class CAirBurster_Parts abstract : public CBoss_Parts
{
	INFO_CLASS(CAirBurster_Parts, CBoss_Parts)

protected:
	CAirBurster_Parts(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAirBurster_Parts(const CAirBurster_Parts& rhs);
	virtual ~CAirBurster_Parts() = default;

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

public:
	_bool	Judge_Dead();

protected:
	_uint	m_iShaderPassIndex = { 0 };

protected:
	_bool	m_bIndependent = { false };	// ��ü�� ���� �и��Ǿ����� ����
	_bool	m_bMediate = { true }; // ���� �� ������� ���ߴ��� ����
	_bool	m_bPartsDead = { false }; //������ ���� ���·� ������ ����
	_bool	m_bReadyDocking = { false }; // ��ü�� ������ �غ� �Ǿ����� ����

protected:
	_bool	m_bTransition = { true }; // ��� ���ƿ��� ����� ������ ���� ����

protected:
	_bool	m_bDocking = { false };	// ��ǥ���� ���� �� ��ŷ�϶�� ��ü�� ���

protected:
	_bool	m_bCannon = { false }; // ��ü�� ĳ�� �������� ����

protected:
	_float4	m_vActivePos = { 0.f,0.f,0.f,1.f }; // Ȱ�������� ��, �и� �� ã�� ��ġ�� ����

protected:
	_bool	m_bIndependentStart = { false }; // ��Ʈ�ѷ��� �ʱ� ��ġ�� �������ֱ� ���ؼ�

protected:
	_float3					m_vMovement_Amount = { 0.f,0.f,0.f }; // �̵����� �ľ��ϱ� ����.

protected:
	shared_ptr<CPhysX_Collider> m_pPhysXColliderCom_Block;

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual	HRESULT	Ready_State() { return S_OK; }

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	virtual void onControllerHit(class CPhysX_Controller* pOtherController, const PxControllersHit& hit);

public:
	GETSET_EX2(_bool, m_bIndependent, Independent, GET, SET)
	GETSET_EX2(_bool, m_bPartsDead, PartsDead, GET, SET)
	GETSET_EX2(_bool, m_bTransition, Transition, GET, SET)
	GETSET_EX2(_bool, m_bReadyDocking, ReadyDocking, GET, SET)
	GETSET_EX2(_bool, m_bDocking, Docking, GET, SET)
	GETSET_EX2(_bool, m_bCannon, Cannon, GET, SET)
	GETSET_EX2(_float4, m_vActivePos, ActivePos, GET, SET)
	GETSET_EX2(_float3, m_vMovement_Amount, Movement_Amount, GET, SET)

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END