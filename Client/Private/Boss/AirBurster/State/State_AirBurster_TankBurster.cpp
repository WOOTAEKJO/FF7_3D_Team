#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_TankBurster.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_TankBurster.h"
#include "CommonModelComp.h"

#include "GameInstance.h"

CState_AirBurster_TankBurster::CState_AirBurster_TankBurster(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_TankBurster::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_strCurAnimTag = "Main|B_AtkCannon01_0";
	m_pActor_ModelCom.lock()->Set_Animation(m_strCurAnimTag, 1.f * AIRBURSTERANIMSPEED, false,
		dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_RUNAWAY);

	static_pointer_cast<CAirBurster>(m_pActor.lock())->Commnad_Cannon(true);
	// 팔들 캐논 명령어 활성화

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Burner_TankBurster
			= FUNCTION_NODE_MAKE
		{
			
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Wait_State<CState_AirBurster_IDLE>();
			
			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	return S_OK;
}

void CState_AirBurster_TankBurster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (!m_bNextAnim && m_pActor_ModelCom.lock()->IsAnimation_Finished(m_strCurAnimTag))
		m_bNextAnim = true;

	if (m_bNextAnim)
	{
		if (m_iFrame == 0)
		{
			m_fAnimSpeed = 1.f;
			m_strCurAnimTag = "Main|B_AtkCannon01_1";
		}
			
		else if (m_iFrame == 1)
		{
			m_fAnimSpeed = 1.f;
			m_strCurAnimTag = "Main|B_AtkCannon01_2";
		}
		else if (m_iFrame == 2)
		{
			return;
		}

		m_iFrame += 1;
		m_bNextAnim = false;

		m_pActor_ModelCom.lock()->Set_Animation(m_strCurAnimTag, m_fAnimSpeed * AIRBURSTERANIMSPEED, false);
	}

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_TankBurster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if ((m_iFrame == 2) && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(50.f))
	{
		shared_ptr<CAirBurster_TankBurster> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_AirBurster_TankBurster"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor);

		_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += 3.f;
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, vPos);
	}
}

void CState_AirBurster_TankBurster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_TankBurster::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);

	m_iFrame = 0;
	m_bNextAnim = false;

	m_bCheck = true;

	m_fAnimSpeed = 1.f;

	static_pointer_cast<CAirBurster>(m_pActor.lock())->Commnad_Cannon(false);
	// 팔들 캐논 명령어 비활성화
}

bool CState_AirBurster_TankBurster::isValid_NextState(CState* state)
{

	if (m_iFrame == 2 && m_pActor_ModelCom.lock()->IsAnimation_Finished(m_strCurAnimTag))
	{
		return true;
	}
	else
		return false;
}

void CState_AirBurster_TankBurster::Free()
{
}
