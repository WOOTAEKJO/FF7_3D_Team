#include "stdafx.h"
#include "Boss/AirBurster/AirBurster.h"
#include "Boss/AirBurster/State_List_AirBurster.h"

#include "Boss/AirBurster/Parts/AirBurster_LeftArm.h"
#include "Boss/AirBurster/Parts/AirBurster_RightArm.h"
#include "Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"

#include "GameInstance.h"
#include "Camera_Manager.h"
#include "AnimNotifyCollection_Client.h"
#include "StatusComp.h"
#include "Common_Shield.h"

#include "UI_Manager.h"

#include "Player.h"

#include "Effect_Manager.h"

IMPLEMENT_CREATE(CAirBurster)
IMPLEMENT_CLONE(CAirBurster, CGameObject)

CAirBurster::CAirBurster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBoss(pDevice, pContext)
{
}

CAirBurster::CAirBurster(const CAirBurster& rhs)
	:CBoss(rhs)
{
}

HRESULT CAirBurster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CAirBurster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_State()))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;

	m_pTransformCom->Set_Speed(1.f);

	m_iMaxHP = 100;
	m_iCurHP = m_iMaxHP;

	m_vMotionBlur_Offset = _float3(0.f, 1.f, 0.f);

	m_ThunderEffect_TimeChecker = FTimeChecker(3.f);

	return S_OK;
}

void CAirBurster::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ 24.1f, 32.5f, 89.2f,1.f });

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom,
			CL_MONSTER_CONTROLLER, 6.f, 2.f,2.f, PxControllerShapeType::eBOX);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);
		
		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
			PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY, m_pTransformCom, { 3.f,5.f,3.f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,2.5f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;

		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	TurnOff_Weapon(L"Part_LeftArm");
	TurnOff_Weapon(L"Part_RightArm");

	// 팔들 콜라이더 off

	// 스탯 설정
#pragma TODO_MSG(임시 스테이터스 설정. 실제로는 적을 JSON으로 생성할 때 설정해주어야 한다.)
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Boss, "AirBurrrrster");

	//에어버스터는 1Phase_Enter에서 생성
}

void CAirBurster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	Judge_Phase();
	cout << m_pStateMachineCom->Get_CurStateName() << endl;
}

void CAirBurster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Thunder_Effect(fTimeDelta);
}

void CAirBurster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_AirBurster_Dead>();
	}
}

void CAirBurster::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CAirBurster::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);

}

HRESULT CAirBurster::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
	for (_uint i = 0; i < ActiveMeshes.size(); ++i)
	{
		if (i == (m_bFlyingMode == true ? HOVER : FLYING))
			continue;

		if (m_bSeparateArm[0])
		{
			if ((i == LEFTARM) || (i == LEFTARMPLATE))
				continue;
		}
		if (m_bSeparateArm[1])
		{
			if ((i == RIGHTARM) || (i == RIGHTARMPLATE))
				continue;
		}

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

_bool CAirBurster::Judge_Phase()
{
	if (m_pStatusCom->Get_HP_Percent() > 0.7f)
	{
		if (m_eCurPhase != AIRBURSTER_PAHSE::PHASE1)
		{
			m_eCurPhase = AIRBURSTER_PAHSE::PHASE1;
			cout << "Airburster phase1" << endl;
			return m_bChangePhase = true;
		}
	}
	else if (0.f < m_pStatusCom->Get_HP_Percent() && m_pStatusCom->Get_HP_Percent() <= 0.7f)
	{
		if (m_eCurPhase != AIRBURSTER_PAHSE::PHASE2)
		{
			m_eCurPhase = AIRBURSTER_PAHSE::PHASE2;
			cout << "Airburster phase2" << endl;
			return m_bChangePhase = true;
		}
	}
	else if (m_pStatusCom->Get_HP_Percent() <= 0.f)
	{
		if (m_eCurPhase != AIRBURSTER_PAHSE::PHASE3)
		{
			m_eCurPhase = AIRBURSTER_PAHSE::PHASE3;
			cout << "Airburster phase3" << endl;
			return m_bChangePhase = true;
		}
	}
	else 
		return false;

	return false;
}

_bool CAirBurster::Judge_Docking()
{
	if (!m_bSeparateArm || m_PartObjects.empty())
		return false;

	_uint iSize = 0;

	for (auto& iter : m_PartObjects)
	{
		if (Compare_Wstr(iter.first, TEXT("Part_LeftArm")) ||
			Compare_Wstr(iter.first, TEXT("Part_RightArm")))
		{
			if (static_pointer_cast<CAirBurster_Parts>(iter.second)->Get_PartsDead())
				++iSize;
			
		}
	}
	// 모든 팔이 죽었는지 판단 유무

	return iSize == 2;
}

void CAirBurster::Ready_Docking()
{
	if (m_PartObjects.empty())
		return;

	for (auto& iter : m_PartObjects)
	{
		if (Compare_Wstr(iter.first, TEXT("Part_LeftArm")) ||
			Compare_Wstr(iter.first, TEXT("Part_RightArm")))
		{
			static_pointer_cast<CAirBurster_Parts>(iter.second)->Set_ReadyDocking(true);
		}
	}
	// 팔들한테 도킹 준비 완료 신호
}

_bool CAirBurster::Judge_Combination(_int* iType)
{
	if (!m_bSeparateArm || m_PartObjects.empty())
		return false;

	for (auto& iter : m_PartObjects)
	{
		if (static_pointer_cast<CAirBurster_Parts>(iter.second)->Get_Docking())
		{
			if (Compare_Wstr(iter.first, TEXT("Part_LeftArm")))
			{
				*iType = 0;
				static_pointer_cast<CAirBurster_Parts>(iter.second)->Set_Docking(false);
			}

			if (Compare_Wstr(iter.first, TEXT("Part_RightArm")))
			{
				*iType = 1;
				static_pointer_cast<CAirBurster_Parts>(iter.second)->Set_Docking(false);
			}

			return true;
		}
	}

	return false;
}

void CAirBurster::Commnad_Cannon(_bool bCheck)
{
	if (!m_bSeparateArm || m_PartObjects.empty())
		return; // 팔이 독립 상태가 아니거나 파츠 리스트가 비었으면 리턴

	for (auto& iter : m_PartObjects)
	{
		if(Compare_Wstr(iter.first, TEXT("Part_LeftArm")) || 
			Compare_Wstr(iter.first, TEXT("Part_RightArm")))
		{
			if (bCheck)
			{
				if (!static_pointer_cast<CAirBurster_Parts>(iter.second)->Get_Cannon())
					static_pointer_cast<CAirBurster_Parts>(iter.second)->Set_Cannon(true);
			}
			else {
				if (static_pointer_cast<CAirBurster_Parts>(iter.second)->Get_Cannon())
					static_pointer_cast<CAirBurster_Parts>(iter.second)->Set_Cannon(false);
			}
		}
	}

	// 두 팔한테 캐논 명령어 활성화 및 비활성화
}

void CAirBurster::Thunder_Effect(_cref_time fTimeDelta)
{
	auto pCurState = m_pStateMachineCom->Get_CurState();

	if ((typeid(*pCurState) == typeid(CState_AirBurster_Intro)) || (typeid(*pCurState) == typeid(CState_AirBurster_1PhaseEnter)) ||
		(typeid(*pCurState) == typeid(CState_AirBurster_2PhaseEnter)) || (typeid(*pCurState) == typeid(CState_AirBurster_Dead)))
		return;

	if (m_ThunderEffect_TimeChecker.Update(fTimeDelta))
	{
		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"AirBurster_Always_Thunder1", shared_from_this());
	}
}

HRESULT CAirBurster::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	
	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EB0002_AirBurster"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		
		// 노티파이 프로토타입 클로닝 + 모델 컴포넌트 전달(Weak)
		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_AirBuster"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	Ready_DissovleTex();

	return S_OK;
}
 
HRESULT CAirBurster::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();

	/* For.Part_LeftArm*/
	Desc.strBoneName = TEXT("L_Forearm_a");
	if (FAILED(Add_PartObject<CAirBurster_LeftArm>(TEXT("Prototype_GameObject_AirBurster_LeftArm"), TEXT("Part_LeftArm"), &Desc, nullptr)))
		RETURN_EFAIL;

	/* For.Part_RightArm*/
	Desc.strBoneName = TEXT("R_Forearm_a");
	if (FAILED(Add_PartObject<CAirBurster_RightArm>(TEXT("Prototype_GameObject_AirBurster_RightArm"), TEXT("Part_RightArm"), &Desc, nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_AirBurster_IDLE>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Transform>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Control_Phase1>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Control_Phase2>();

	m_pStateMachineCom->Add_State<CState_AirBurster_FrontMachineGun>();
	m_pStateMachineCom->Add_State<CState_AirBurster_RearMachineGun>();
	m_pStateMachineCom->Add_State<CState_AirBurster_EnergyBall>();
	m_pStateMachineCom->Add_State<CState_AirBurster_FingerBeam>();
	m_pStateMachineCom->Add_State<CState_AirBurster_EMField>();

	m_pStateMachineCom->Add_State<CState_AirBurster_TankBurster>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Burner>();
	m_pStateMachineCom->Add_State<CState_AirBurster_ShoulderBeam>();
	m_pStateMachineCom->Add_State<CState_AirBurster_RocketArm>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Docking>();

	m_pStateMachineCom->Add_State<CState_AirBurster_Intro>();
	m_pStateMachineCom->Add_State<CState_AirBurster_2PhaseEnter>();
	m_pStateMachineCom->Add_State<CState_AirBurster_1PhaseEnter>();

	m_pStateMachineCom->Add_State<CState_AirBurster_Dead>();

	m_pStateMachineCom->Add_State<CState_AirBurster_Turn180>();

	m_pStateMachineCom->Add_State<CState_AirBurster_Hook>();
	m_pStateMachineCom->Add_State<CState_AirBurster_Hook_Return>();

	m_pStateMachineCom->Add_State<CState_AirBurster_Burst>();

	m_pStateMachineCom->Set_State<CState_AirBurster_Intro>();

	m_eCurPhase = AIRBURSTER_PAHSE::PHASE2;
	m_pStatusCom->Set_CurHP(7000.f);

	return S_OK;
}

void CAirBurster::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CAirBurster::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CAirBurster::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CAirBurster::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster::onControllerHit(CPhysX_Controller* pOtherController, const PxControllersHit& hit)
{
	// 컨트롤끼리 충돌했을 경우
	if (pOtherController->Get_ControllerDesc().iFilterType == CL_PLAYER_CONTROLLER)
	{
		CState* pCurState = m_pStateMachineCom->Get_CurState();
		if (typeid(*pCurState) == typeid(CState_AirBurster_RearMachineGun))
		{
			if (!m_bPlayerCol)
				m_bPlayerCol = true;
		}

		if (typeid(*pCurState) == typeid(CState_AirBurster_Hook_Return)|| typeid(*pCurState) == typeid(CState_AirBurster_RearMachineGun))
		{

			pOtherController->Get_Owner().lock()->Get_TransformCom().lock()->Add_Position(XMVectorSetW(m_vMovement_Amount * 2.f, 1.f));
			// 이동량을 더함.
			// 서로 반대 방향을 보고 있으면.
		}
		else {
			_vector vOtherLook = XMVector3Normalize(pOtherController->Get_Owner().lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK));

			pOtherController->Get_Owner().lock()->Get_TransformCom().lock()->Add_Position(XMVectorSetW((-vOtherLook) * 2.f, 1.f));
		}
	}
}

void CAirBurster::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iStatusDamaged & ECast(EStatusModified::Block))
	{
		shared_ptr<CCommon_Shield> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_Common_Shield"), nullptr, &pBullet);
		pBullet->Set_Owner(shared_from_this());
		pBullet->Get_TransformCom().lock()->Set_Scaling(3.f, 3.f, 3.f);
		DynPtrCast<CCommon_Shield>(pBullet)->Set_Offset({ 0.f, 1.f, 0.f });
	}
	else if (iStatusDamaged & ECast(EStatusModified::Damage))
	{
		if (iAddHitPower == 1)
		{
			auto pAnimComp = m_pModelCom->Get_AnimationComponent();

			pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
		}
	}
}

void CAirBurster::Set_SeparateArm(wstring strPartsName,_bool bCheck)
{
	if (m_PartObjects.empty())
		return;

	_uint iNum = 0;

	if (Compare_Wstr(strPartsName, TEXT("Part_LeftArm")))
		iNum = 0;
	else if (Compare_Wstr(strPartsName, TEXT("Part_RightArm")))
		iNum = 1;
	else
		return;

	if (bCheck)
	{
		static_pointer_cast<CAirBurster_Parts>(m_PartObjects.find(strPartsName)->second)
			->Set_Independent(true);
	}
	else
	{
		static_pointer_cast<CAirBurster_Parts>(m_PartObjects.find(strPartsName)->second)
			->Set_Independent(false);
		static_pointer_cast<CAirBurster_Parts>(m_PartObjects.find(strPartsName)->second)
			->Set_ReadyDocking(false);
	}

	m_bSeparateArm[iNum] = bCheck;

	

}

_bool CAirBurster::Get_SeparateArm(wstring strPartsName)
{
	_bool bCheck;

	if (Compare_Wstr(strPartsName, TEXT("Part_LeftArm")))
		bCheck = m_bSeparateArm[0];
	else if (Compare_Wstr(strPartsName, TEXT("Part_RightArm")))
		bCheck = m_bSeparateArm[1];
	else
		return false;

	// 분리 상태 유무

	return bCheck;
}

_bool CAirBurster::Is_All_SeparateArm()
{
	if (m_bSeparateArm[0] && m_bSeparateArm[1])
		return true;

	return false;
}

void CAirBurster::OnOff_Block(_bool bOnOff)
{
	if (bOnOff)
	{
		if (m_pBlock == nullptr)
		{
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_AirBurster_Block"), nullptr, &m_pBlock);
			m_pBlock->Set_Owner(weak_from_this());
		}
	}
	else
	{
		if (m_pBlock)
		{
			m_pBlock->Set_Dead();
			m_pBlock = nullptr;
		}
	}
}

void CAirBurster::Free()
{
	__super::Free();
}