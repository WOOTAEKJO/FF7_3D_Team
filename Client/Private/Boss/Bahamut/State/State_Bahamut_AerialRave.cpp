#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_AerialRave.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "Sound_Manager.h"

CState_Bahamut_AerialRave::CState_Bahamut_AerialRave(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_AerialRave::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMetorDive01_0", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_RUNAWAY);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false); //		중력 관리

	if (!m_pBehaviorTree)
	{
		Create_Trail();

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

void CState_Bahamut_AerialRave::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_AerialRave::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if ((m_iFrame == 0) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_iFrame = 1;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMetorDive01_1", 1.f, false);
	}

	if ((m_iFrame == 1))
	{
		Cur_Player_Look(fTimeDelta,false);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		{
			m_iFrame = 2;
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMetorDive01_2", 1.f, true);
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

			auto pPlyaerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor.lock());
			Activate_Trail(true);
			m_vTargetPos = pPlyaerInfo.vTargetPos;
			// 끝나는 시점에 타겟 위치 저장
		}
	}

	if ((m_iFrame == 2))
	{	
		if (m_pBodyBullet == nullptr)
		{
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_Body"), nullptr, &m_pBodyBullet);
			m_pBodyBullet->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Spine_d");

			m_pBodyBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			m_pBodyBullet->Set_BoneName(L"C_Spine_d");
		}

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 20.f);

		if (!m_pActor_TransformCom.lock()->Go_Target(m_vTargetPos, fTimeDelta, 2.f))
		{
			if (m_pBodyBullet)
			{
				m_pBodyBullet->Set_Dead();
				m_pBodyBullet = nullptr;
			}

			m_iFrame = 3;
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMetorDive01_3", 1.f, false);
			m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true); //		중력 관리

			Activate_Trail(false);
			GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_MateorDive_Hit",m_pActor.lock());
			// 목표 지점에 도착하면

			m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"103_SE_Bahamut (FX_MeteorDive_Exp_01).wav", ESoundGroup::Narration, 0.5f);

			Camera_Shaking("Bahamut_MeteorDive", true);
		}
	}


}

void CState_Bahamut_AerialRave::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_AerialRave::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_iFrame = 0;

	Activate_Trail(false);
}

bool CState_Bahamut_AerialRave::isValid_NextState(CState* state)
{
	if ((m_iFrame == 3) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_Bahamut_AerialRave::Create_Trail()
{
	wstring strBoneNameArray[DIVETRAILNUM] = {
			L"L_WingA_Red",L"L_WingB_Distortion",L"L_WingC_Red",L"L_WingCrow_Distortion",
			L"R_WingA_Distortion",L"R_WingB_Red",L"R_WingC_Distortion",L"R_WingCrow_Red" };

	for (_uint i = 0; i < DIVETRAILNUM; i++)
	{
		if (m_pSpinTrail[i] == nullptr)
		{
			m_pSpinTrail[i] = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(
				L"ET_Bahamut_Spin_" + strBoneNameArray[i], m_pActor.lock());

			if (m_pSpinTrail[i])
			{
				m_pSpinTrail[i]->Trail_Pos_Reset();
				m_pSpinTrail[i]->TurnOff_State(OBJSTATE::Active);
			}
		}
	}
}

void CState_Bahamut_AerialRave::Activate_Trail(_bool bActivate)
{
	for (_uint i = 0; i < DIVETRAILNUM; i++)
	{
		if (m_pSpinTrail[i])
		{
			if (bActivate)
			{
				if (!m_pSpinTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pSpinTrail[i]->TurnOn_State(OBJSTATE::Active);

				}
			}
			else {
				if (m_pSpinTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pSpinTrail[i]->Trail_Pos_Reset();
					m_pSpinTrail[i]->TurnOff_State(OBJSTATE::Active);
				}

			}
		}
	}
}

void CState_Bahamut_AerialRave::Free()
{
}
