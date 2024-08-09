#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Hook.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"

#include "GameInstance.h"

CState_AirBurster_Hook::CState_AirBurster_Hook(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Hook::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	_vector vLook = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

	if (XMVectorGetX(XMVector3Dot(vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f))) < 0.f)
	{// 왼쪽 판단
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkHookL01", 2.f * AIRBURSTERANIMSPEED, false);
		m_strBoneName = TEXT("L_Hand_a");
	}
	else { // 오른쪽 판단
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkHookR01", 2.f * AIRBURSTERANIMSPEED, false);
		m_strBoneName = TEXT("R_Hand_a");
	}

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Shoot_Hook = FUNCTION_NODE_MAKE{
		if (!m_bCheck && m_pActor_ModelCom.lock()->IsAnimation_UpTo(45.f))
		{
			m_bCheck = true;

			shared_ptr<CAirBurster_Hook> pBullet = { nullptr };
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_AirBurster_Hook"), nullptr, &pBullet);
			pBullet->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(m_strBoneName);
			pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			pBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[2]);
			pBullet->Set_BoneName(m_strBoneName);
		} // 훅 콜라이더 생성

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_Return
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Hook_Return>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Shoot_Hook)
				.leaf<FunctionNode>(Call_Return)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_Hook::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Hook::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Hook::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Hook::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);
	m_bCheck = false;
}

bool CState_AirBurster_Hook::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_Hook::Free()
{
}
