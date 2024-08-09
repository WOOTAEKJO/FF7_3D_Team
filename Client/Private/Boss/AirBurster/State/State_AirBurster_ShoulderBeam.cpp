#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_ShoulderBeam.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"

#include "GameInstance.h"

CState_AirBurster_ShoulderBeam::CState_AirBurster_ShoulderBeam(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_ShoulderBeam::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMissile01", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE CheckAnimFinished = FUNCTION_NODE_MAKE
		{
			if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
				return BT_STATUS::Success;
			
			return BT_STATUS::Failure;
		};

		FUNCTION_NODE ShoulderBeam = FUNCTION_NODE_MAKE {
		if (m_iCount == 0)
		{
			_uint seed = std::chrono::system_clock::now().time_since_epoch().count();
			shuffle(m_Muzzles.begin(), m_Muzzles.end(), default_random_engine(seed));
		}
		if (m_iCount++ >= 11)
			m_iCount = 0;
			
		if (m_pActor_ModelCom.lock()->IsAnimation_Range(47.f, 95.f))
		{
			// L
			{
				shared_ptr<CAirBurster_ShoulderBeam> pBullet = { nullptr };
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_AirBurster_ShoulderBeam"), nullptr, &pBullet);
				pBullet->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzle") + m_Muzzles[m_iCount] + TEXT("_a"));
				pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				pBullet->Get_TransformCom().lock()->Set_Look_Manual({ 0.f, 1.f, 0.f });
			}

			// R
			{
				shared_ptr<CAirBurster_ShoulderBeam> pBullet = { nullptr };
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_AirBurster_ShoulderBeam"), nullptr, &pBullet);
				pBullet->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzle") + m_Muzzles[m_iCount] + TEXT("_a"));
				pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				pBullet->Get_TransformCom().lock()->Set_Look_Manual({ 0.f, 1.f, 0.f });
			}
		}
		
		

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(ShoulderBeam)
				.leaf<FunctionNode>(CheckAnimFinished)
				.leaf<FunctionNode>(Call_IDLE)
			.end()
		.build();
	}
	return S_OK;
}

void CState_AirBurster_ShoulderBeam::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_ShoulderBeam::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_ShoulderBeam::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_ShoulderBeam::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_iCount = 0;
}

bool CState_AirBurster_ShoulderBeam::isValid_NextState(CState* state)
{
	if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
	{
		return true;
	}
	else
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return true;
		else
			return false;
	}

}

void CState_AirBurster_ShoulderBeam::Free()
{
}
