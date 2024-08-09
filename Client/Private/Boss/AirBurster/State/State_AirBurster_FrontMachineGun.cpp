#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_FrontMachineGun.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_MachineGun.h"
#include "CommonModelComp.h"

#include "GameInstance.h"

CState_AirBurster_FrontMachineGun::CState_AirBurster_FrontMachineGun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_bTimeCheckerL = FTimeChecker(0.1f);
	m_bTimeCheckerR = FTimeChecker(0.12f);
}

HRESULT CState_AirBurster_FrontMachineGun::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkVulcan01", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE AnimFinished = FUNCTION_NODE_MAKE{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return BT_STATUS::Success;

		return BT_STATUS::Failure;
		};

		FUNCTION_NODE Shoot_Machinegun = FUNCTION_NODE_MAKE {
		if (m_pActor_ModelCom.lock()->IsAnimation_Range(20.f, 40.f))
		{
			if (m_bTimeCheckerL.Update(fTimeDelta))
			{
				// 머신건 생성
				{
					shared_ptr<CAirBurster_MachineGun> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_MachineGun"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_MachineGunFront");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzleA_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);
				}
			}

			if (m_bTimeCheckerR.Update(fTimeDelta))
			{

				// 머신건 생성
				{
					shared_ptr<CAirBurster_MachineGun> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_MachineGun"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_MachineGunFront");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzleA_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);
				}
			}
		}

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_RearMachineGun
			= FUNCTION_NODE_MAKE
		{

			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RearMachineGun>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Shoot_Machinegun)
				.leaf<FunctionNode>(AnimFinished)
				.leaf<FunctionNode>(Call_RearMachineGun)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_FrontMachineGun::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_FrontMachineGun::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_FrontMachineGun::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_FrontMachineGun::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster_FrontMachineGun::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_FrontMachineGun::Free()
{
}
