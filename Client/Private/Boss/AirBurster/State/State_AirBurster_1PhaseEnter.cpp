#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_1PhaseEnter.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CState_AirBurster_1PhaseEnter::CState_AirBurster_1PhaseEnter(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_1PhaseEnter::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Turn01_R90", 1.f * AIRBURSTERANIMSPEED, false, false);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Call_Phase1
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Wait_State<CState_AirBurster_Control_Phase1>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_Phase1)
			.end()
			.build();
	}

	DynPtrCast<CAirBurster>(m_pActor.lock())->OnOff_Block(true);

	return S_OK;
}

void CState_AirBurster_1PhaseEnter::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_1PhaseEnter::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_1PhaseEnter::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_1PhaseEnter::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_ChangePhase(false);
	m_pActor_TransformCom.lock()->Rotation(_float4(0.f,1.f,0.f,0.f),XMConvertToRadians(90.f));
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(false);

	m_pActor.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	// 콜라이더 On

	GET_SINGLE(CUI_Manager)->Make_MonsterUI("AirBuster", m_pActor.lock());
	// UI ON
}

bool CState_AirBurster_1PhaseEnter::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_1PhaseEnter::Free()
{
}
