#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_FingerBeam.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "../Public/Boss/AirBurster/Weapon/AirBurster_FingerBeam.h"

#include "GameInstance.h"

CState_AirBurster_Arm_FingerBeam::CState_AirBurster_Arm_FingerBeam(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_FingerBeam::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkBeam01", 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Call_Term
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Term>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_Term)
			.end()
			.build();
	}
	
	return S_OK;
}

void CState_AirBurster_Arm_FingerBeam::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	Cur_Player_Look(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_FingerBeam::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bCheck && m_pActor_ModelCom.lock()->IsAnimation_UpTo(64.f))
	{
		m_bCheck = false;

		{
			wstring strText[5] = { L"U",L"V" ,L"W" ,L"X" ,L"Y" };

			if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_LeftArm")))
			{
				for (_uint i = 0; i < 5; i++)
				{
					shared_ptr<CAirBurster_FingerBeam> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_FingerBeam"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_FingerBeam_Physics");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzle") + strText[i] + TEXT("_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					pBullet->Set_BoneName(TEXT("L_VFXMuzzle") + strText[i] + TEXT("_a"));
				}
			}
			else if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_RightArm")))
			{
				for (_uint i = 0; i < 5; i++)
				{
					shared_ptr<CAirBurster_FingerBeam> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_FingerBeam"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_FingerBeam_Magic");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzle") + strText[i] + TEXT("_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					pBullet->Set_BoneName(TEXT("R_VFXMuzzle") + strText[i] + TEXT("_a"));
				}
			}
		}
	}
}

void CState_AirBurster_Arm_FingerBeam::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_FingerBeam::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_Transition(false);

	m_bCheck = true;
}

bool CState_AirBurster_Arm_FingerBeam::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_Arm_FingerBeam::Free()
{
}
