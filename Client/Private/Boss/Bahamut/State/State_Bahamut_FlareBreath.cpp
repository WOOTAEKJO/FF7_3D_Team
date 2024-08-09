#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_FlareBreath.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_FlareBreath::CState_Bahamut_FlareBreath(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(0.15f);
}

HRESULT CState_Bahamut_FlareBreath::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDarkBreath01", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Condition_Anim_Finished
			= FUNCTION_NODE_MAKE
		{
			if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
				return BT_STATUS::Success;

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Shoot_Breath
			= FUNCTION_NODE_MAKE
		{
			if (m_TimeChecker.Update(fTimeDelta) && m_pActor_ModelCom.lock()->IsAnimation_Range(25.f,60.f))
			{
				{
					shared_ptr<CBahamut_Breath> pBullet = nullptr;
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_Bahamut_Breath"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(L"C_Tongue_End");
					_matrix MuzzleMatrixTemp = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(L"C_Tongue_c");

					_vector vLook = XMVector3Normalize(MuzzleMatrix.r[3] - MuzzleMatrixTemp.r[3]);

					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);
					pBullet->Set_BoneName(L"C_Tongue_End");
				}
			}

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Shoot_Breath)
			.leaf<FunctionNode>(Condition_Anim_Finished)
			.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}
	return S_OK;
}

void CState_Bahamut_FlareBreath::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

}

void CState_Bahamut_FlareBreath::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Range(0.f, 20.f))
	{
		Cur_Player_Look(fTimeDelta);
		auto PlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
		m_pActor_TransformCom.lock()->Go_Target(PlayerInfo.vTargetPos, fTimeDelta, 3.f);
	}
}

void CState_Bahamut_FlareBreath::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_FlareBreath::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_FlareBreath::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_FlareBreath::Free()
{
}
