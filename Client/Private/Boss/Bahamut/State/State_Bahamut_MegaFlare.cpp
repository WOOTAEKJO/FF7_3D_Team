#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_MegaFlare.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "Sound_Manager.h"

CState_Bahamut_MegaFlare::CState_Bahamut_MegaFlare(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(2.f);
}

HRESULT CState_Bahamut_MegaFlare::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMegaFlare01", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_RUNAWAY);

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

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Bahamut_Limit_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	m_pActor_TransformCom.lock()->Set_Position(1.f, XMVectorSet(25.f, 0.f, 33.f, 1.f));

	m_pActor_TransformCom.lock()->Look_At_OnLand(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CState_Bahamut_MegaFlare::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

}

void CState_Bahamut_MegaFlare::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(110.f))
	{
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_Bahamut_FlareBall"), nullptr, &m_pFlareBall);
		m_pFlareBall->Set_Owner(m_pActor);
		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
			Get_BoneTransformMatrixWithParents(L"C_Tongue_End");

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"102_SE_Bahamut(FX_MegaFlare_Muzzle_01).wav", ESoundGroup::Narration, 0.5f);
		

		m_pFlareBall->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		m_pFlareBall->Set_BoneName(L"C_Tongue_End");
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(170.f))
	{

		if (m_pFlareBall)
		{
			m_pFlareBall->Set_Independent(true);
		}
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(173.f))
	{
		
		if (!m_TimeChecker.Update(fTimeDelta) && m_bRepeat)
		{
			m_pActor_ModelCom.lock()->AnimationComp()->Set_AnimTrackPos(171);
		}
		else {
			m_bRepeat = false;
		}
	}
}

void CState_Bahamut_MegaFlare::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_MegaFlare::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
	if (eCurPhase == CBahamut::PHASE::PHASE5)
	{
		static_pointer_cast<CBahamut>(m_pActor.lock())->Set_CurPhase(CBahamut::PHASE::PHASE0);
		// 궁극기 이후 페이스 초기화
	}

	m_bRepeat = true;
}

bool CState_Bahamut_MegaFlare::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_MegaFlare::Free()
{
}
