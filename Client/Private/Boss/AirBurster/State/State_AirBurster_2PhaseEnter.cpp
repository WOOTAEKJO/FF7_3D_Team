#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_2PhaseEnter.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"

#include "camera.h"
#include "Camera_Manager.h"

CState_AirBurster_2PhaseEnter::CState_AirBurster_2PhaseEnter(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_2PhaseEnter::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	_vector vLook = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

	string strCutSceneTag;

	if (XMVectorGetX(XMVector3Dot(vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f))) < 0.f)
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkP2L01", 1.f * AIRBURSTERANIMSPEED, false);
		m_iTurnType = 0;
		strCutSceneTag = "Airburster_2phaseEnterL_Following";
	}
	else {
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkP2R01", 1.f * AIRBURSTERANIMSPEED, false);
		m_iTurnType = 1;
		strCutSceneTag = "Airburster_2phaseEnterR_Following";
	}

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{
		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// 콜라이더 off

		m_fStartPosX = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION).m128_f32[0];

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Wait_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	m_vCurPlayerPos[CLOUD] = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor,CLOUD).vTargetPos;
	m_vCurPlayerPos[AERITH] = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor, AERITH).vTargetPos;

	DynPtrCast<CAirBurster>(m_pActor.lock())->OnOff_Block(false);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera(strCutSceneTag, (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE,m_pActor.lock());

	return S_OK;
}

void CState_AirBurster_2PhaseEnter::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

	GET_SINGLE(CClient_Manager)->Set_PlayerPos(m_vCurPlayerPos[CLOUD], m_vCurPlayerPos[AERITH]);
}

void CState_AirBurster_2PhaseEnter::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	

	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(90.f))
	{	

		_float4 fCurPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

		if (m_fStartPosX + m_fMovement_amount <= fCurPos.x)
		{
			if (m_bMove)
			{
				m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
				m_bMove = false;
			}
		}
	}
}

void CState_AirBurster_2PhaseEnter::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_2PhaseEnter::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_ChangePhase(false);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(false);
	if(m_iTurnType == 0)
		m_pActor_TransformCom.lock()->Rotation(_float4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	else if(m_iTurnType == 1)
		m_pActor_TransformCom.lock()->Rotation(_float4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	m_fMovement_amount = 15.f;
	m_fStartPosX = 0.f;

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	m_pActor.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	// 콜라이더 On
}

bool CState_AirBurster_2PhaseEnter::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_2PhaseEnter::Free()
{
}
