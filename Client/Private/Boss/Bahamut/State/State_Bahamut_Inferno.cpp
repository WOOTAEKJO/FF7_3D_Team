#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Inferno.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_Inferno::CState_Bahamut_Inferno(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
}

HRESULT CState_Bahamut_Inferno::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkHomingLaser01", 1.f, false,
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

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Condition_Anim_Finished)
			.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}
	return S_OK;
}

void CState_Bahamut_Inferno::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_Inferno::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	Cur_Player_Look(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(45.f) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(65.f)
		|| m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(85.f))
	{
		for (_uint i = 1; i < 13; ++i)
		{
			shared_ptr<CBahamut_Inferno> pBullet = { nullptr };
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_Inferno"), nullptr, &pBullet);
			pBullet->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Tongue_End");

			pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);

			_vector vLook = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_UP));

			pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);

			_vector vBulletUp = XMVector3Normalize(pBullet->Get_TransformCom().lock()->Get_State(CTransform::STATE_UP));
			_float fAngle =30.f * i;
			pBullet->Get_TransformCom().lock()->Rotation(vBulletUp, XMConvertToRadians(fAngle));
			pBullet->Set_BoneName(L"C_Tongue_End");
		}
	}
}

void CState_Bahamut_Inferno::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CState_Bahamut_Inferno::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_Inferno::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Inferno::Free()
{
}
