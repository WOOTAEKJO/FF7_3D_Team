#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_RearMachineGun.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_MachineGun.h"
#include "CommonModelComp.h"


#include "GameInstance.h"

CState_AirBurster_RearMachineGun::CState_AirBurster_RearMachineGun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_bTimeCheckerL = FTimeChecker(0.1f);
	m_bTimeCheckerR = FTimeChecker(0.12f);

	m_TimeChecker = FTimeChecker(0.1f);
}

HRESULT CState_AirBurster_RearMachineGun::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMachinegun01", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_GUARD);

	auto pCloudInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor, PLAYER_TYPE::CLOUD);
	auto pAerithInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor, PLAYER_TYPE::AERITH);

	_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
	_vector vCloudLook = pCloudInfo.vDirToTarget;

	if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCloudLook), XMVector3Normalize(vLook))) > 0.f)
	{
		//클라우드가 보스 방향에 있다.
		if (pCloudInfo.pTarget.lock() != nullptr)
		{
			m_vTargetPos = pCloudInfo.vTargetPos;
		}
		else {
			_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
			m_vTargetPos = vPos + XMVector3Normalize(vLook) * NONTARGETDISTANCE;
		}
	}
	else {
		//에어리스가 보스 방향에 있다.
		if (pAerithInfo.pTarget.lock() != nullptr)
		{
			m_vTargetPos = pAerithInfo.vTargetPos;
		}
		else {
			_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
			m_vTargetPos = vPos + XMVector3Normalize(vLook) * NONTARGETDISTANCE;
		}
	}

	if (!m_pBehaviorTree)
	{
		m_vOriginPos = static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_OriginPos();
		// 1페이즈 제자리 위치

		FUNCTION_NODE AnimFinished = FUNCTION_NODE_MAKE {
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			return BT_STATUS::Success;

		return BT_STATUS::Failure;
		};

		FUNCTION_NODE Shoot_Machinegun = FUNCTION_NODE_MAKE {
		if (m_pActor_ModelCom.lock()->IsAnimation_Range(20.f, 40.f))
		{
			if (m_bTimeCheckerL.Update(fTimeDelta))
			{
				// 머신건 생성
				{
					shared_ptr<CAirBurster_MachineGun> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_MachineGun"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_MachineGunRear");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzleP_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(-vLook);
				}
			}
			if (m_bTimeCheckerR.Update(fTimeDelta))
			{
				// 머신건 생성
				{
					shared_ptr<CAirBurster_MachineGun> pBullet = { nullptr };
					m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
						TEXT("Prototype_GameObject_AirBurster_MachineGun"), nullptr, &pBullet);
					pBullet->Set_Owner(m_pActor);
					pBullet->Set_StatusComByOwner("AirBurster_MachineGunRear");
					_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
						Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzleP_a"));
					pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
					_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
					pBullet->Get_TransformCom().lock()->Set_Look_Manual(-vLook);
				}
			}
		}

		return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_Hook
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Hook>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Shoot_Machinegun)
				.leaf<FunctionNode>(AnimFinished)
				.leaf<FunctionNode>(Call_Hook)
			.end()
			.build();
	}

	m_fOriginDistance = XMVectorGetX(XMVector3Length(m_vTargetPos - m_vOriginPos));

	return S_OK;
}

void CState_AirBurster_RearMachineGun::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);

	
}

void CState_AirBurster_RearMachineGun::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vTemPos = XMVectorSetW(m_vTargetPos, 1.f);

	vPos.m128_f32[1] = 0.f;
	vTemPos.m128_f32[1] = 0.f;
	_float fLength = XMVectorGetX(XMVector3Length(vTemPos - vPos)); // 현재 타겟과의 거리

	_float fMaxLength = XMVectorGetX(XMVector3Length(vPos - m_vOriginPos)); // 최대 움직임 길이

	_float fSpeed = pow((m_fOriginDistance/fLength),2) * m_fSpeed; // 타겟에 가까울 수록 스피드가 올라감

	_float fSpeedDelta = m_fAcceleration * fTimeDelta; // 속도 변화량

	shared_ptr<CAirBurster> pAirBurster = DynPtrCast<CAirBurster>(m_pActor.lock());

	if (pAirBurster->Get_PlayerCol())
	{
		pAirBurster->Set_PlayerCol(false);
		m_bArrive = true;
	}
		

	if (!m_bArrive)
	{
		if (fMaxLength < HOOKRUSHMAXLANGTH && fLength >= HOOKRUSHMINLANGTH)
		{
			fSpeed += fSpeedDelta;
			fSpeed = min(max(fSpeed, m_fMinSpeed), m_fMaxSpeed); //최소 속도와 최고 속도를 정함.

			m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * fSpeed);

			
		}
		else
		{
			m_bArrive = true;
		}
			
	}
}

void CState_AirBurster_RearMachineGun::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_RearMachineGun::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_bArrive = false;

	m_bMotionBlur = true;
}

bool CState_AirBurster_RearMachineGun::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished() && m_bArrive)
		return true;
	else
		return false;
}

void CState_AirBurster_RearMachineGun::Free()
{
}
