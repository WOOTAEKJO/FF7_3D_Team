#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Clorush.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_Clorush::CState_Bahamut_Clorush(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);

	for (_uint i = 0; i < 4; i++)
		m_pCloTrail[i] = nullptr;
}

HRESULT CState_Bahamut_Clorush::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
	
	if ((eCurPhase == CBahamut::PHASE::PHASE0)||
		(eCurPhase == CBahamut::PHASE::PHASE1) || (eCurPhase == CBahamut::PHASE::PHASE2))
	{
		m_strCurAnimTag = "Main|B_AtkScartch01";
	}
	else if ((eCurPhase == CBahamut::PHASE::PHASE3) || (eCurPhase == CBahamut::PHASE::PHASE4) ||
		(eCurPhase == CBahamut::PHASE::PHASE5))
	{
		m_strCurAnimTag = "Main|B_AtkScratch02";
		m_bStrong = true;
	}

	m_pActor_ModelCom.lock()->Set_Animation(m_strCurAnimTag, 1.f, false,
		static_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		Create_Clo();

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

void CState_Bahamut_Clorush::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_Clorush::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	Cur_Player_Look(fTimeDelta);

	_float fMinTPos(0.f), fMaxTPos(0.f);

	if (m_bStrong)
	{
		fMinTPos = 100.f;
		fMaxTPos = 250.f;

		Clo_Anim_Range(110.f, 120.f, CLOLEFT);
		Clo_Anim_Range(125.f, 135.f, CLORIGHT);

		Clo_Anim_Range(140.f, 148.f, CLOLEFT);
		Clo_Anim_Range(150.f, 160.f, CLORIGHT);

		Clo_Anim_Range(165.f, 175.f, CLOLEFT);
		Clo_Anim_Range(180.f, 188.f, CLORIGHT);

		Clo_Anim_Range(190.f, 200.f, CLOLEFT);
		Clo_Anim_Range(210.f, 220.f, CLORIGHT);
		
	}
	else {
		fMinTPos = 40.f;
		fMaxTPos = 70.f;

		Clo_Anim_Range(40.f,50.f, CLORIGHT);
		Clo_Anim_Range(56.f, 70.f, CLOLEFT);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Range(fMinTPos, fMaxTPos))
	{
		if ((m_pCloL))
		{
			if (m_pCloL->IsState(OBJSTATE::WillRemoved))
			{
				m_pCloL = nullptr;
			}
		}

		if((m_pCloR))
		{
			if (m_pCloR->IsState(OBJSTATE::WillRemoved))
			{
				m_pCloR = nullptr;
			}
		}

		if ((m_pCloL == nullptr))
		{
			{
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_Bahamut_Clo"), nullptr, &m_pCloL);
				m_pCloL->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(L"L_Weapon_a");
				m_pCloL->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				m_pCloL->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[2]);
				m_pCloL->Set_BoneName(L"L_Weapon_a");
			}
		}
		if((m_pCloR == nullptr))
		{
			{
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_Bahamut_Clo"), nullptr, &m_pCloR);
				m_pCloR->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(L"R_Weapon_a");
				m_pCloR->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				m_pCloR->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[2]);
				m_pCloR->Set_BoneName(L"R_Weapon_a");
			}
		} // 각각의 팔들에 콜라이더 생성

		auto PlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
		m_pActor_TransformCom.lock()->Go_Target(PlayerInfo.vTargetPos, fTimeDelta, 3.f);
	}
	if(m_pActor_ModelCom.lock()->IsAnimation_UpTo(fMaxTPos + 1.f))
	{
		if ((m_pCloL))
		{
			m_pCloL->Set_Dead();
			m_pCloL = nullptr;
		}
		if((m_pCloR))
		{
			m_pCloR->Set_Dead();
			m_pCloR = nullptr;

		}// 파들 콜라이더 삭제
	}
}

void CState_Bahamut_Clorush::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Clorush::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_bStrong = false;

	Activate_Clo(false, 0);
	Activate_Clo(false, 1);
}

bool CState_Bahamut_Clorush::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Clorush::Create_Clo()
{
	// 0 Right
	// 1 Left

	wstring strBoneNameArray[8] = {
			L"R_Index",L"R_Middle",L"R_Ring",L"R_Thumb",
			L"L_Index",L"L_Middle",L"L_Ring",L"L_Thumb" };

	for (_uint i = 0; i < 8; i++)
	{
		if (m_pCloTrail[i] == nullptr)
		{
			m_pCloTrail[i] = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(
				L"ET_Bahamut_Clo_" + strBoneNameArray[i], m_pActor.lock());
			if (m_pCloTrail[i])
			{
				m_pCloTrail[i]->Trail_Pos_Reset();
				m_pCloTrail[i]->TurnOff_State(OBJSTATE::Active);
			}
		}
	}
}

void CState_Bahamut_Clorush::Activate_Clo(_bool bActivate, _uint iDir)
{
	// 0 Right
	// 1 Left

	_uint iMin(0), iMax(0);

	if (iDir == 0)
	{
		iMin = 0;
		iMax = 4;
	}
	else if (iDir == 1)
	{
		iMin = 4;
		iMax = 8;
	}
	else
		return;

	for (_uint i = iMin; i < iMax; i++)
	{
		if (m_pCloTrail[i])
		{
			if (bActivate)
			{
				if (!m_pCloTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pCloTrail[i]->TurnOn_State(OBJSTATE::Active);
					
				}
			}
			else {
				if (m_pCloTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pCloTrail[i]->Trail_Pos_Reset();
					m_pCloTrail[i]->TurnOff_State(OBJSTATE::Active);
				}
					
			}
		}
	}
}

void CState_Bahamut_Clorush::Clear_Clo()
{
	for (_uint i = 0; i < 8; i++)
	{
		if (m_pCloTrail[i])
		{
			m_pCloTrail[i]->Set_Dead();
			m_pCloTrail[i] = nullptr;
		}
	}
}

void CState_Bahamut_Clorush::Clo_Anim_Range(_float fMin, _float fMax, _uint iDir)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Range(fMin, fMax))
	{
		Activate_Clo(true, iDir);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fMax + 1.f))
	{
		Activate_Clo(false, iDir);
	}
}

void CState_Bahamut_Clorush::Free()
{
}
