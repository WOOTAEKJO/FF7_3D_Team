#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Grab.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

#include "State_List_Cloud.h"
#include "Aerith/State/State_List_Aerith.h"
#include "Player.h"

CState_Bahamut_Grab::CState_Bahamut_Grab(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(2.f);
}

HRESULT CState_Bahamut_Grab::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkImpulseZero01_0", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	return S_OK;
}

void CState_Bahamut_Grab::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_Grab::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if ((m_iFrame == 0))
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Range(0.f, 30.f))
		{
			Cur_Player_Look(fTimeDelta);
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
			m_pActor_TransformCom.lock()->Go_Target(pPlayerInfo.vTargetPos, fTimeDelta * 2.f, 3.f);
		}

		if (m_pActor_ModelCom.lock()->IsAnimation_Range(40.f, 49.f))
		{
			if (m_pGrabBullet == nullptr)
			{
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_Bahamut_Grab"), nullptr, &m_pGrabBullet);
				m_pGrabBullet->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(L"R_Weapon_a");
				m_pGrabBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				m_pGrabBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[2]);
				m_pGrabBullet->Set_BoneName(L"R_Weapon_a");
			}

			if (m_pGrabBullet)
			{
				if (m_pGrabBullet->Get_GrabCol())
				{
					m_bGrabSucces = true;
					m_eGrab_Player = m_pGrabBullet->Get_Col_PlayerType();
				}
					
			}
		}

		if (m_bGrabSucces && m_pActor_ModelCom.lock()->IsAnimation_UpTo(50.f))
		{
			if (m_pGrabBullet)
			{
				if (m_eGrab_Player != PLAYER_TYPE::PLAYER_END)
				{
					auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor, m_eGrab_Player);

					if (m_eGrab_Player == Client::CLOUD)
						pPlayerInfo.pTarget.lock()->Get_StateMachineCom().lock()->Set_State<CState_Cloud_Damage_Catch>();
					else
						pPlayerInfo.pTarget.lock()->Get_StateMachineCom().lock()->Set_State<CState_Aerith_Damage_Catch>();

					pPlayerInfo.pTarget.lock()->Get_PhysXControllerCom().lock()->Set_Gravity(false);
				}

				m_pGrabBullet->Set_Dead();
				m_pGrabBullet = nullptr;
			}

			m_iFrame = 1;
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkImpulseZero01_1", 1.f, false);
			// 트렉포지션 확인해서 잡는 애니메이션에 다음 애니메이션으로 바꿔줘야 함
		}
		else if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(50.f))
		{
			if (m_pGrabBullet)
			{
				m_pGrabBullet->Set_Dead();
				m_pGrabBullet = nullptr;
			}
		}
	}

	if (m_iFrame == 1 && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_iFrame = 2;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkImpulseZero01_2", 1.f, true);
	}

	if (m_iFrame == 2)
	{
		if (m_TimeChecker.Update(fTimeDelta))
		{
			m_iFrame = 3;
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkImpulseZero01_3", 1.f, false);
		}
	}

	if (m_iFrame == 3)
	{
		if (m_pFireBall == nullptr)
		{
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_FireBall"), nullptr, &m_pFireBall);
			m_pFireBall->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Tongue_End");
			_matrix MuzzleMatrixTemp = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Tongue_c");

			_vector vLook = XMVector3Normalize(MuzzleMatrix.r[3] - MuzzleMatrixTemp.r[3]);

			m_pFireBall->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			m_pFireBall->Get_TransformCom().lock()->Set_Look_Manual(vLook);
			m_pFireBall->Set_BoneName(L"C_Tongue_End");
		}

		if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(28.f))
		{
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Head_a");
			_matrix MuzzleMatrixTemp = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Neck_d");

			_vector vLook = XMVector3Normalize(MuzzleMatrix.r[3] - MuzzleMatrixTemp.r[3]);

			m_pFireBall->Get_PhysXColliderCom().lock()->WakeUp();
			m_pFireBall->Get_TransformCom().lock()->Set_Look_Manual(vLook);
			m_pFireBall->Set_Independent(true);
		}
	}


	if (m_bGrabSucces)
	{// 바뀌는 걸 생각해야함. -> 스위칭
		for (_uint i = 0; i < 2; i++)
		{
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor.lock(), (PLAYER_TYPE)i);

			auto pCurState = pPlayerInfo.pTarget.lock()->Get_StateMachineCom().lock()->Get_CurState();

			if (typeid(*pCurState) == typeid(CState_Cloud_Damage_Catch) || typeid(*pCurState) == typeid(CState_Aerith_Damage_Catch))
			{
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(L"R_Weapon_a");

				pPlayerInfo.pTarget.lock()->Get_TransformCom().lock()->Set_Position(1.f, MuzzleMatrix.r[3]);
			}
		}
	}
}
void CState_Bahamut_Grab::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Grab::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_iFrame = 0;
	m_bGrabSucces = false;

	m_pFireBall = nullptr;

	m_eGrab_Player = PLAYER_TYPE::PLAYER_END; // 플레이어 타입 초기화
}

bool CState_Bahamut_Grab::isValid_NextState(CState* state)
{
	if (m_bGrabSucces)
	{
		if ((m_iFrame == 3) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return true;
		else
			return false;
	}
	else {
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return true;
		else
			return false;
	}
	
}

void CState_Bahamut_Grab::Free()
{
}
