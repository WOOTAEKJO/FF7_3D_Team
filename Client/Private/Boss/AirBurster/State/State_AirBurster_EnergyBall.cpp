#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_EnergyBall.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_EnergyBall.h"
#include "CommonModelComp.h"

#include "GameInstance.h"

CState_AirBurster_EnergyBall::CState_AirBurster_EnergyBall(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_bTimeChecker = FTimeChecker(0.5f);
}

HRESULT CState_AirBurster_EnergyBall::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkEnergy01_1", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Shoot_EnergyBall = FUNCTION_NODE_MAKE {

			if (m_pActor_ModelCom.lock()->IsAnimation_Range(10.f, 100.f))
			{
				// 에너지볼 생성
				if (m_bTimeChecker.Update(fTimeDelta))
				{
					for (_int i = -7; i <= 7; ++i)
					{
						_float fRadian = XMConvertToRadians(Cast<_float>(i * 5));

						shared_ptr<CAirBurster_EnergyBall> pBullet = { nullptr };
						m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
							TEXT("Prototype_GameObject_AirBurster_EnergyBall"), nullptr, &pBullet);
						pBullet->Set_Owner(m_pActor);
						_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
							Get_BoneTransformMatrixWithParents(TEXT("C_VFXMuzzleA_a"));
						pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
						pBullet->Get_TransformCom().lock()->Set_Look_Manual(-MuzzleMatrix.r[1]);
						pBullet->Get_TransformCom().lock()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
						pBullet->Get_TransformCom().lock()->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(20.f));
					}
				}
			}

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_Next = FUNCTION_NODE_MAKE {

			if(static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase()==CAirBurster::PHASE1)
				m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RearMachineGun>();
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase() == CAirBurster::PHASE2)
				m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Shoot_EnergyBall)
				.leaf<FunctionNode>(Call_Next)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_EnergyBall::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_EnergyBall::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_EnergyBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_EnergyBall::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_AirBurster_EnergyBall::isValid_NextState(CState* state)
{
	if (dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
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

void CState_AirBurster_EnergyBall::Free()
{
}
