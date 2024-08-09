#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_HeavyStrike.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "Sound_Manager.h"

CState_Bahamut_HeavyStrike::CState_Bahamut_HeavyStrike(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_HeavyStrike::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkImpulse01", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

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

void CState_Bahamut_HeavyStrike::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_HeavyStrike::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	Cur_Player_Look(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(30.f))
	{
		if (m_pStrikeBulletL == nullptr)
		{
			m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"012_SE_Bahamut (B_AtkImpulse01_hand001).wav", ESoundGroup::Narration, 0.5f);

			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_HeavyStrike"), nullptr, &m_pStrikeBulletL);
			m_pStrikeBulletL->Set_Owner(m_pActor);

			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"L_Weapon_a");
			_matrix MuzzleMatrixTemp = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"L_ForearmrollB_End");

			_vector vLook = XMVector3Normalize(MuzzleMatrix.r[3] - MuzzleMatrixTemp.r[3]);

			m_pStrikeBulletL->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			m_pStrikeBulletL->Get_TransformCom().lock()->Set_Look_Manual(vLook);
			m_pStrikeBulletL->Set_BoneName(L"L_Weapon_a");
		}

		if (m_pStrikeBulletR == nullptr)
		{
			m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"013_SE_Bahamut (B_AtkImpulse01_hand002).wav", ESoundGroup::Narration, 0.5f);

			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_HeavyStrike"), nullptr, &m_pStrikeBulletR);
			m_pStrikeBulletR->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"R_Weapon_a");
			_matrix MuzzleMatrixTemp = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"R_ForearmrollB_End");

			_vector vLook = XMVector3Normalize(MuzzleMatrix.r[3] - MuzzleMatrixTemp.r[3]);

			m_pStrikeBulletR->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			m_pStrikeBulletR->Get_TransformCom().lock()->Set_Look_Manual(vLook);
			m_pStrikeBulletR->Set_BoneName(L"R_Weapon_a");
		}
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(71.f))
	{
		if (m_pStrikeBulletR)
		{
			m_pStrikeBulletR->Set_Independent(true);
		}
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(103.f))
	{
		if (m_pStrikeBulletL)
		{
			m_pStrikeBulletL->Set_Independent(true);
		}
	}
	
}

void CState_Bahamut_HeavyStrike::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_HeavyStrike::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_pStrikeBulletL = nullptr;
	m_pStrikeBulletR = nullptr;
}

bool CState_Bahamut_HeavyStrike::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_HeavyStrike::Free()
{
}
