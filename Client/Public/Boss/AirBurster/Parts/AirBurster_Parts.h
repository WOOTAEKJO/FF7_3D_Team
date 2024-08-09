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
	_bool	m_bIndependent = { false };	// 본체와 팔이 분리되었는지 유무
	_bool	m_bMediate = { true }; // 독립 후 월드행렬 맞추는지 유무
	_bool	m_bPartsDead = { false }; //파츠가 죽음 상태로 들어갔는지 유무
	_bool	m_bReadyDocking = { false }; // 본체와 결합할 준비가 되었는지 유무

protected:
	_bool	m_bTransition = { true }; // 상시 돌아오는 페이즈에 보간을 할지 유무

protected:
	_bool	m_bDocking = { false };	// 목표지점 도달 시 도킹하라고 본체에 명령

protected:
	_bool	m_bCannon = { false }; // 본체가 캐논 상태인지 유무

protected:
	_float4	m_vActivePos = { 0.f,0.f,0.f,1.f }; // 활동상태일 때, 분리 후 찾은 위치를 저장

protected:
	_bool	m_bIndependentStart = { false }; // 컨트롤러의 초기 위치를 세팅해주기 위해서

protected:
	_float3					m_vMovement_Amount = { 0.f,0.f,0.f }; // 이동량을 파악하기 위함.

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