#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_DarkClaw.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "Sound_Manager.h"

CState_Bahamut_DarkClaw::CState_Bahamut_DarkClaw(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_DarkClaw::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkKama01", 1.f, false,
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

	m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"095_SE_Bahamut (FX_Kama_Charge_01).wav", ESoundGroup::Narration, 0.5f);

	return S_OK;
}

void CState_Bahamut_DarkClaw::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_DarkClaw::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if(m_pActor_ModelCom.lock()->IsAnimation_Range(0.f,60.f))
		Cur_Player_Look(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(61.f))
	{
		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"096_SE_Bahamut(FX_Kama_Shot_01).wav", ESoundGroup::Narration, 0.5f);

		_float fAngle[4] = { -9.f,-3.f,3.f,9.f };

		for (_uint i = 0; i < 4; ++i)
		{
			shared_ptr<CBahamut_DarkClaw> pBullet = { nullptr };
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_DarkClaw"), nullptr, &pBullet);
			pBullet->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"R_Weapon_a");

			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
			_vector vSocketPos = MuzzleMatrix.r[3];
			vSocketPos.m128_f32[1] = pPlayerInfo.vTargetPos.y;

			_vector vLook = XMVector3Normalize(pPlayerInfo.vTargetPos - vSocketPos);
			
			pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);

			_vector vUp = pBullet->Get_TransformCom().lock()->Get_State(CTransform::STATE_UP);
			pBullet->Get_TransformCom().lock()->Rotation(vUp, XMConvertToRadians(fAngle[i]));

			pBullet->Set_BoneName(L"R_Weapon_a");
		}
	}
}

void CState_Bahamut_DarkClaw::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_DarkClaw::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_DarkClaw::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_DarkClaw::Free()
{
}
