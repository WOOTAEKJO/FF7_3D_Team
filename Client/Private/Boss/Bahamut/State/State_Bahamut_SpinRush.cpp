#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_SpinRush.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "Client_Manager.h"

#include "GameInstance.h"

CState_Bahamut_SpinRush::CState_Bahamut_SpinRush(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_SpinRush::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkSpinAttack01", 1.5f * BAHAMUTANIMSPEED, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		Create_Trail();

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

void CState_Bahamut_SpinRush::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_SpinRush::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Range(5.f, 20.f))
	{
		Cur_Player_Look(fTimeDelta);
		m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * 1.5f);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Range(40.f, 70.f))
	{
		Activate_Trail(true);

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

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 6.f);
	}
	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(71.f))
	{
		Activate_Trail(false);

		if (m_pBodyBullet)
		{
			m_pBodyBullet->Set_Dead();
			m_pBodyBullet = nullptr;
		}
	}

}

void CState_Bahamut_SpinRush::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
}

void CState_Bahamut_SpinRush::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	Activate_Trail(false);
}

bool CState_Bahamut_SpinRush::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_SpinRush::Create_Trail()
{
	wstring strBoneNameArray[SPINTRAILNUM] = {
			L"L_WingA_Red",L"L_WingB_Distortion",L"L_WingC_Red",L"L_WingCrow_Distortion",
			L"R_WingA_Distortion",L"R_WingB_Red",L"R_WingC_Distortion",L"R_WingCrow_Red" };

	for (_uint i = 0; i < SPINTRAILNUM; i++)
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

void CState_Bahamut_SpinRush::Activate_Trail(_bool bActivate)
{
	for (_uint i = 0; i < SPINTRAILNUM; i++)
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

void CState_Bahamut_SpinRush::Free()
{
}
