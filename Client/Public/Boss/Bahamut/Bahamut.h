#pragma once
#include "../Public/Boss/Boss.h"

BEGIN(Client)

class CBahamut final : public CBoss
{
	INFO_CLASS(CBahamut, CBoss)

public:
	enum class PHASE { PHASE0 = 0, PHASE1 = 1, PHASE2 = 2, PHASE3 = 3, PHASE4 = 4, PHASE5 = 5};

private:
	CBahamut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBahamut(const CBahamut& rhs);
	virtual ~CBahamut() = default;

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

private:
	_bool					m_bTransition = { true }; // ��� ���ƿ��� ����� ������ ���� ����
	PHASE					m_eCurPhase = { PHASE::PHASE0 };	// ���� ī��Ʈ ���̽��� ��Ÿ����.

private:
	_bool					m_bCrossAlwaysEffect = { true }; // ��� ����Ʈ�� �����ư��鼭 �����ϱ� ����

private:
	shared_ptr<CEffect>		m_pAlwaysRing = { nullptr }; // ��� ����Ʈ �� ��ƼŬ 

private:
	FTimeChecker			m_TImeChecker_EffectRing; // ��� ����Ʈ �� Ÿ�� üũ
	_uint					m_iEffectRing_Frame = { 0 }; // �� ����Ʈ �����Ӻ��� 

private:
	FTimeChecker			m_TimeChecker_EffectEye; // ��� ����Ʈ �� Ÿ�� üũ

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual	HRESULT	Ready_State() override;

private:
	void	Always_Effect_Step(_cref_time fTimeDelta);

public:
	void	Always_Effect_Ring(_cref_time fTimeDelta);
	void	Always_Particle_Ring_ON_OFF(_bool bOnOff);

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
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override;

public:
	GETSET_EX2(_bool, m_bTransition, Transition, GET, SET)
	GETSET_EX2(PHASE, m_eCurPhase, CurPhase, GET, SET)
	GETSET_EX2(_uint, m_iEffectRing_Frame, EffectRing_Frame, GET, SET)

public:
	static shared_ptr<CBahamut> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END