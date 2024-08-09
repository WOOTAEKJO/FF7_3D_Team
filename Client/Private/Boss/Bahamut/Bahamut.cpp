#include "stdafx.h"
#include "../Public/Boss/Bahamut/Bahamut.h"
#include "../Public/Boss/Bahamut/State_List_Bahamut.h"

#include "GameInstance.h"
#include "Camera_Manager.h"
#include "AnimNotifyCollection_Client.h"
#include "StatusComp.h"

#include "UI_Manager.h"
#include "StatusComp.h"

IMPLEMENT_CREATE(CBahamut)
IMPLEMENT_CLONE(CBahamut, CGameObject)

CBahamut::CBahamut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBoss(pDevice, pContext)
{
}

CBahamut::CBahamut(const CBahamut& rhs)
	:CBoss(rhs)
{
}

HRESULT CBahamut::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CBahamut::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_State()))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;

	m_pTransformCom->Set_Speed(6.f);

	m_TimeChecker = FTimeChecker(0.3f);
	m_TImeChecker_EffectRing = FTimeChecker(0.3f);
	m_TimeChecker_EffectEye = FTimeChecker(0.1f);

	m_iMaxHP = 100;
	m_iCurHP = m_iMaxHP;

	m_vMotionBlur_Offset = _float3(0.f, 3.f, 0.f);

	return S_OK;
}

void CBahamut::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
#if LEVEL_STAGE1_TEST_MAP == 97
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -1.f, 2.f, -1.5f,1.f });

#elif LEVEL_STAGE1_TEST_MAP == 98
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -1.f, 7.f, -1.5f,1.f });
#endif

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -10.f,10.f,12.f,1.f });
	if (m_pModelCom)
	{
		SetUp_Controller(6.f, 1.7f);
		
		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
			PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY, m_pTransformCom, { 2.5f,8.f,2.5f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,5.f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;

		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	// 스탯 설정
//#pragma TODO_MSG(임시 스테이터스 설정. 실제로는 적을 JSON으로 생성할 때 설정해주어야 한다.)
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Boss, "Bahamut");

	GET_SINGLE(CUI_Manager)->Make_MonsterUI("Bahamute", shared_from_this());
}

void CBahamut::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Always_Effect_Step(fTimeDelta);
	Always_Effect_Ring(fTimeDelta);
}

void CBahamut::Late_Tick(_cref_time fTimeDelta)
{		
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_Bahamut_Dead>();
	}

}

void CBahamut::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CBahamut::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);	
}

HRESULT CBahamut::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
	for (_uint i = 0; i < ActiveMeshes.size(); ++i)
	{

		// 뼈 바인딩
		if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
			RETURN_EFAIL;

		// 텍스처 바인딩
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
			RETURN_EFAIL;

		// 패스, 버퍼 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Begin(m_iShaderPassIndex)))
			RETURN_EFAIL;
		//버퍼 렌더
		if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
			RETURN_EFAIL;
	}

	return S_OK;
}

_bool CBahamut::Judge_Dead()
{
	if (Get_StatusCom().lock()->IsZeroHP())
	{
		m_pPhysXColliderCom->PutToSleep(); // 콜라이더 off
		return true;
	}

	return false;
}

HRESULT CBahamut::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	
	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("SU0005_Bahamut"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		
		// 노티파이 프로토타입 클로닝 + 모델 컴포넌트 전달(Weak)
		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_Bahamut"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	Ready_DissovleTex();

	return S_OK;
}
 
HRESULT CBahamut::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_Bahamut_IDLE>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Walk>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Dead>();
	m_pStateMachineCom->Add_State<CState_Bahamut_MegaFlare>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Count>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Grab>();
	m_pStateMachineCom->Add_State<CState_Bahamut_AerialRave>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Aura>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Clorush>();
	m_pStateMachineCom->Add_State<CState_Bahamut_FlareBreath>();
	m_pStateMachineCom->Add_State<CState_Bahamut_SpinRush>();
	m_pStateMachineCom->Add_State<CState_Bahamut_DarkClaw>();
	m_pStateMachineCom->Add_State<CState_Bahamut_HeavyStrike>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Inferno>();
	m_pStateMachineCom->Add_State<CState_Bahamut_Intro>();
	
	m_pStateMachineCom->Set_State<CState_Bahamut_Intro>();

	return S_OK;
}

void CBahamut::Always_Effect_Step(_cref_time fTimeDelta)
{
	if (typeid(*m_pStateMachineCom->Get_CurState()) == typeid(CState_Bahamut_Dead))
		return;

	if (m_TimeChecker.Update(fTimeDelta))
	{
		if ((m_eCurPhase == PHASE::PHASE0))
		{
			if (m_bCrossAlwaysEffect)
			{
				m_bCrossAlwaysEffect = false;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep1",
					L"L_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep1",
					L"R_Foreleg_a", shared_from_this());
			}
			else {
				m_bCrossAlwaysEffect = true;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep1",
					L"R_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep1",
					L"L_Foreleg_a", shared_from_this());
			}
		}
		else if ((m_eCurPhase == PHASE::PHASE1) || (m_eCurPhase == PHASE::PHASE2))
		{
			if (m_bCrossAlwaysEffect)
			{
				m_bCrossAlwaysEffect = false;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep2",
					L"L_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep2",
					L"R_Foreleg_a", shared_from_this());

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep1Splash1",
					L"L_Hand_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep1Splash1",
					L"R_Foot_a", shared_from_this());
			}
			else {
				m_bCrossAlwaysEffect = true;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep2",
					L"R_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep2",
					L"L_Foreleg_a", shared_from_this());

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep1Splash1",
					L"R_Hand_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep1Splash1",
					L"L_Foot_a", shared_from_this());
			}
		}
		else if ((m_eCurPhase == PHASE::PHASE3)||(m_eCurPhase == PHASE::PHASE4) || (m_eCurPhase == PHASE::PHASE5))
		{
			if (m_bCrossAlwaysEffect)
			{
				m_bCrossAlwaysEffect = false;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep3",
					L"L_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep3",
					L"R_Foreleg_a", shared_from_this());

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep2Splash1",
					L"L_Hand_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep2Splash1",
					L"R_Foot_a", shared_from_this());
			}
			else {
				m_bCrossAlwaysEffect = true;

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep3",
					L"R_Forearm_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysBoneStep3",
					L"L_Foreleg_a", shared_from_this());

				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep2Splash1",
					L"R_Hand_a", shared_from_this());
				GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"BahamutAlwaysRingStep2Splash1",
					L"L_Foot_a", shared_from_this());
			}
		}
	}
}

void CBahamut::Always_Effect_Ring(_cref_time fTimeDelta)
{
	if (typeid(*m_pStateMachineCom->Get_CurState()) == typeid(CState_Bahamut_AerialRave) ||
		typeid(*m_pStateMachineCom->Get_CurState()) == typeid(CState_Bahamut_MegaFlare) ||
		typeid(*m_pStateMachineCom->Get_CurState()) == typeid(CState_Bahamut_Dead))
		return;

	if (m_TImeChecker_EffectRing.Update(fTimeDelta))
	{
		if ((m_eCurPhase == PHASE::PHASE1) || (m_eCurPhase == PHASE::PHASE2))
		{
			if (m_iEffectRing_Frame == 0)
			{
				m_iEffectRing_Frame += 1;

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"BahamutAlwaysRingStep1Ring1NonLoop",
					shared_from_this(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 0.5f, 0.f));
			}
			else if (m_iEffectRing_Frame == 1)
			{
				m_iEffectRing_Frame = 0;

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"BahamutAlwaysRingStep1Ring2NonLoop",
					shared_from_this(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 0.5f, 0.f));

			}
		}
		else if ((m_eCurPhase == PHASE::PHASE3)||(m_eCurPhase == PHASE::PHASE4) || (m_eCurPhase == PHASE::PHASE5))
		{
			if (m_iEffectRing_Frame == 0)
			{
				m_iEffectRing_Frame += 1;

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"BahamutAlwaysRingStep2Ring1NonLoop",
					shared_from_this(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 0.5f, 0.f));
			}
			else if (m_iEffectRing_Frame == 1)
			{
				m_iEffectRing_Frame += 1;

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"BahamutAlwaysRingStep2Ring2NonLoop",
					shared_from_this(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 0.5f, 0.f));
			}
			else if (m_iEffectRing_Frame == 2)
			{
				m_iEffectRing_Frame = 0;

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"BahamutAlwaysRingStep2Ring3NonLoop",
					shared_from_this(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 0.5f, 0.f));
			}
		}
	}
}

void CBahamut::Always_Particle_Ring_ON_OFF(_bool bOnOff)
{
	if (bOnOff)
	{
		if (m_pAlwaysRing)
			m_pAlwaysRing->TurnOn_State(OBJSTATE::Active);
	}
	else {
		if (m_pAlwaysRing)
			m_pAlwaysRing->TurnOff_State(OBJSTATE::Active);
	}
}

void CBahamut::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CBahamut::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CBahamut::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CBahamut::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == COLLIDER_LAYER::CL_PLAYER_ATTACK)
	{
		
	}
}

void CBahamut::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut::onControllerHit(CPhysX_Controller* pOtherController, const PxControllersHit& hit)
{
	 //컨트롤끼리 충돌했을 경우
	if (typeid(*m_pStateMachineCom->Get_CurState()) == typeid(CState_Bahamut_SpinRush))
	{
		if (pOtherController->Get_ControllerDesc().iFilterType == CL_PLAYER_CONTROLLER)
		{
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(weak_from_this());

			_vector vForce;
			_float fPower = 3000.f;

			vForce = Convert_Vector(Convert_PxVec3(pPlayerInfo.vDirToTarget)* fPower);

			pOtherController->Add_Force(vForce);
		}
	}

}

void CBahamut::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	// 여기에 대미지 받은 결과에 대한 처리를 할 것.
	if (iAddHitPower == 1)
	{
		auto pAnimComp = m_pModelCom->Get_AnimationComponent();

		pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
	}
}

void CBahamut::Free()
{
	__super::Free();
}