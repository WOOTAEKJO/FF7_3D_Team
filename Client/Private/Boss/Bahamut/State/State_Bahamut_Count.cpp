#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Count.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "Sound_Manager.h"

CState_Bahamut_Count::CState_Bahamut_Count(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_Count::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkCountDown01", 0.5f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Condition_Anim_Finished
			= FUNCTION_NODE_MAKE
		{
			if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
				return BT_STATUS::Success;

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Aura
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Aura>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Condition_Anim_Finished)
			.leaf<FunctionNode>(Call_Aura)
			.end()
			.build();
	}

	m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"071_SE_Bahamut (FX_Countdown_Charge_01; FX_MegaFlare_Charge_01).wav", ESoundGroup::Narration, 0.5f);

	return S_OK;
}

void CState_Bahamut_Count::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

}

void CState_Bahamut_Count::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(30.f))
	{
		CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();

		if ((eCurPhase == CBahamut::PHASE::PHASE0) || (eCurPhase == CBahamut::PHASE::PHASE1) || 
			(eCurPhase == CBahamut::PHASE::PHASE2))
		{
			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_Count_Splash4_2", m_pActor.lock(),
				CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 4.f, 0.f));
		}
		else if ((eCurPhase == CBahamut::PHASE::PHASE3) || (eCurPhase == CBahamut::PHASE::PHASE4) || (eCurPhase == CBahamut::PHASE::PHASE5))
		{
			GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_Count_Splash5_2", m_pActor.lock(),
				CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 4.f, 0.f));
		}
	}
}

void CState_Bahamut_Count::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Count::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_Count::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Count::Free()
{
}
