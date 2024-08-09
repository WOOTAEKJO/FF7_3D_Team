#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Aura.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

#include "Camera_Manager.h"
#include "Sound_Manager.h"

CState_Bahamut_Aura::CState_Bahamut_Aura(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_Aura::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkAura01", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

	m_iCurPhase = (_uint)static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
	if (m_iCurPhase != (_uint)CBahamut::PHASE::PHASE5)
	{
		m_iCurPhase = ((_int)m_iCurPhase + 1);

		static_pointer_cast<CBahamut>(m_pActor.lock())->Set_CurPhase((CBahamut::PHASE)m_iCurPhase);
		// 현재 페이스에서 다음 페이스로
		// 5페이스면 그냥 놔둔다.
		// 궁극기에서 초기화 예정
		static_pointer_cast<CBahamut>(m_pActor.lock())->Set_EffectRing_Frame(0);
	}

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
	
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT_AURA);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT_AURA);

	
	
	return S_OK;
}

void CState_Bahamut_Aura::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

	TurnOnOff_MotionBlur(37.f, 77.f, true);
	Camera_Shaking("Bahamut_Aura", 37.f, 77.f);
}

void CState_Bahamut_Aura::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(35.f))
	{
		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"003_SE_Bahamut (B_AtkAura01_voice).wav", ESoundGroup::Narration, 1.f);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Range(40.f, 85.f))
	{
		_float4 fLightDiffuse;

		

		if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(40.f))
		{
			

			if ((m_iCurPhase == (_uint)CBahamut::PHASE::PHASE0) || (m_iCurPhase == (_uint)CBahamut::PHASE::PHASE1)
				|| (m_iCurPhase == (_uint)CBahamut::PHASE::PHASE2))
			{
				GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_Aura_New1_1", m_pActor.lock(),
					CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.361f, 6.f, 0.f));

				fLightDiffuse = _float4(0.272f, 0.084f, 0.615f, 1.f);
			}
			else if ((m_iCurPhase == (_uint)CBahamut::PHASE::PHASE3)||
				(m_iCurPhase == (_uint)CBahamut::PHASE::PHASE4) || (m_iCurPhase == (_uint)CBahamut::PHASE::PHASE5))
			{
				GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_Aura_New2_1", m_pActor.lock(),
					CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.361f, 6.f, 0.f));

				fLightDiffuse = _float4(0.615, 0.084, 0.084, 1.f);
			}
		}

		if (m_pLight == nullptr && m_bLight)
		{
			// 0.615, 0.084, 0.084 레드
			// 0.272f, 0.084f, 0.615f 퍼플

			m_pLight = m_pGameInstance->Make_Light_On_Owner(m_pActor, fLightDiffuse, 10.f, _float3(0.f, 5.f, 0.f));
			m_bLight = false;
		}
		else if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(77.f))
		{
			if (m_pLight)
			{
				m_pGameInstance->Release_Light(m_pLight);
				m_pLight = nullptr;
			}
		}

		if (m_pAuraBullet == nullptr)
		{
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_Bahamut_Aura"), nullptr, &m_pAuraBullet);
			m_pAuraBullet->Set_Owner(m_pActor);
			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(L"C_Spine_d");

			m_pAuraBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			m_pAuraBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[2]);
			m_pAuraBullet->Set_BoneName(L"C_Spine_d");
		}
	}
	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(86.f))
	{
		if (m_pAuraBullet)
		{
			m_pAuraBullet->Set_Dead();
			m_pAuraBullet = nullptr;
		}
	}
}

void CState_Bahamut_Aura::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Aura::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_bLight = true;

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_BAHAMUT);
}

bool CState_Bahamut_Aura::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Aura::Free()
{
}
