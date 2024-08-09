#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Burner.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_Burner.h"
#include "CommonModelComp.h"

#include "GameInstance.h"

CState_AirBurster_Burner::CState_AirBurster_Burner(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_bTimeChekcer = FTimeChecker(0.2f);
}

HRESULT CState_AirBurster_Burner::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkVernier01", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Burner_Breathe = FUNCTION_NODE_MAKE {

		if (m_pActor_ModelCom.lock()->IsAnimation_Range(0.f, 100.f))
		{
			if (m_bTimeChekcer.Update(fTimeDelta))
			{
				// L
				{
					shared_ptr<CAirBurster_Burner> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_Burner"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("L_Hand_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[0]);
				}

				// R
				{
					shared_ptr<CAirBurster_Burner> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_Burner"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("R_Hand_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[0]);
				}
			}
		}

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_IDLE = FUNCTION_NODE_MAKE {

			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Burner_Breathe)
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_Burner::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Burner::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Burner::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Burner::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster_Burner::isValid_NextState(CState* state)
{
	if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
	{
		return true;
	}
	else {
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return true;
		else
			return false;
	}

}

void CState_AirBurster_Burner::Free()
{
}
