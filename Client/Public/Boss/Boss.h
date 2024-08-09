#pragma once
#include "Client_Defines.h"
#include "Monster.h"

#define AI_ACTION_SKILL -10 //스킬 공격
#define AI_ACTION_NOR_ATTACK 0 // 일반 공격 3세트 (중간 중간에 배회)
#define AI_ACTION_GUARD 5 // 회피(구르기) 후 타겟 바라보며 가드 상태(5 끝날때까지)
#define AI_ACTION_RUNAWAY 10 // 뒤로 돌아서 뛰어서 도망감
#define AI_ACTION_NON_TARGET 20 // 기존 타겟팅 풀리고 20이 아닌 몬스터 탐색 및 타겟팅

BEGIN(Client)

class CBoss abstract : public CMonster
{
	INFO_CLASS(CBoss, CMonster)

protected:
	CBoss(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

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

protected: // 임시 체력
	_int	m_iMaxHP = { 100 };
	_int	m_iCurHP = { 100 };

protected:
	_float3		m_vMotionBlur_Offset = { 0.f,0.f,0.f };

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual HRESULT	Ready_PartObjects() { return S_OK; }
	virtual	HRESULT	Ready_State() { return S_OK; }

protected:
	void	SetUp_Controller(_float fHeight, _float fRadius);

	void	Parts_Command(wstring wstrPartsTag, _uint iCommandIndex);

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	GETSET_EX2(_int, m_iMaxHP, MaxHP, GET, SET)
	GETSET_EX2(_int, m_iCurHP, CurHP, GET, SET)
	GETSET_EX2(_float3, m_vMotionBlur_Offset, MotionBlur_Offset, GET, SET)

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;

};

END