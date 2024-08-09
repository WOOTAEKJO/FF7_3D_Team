#pragma once
#include "../Public/Boss/Boss.h"

#define FLYING 17
#define HOVER 18

#define LEFTARM 19
#define LEFTARMPLATE 20
#define RIGHTARM 21
#define RIGHTARMPLATE 22

BEGIN(Client)

class CAirBurster final : public CBoss
{
	INFO_CLASS(CAirBurster, CBoss)

public:
	enum AIRBURSTER_PAHSE { INTRO, PHASE1, PHASE2, PHASE3, PHASE_END };
	enum WEAPON_COMMAND {WEAPON_NONE,WEAPON_IDLE,WEAPON_ATTACK,WEAPON_COMMAND_END};
	enum ARM_COMMAND {ARM_NONE,ARM_IDLE, ARM_COMMAND_END};

private:
	CAirBurster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAirBurster(const CAirBurster& rhs);
	virtual ~CAirBurster() = default;

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
	_bool	Judge_Phase();
	_bool	Judge_Docking();
	void	Ready_Docking();
	_bool	Judge_Combination(_int* iType);
	void	Commnad_Cannon(_bool bCheck);

	void	Thunder_Effect(_cref_time fTimeDelta);

private:
	_bool					m_bSeparateArm[2] = {false}; // 팔 메시 렌더 판단
	_bool					m_bFlyingMode = { false }; // 하단 메시 렌더 판단
			
private:
	AIRBURSTER_PAHSE		m_eCurPhase = { PHASE_END }; // 현재 페이즈를 나타낸다.
	_bool					m_bChangePhase = { false }; // 페이즈 전환 판단

private:
	_bool					m_bTransition = { true }; // 상시 돌아오는 페이즈에 보간을 할지 유무

private:
	_float3					m_vOriginPos = { 84.1f,2.5f,89.2f }; // 1페이즈 오리진 포스

private:
	shared_ptr<class CLight > m_pLight;

private:
	_bool					m_bPlayerCol = { false }; // 페이즈1 RearMachineGun 때 충돌 체크

private:
	_float3					m_vMovement_Amount = { 0.f,0.f,0.f }; // 이동량을 파악하기 위함.
	// 페이즈1 RearMachineGun 때 사용.

private:
	FTimeChecker			m_ThunderEffect_TimeChecker;

private:
	shared_ptr<class CAirBurster_Block> m_pBlock = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual HRESULT	Ready_PartObjects() override;
	virtual	HRESULT	Ready_State() override;

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
	GETSET_EX2(_bool, m_bFlyingMode, FlyingMode, GET, SET)
	GETSET_EX2(_bool, m_bChangePhase, ChangePhase, GET, SET)
	GETSET_EX2(_bool, m_bTransition, Transition, GET, SET)
	GETSET_EX2(AIRBURSTER_PAHSE, m_eCurPhase, CurPhase, GET, SET)
	GETSET_EX2(_float3, m_vOriginPos, OriginPos, GET, SET)
	GETSET_EX2(_bool, m_bPlayerCol, PlayerCol, GET, SET)
	GETSET_EX2(_float3, m_vMovement_Amount, Movement_Amount, GET, SET)

public:
	void	Set_SeparateArm(wstring strPartsName, _bool bCheck);
	_bool	Get_SeparateArm(wstring strPartsName);

	_bool	Is_All_SeparateArm();

	void	OnOff_Block(_bool bOnOff);

public:
	static shared_ptr<CAirBurster> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END