#include "stdafx.h"
#include "Effect_Group.h"

#include "GameInstance.h"
#include "Effect.h"

#include "Particle.h"
#include "Sprite.h"
#include "Trail_Buffer.h"
#include "Trail_Effect.h"

#include "PartObject.h"

#include "BoneContainer.h"

IMPLEMENT_CLONE(CEffect_Group, CGameObject)
IMPLEMENT_CREATE(CEffect_Group)
IMPLEMENT_CREATE_EX1(CEffect_Group, const string&, strFilePath)

CEffect_Group::CEffect_Group(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{
	
}

CEffect_Group::CEffect_Group(const CEffect_Group& rhs)
	: CEffect(rhs), m_vecGroup(rhs.m_vecGroup), m_eOwnerType(rhs.m_eOwnerType)
{
}

HRESULT CEffect_Group::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Group::Initialize_Prototype(string strFilePath)
{
	json In_Json;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	return S_OK;
}

HRESULT CEffect_Group::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;	

	if (m_bLoad)
	{
		Create_Effect();
	}

	return S_OK;
}

void CEffect_Group::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	if(m_pGameInstance->Get_CreateLevelIndex() == LEVEL_TOOL) {
		Tool_Create_Effect();
		// 툴에서 사용 -> 툴에서 로드가 아닌 먼저 불러오는 경우를 생각해야 함.
	}

	Connection_Owner(); // 오너 연결
}

void CEffect_Group::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	Update_Effect(fTimeDelta);

	if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{// 툴에서만 사용 가능
		for (auto iter : m_vecGroup)
		{
			if (!(iter).pEffect->Get_IsEffectDead() && iter.pEffect->IsState(OBJSTATE::Active))
			{
				if (iter.pEffect->IsState(OBJSTATE::Begin_Play))
					iter.pEffect->Begin_Play(fTimeDelta);

				iter.pEffect->Priority_Tick(fTimeDelta);
			}
		} // 매니저를 대신해서 이펙트들을 관리 해준다.
	}
}

void CEffect_Group::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{
		for (auto iter : m_vecGroup)
		{
			if (!(iter).pEffect->Get_IsEffectDead() && iter.pEffect->IsState(OBJSTATE::Active))
				iter.pEffect->Tick(fTimeDelta);
		}
	}
}

void CEffect_Group::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{
		for (auto iter : m_vecGroup)
		{
			if (!(iter).pEffect->Get_IsEffectDead() && iter.pEffect->IsState(OBJSTATE::Active))
				iter.pEffect->Late_Tick(fTimeDelta);
		}
	}

	Judge_Dead(); // 죽음 조건 판단
}

void CEffect_Group::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (m_bParticleDead)
		return;

	if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{
		for (auto iter : m_vecGroup)
		{
			if (!(iter).pEffect->Get_IsEffectDead() && iter.pEffect->IsState(OBJSTATE::Active))
				iter.pEffect->Before_Render(fTimeDelta);
		}
	}

	Update_GroupEffect_Matrix();	// 월드행렬 계산식
}

HRESULT CEffect_Group::Render()
{
	return S_OK;
}

HRESULT CEffect_Group::Add_Effect(EFFECT_TYPE eType, const string& strFilePath, weak_ptr<CEffect> pEffect)
{
	// 툴 전용
	if (pEffect.lock() == nullptr)
		RETURN_EFAIL;

	EFFECT_GROUP_DESC Desc = {};
	Desc.pEffect = pEffect.lock();
	Desc.pEffect->TurnOff_State(OBJSTATE::Active);
	Desc.eEffectType = eType;
	Desc.bEnd = false;
	Desc.strFilePath = strFilePath;

	vector<string> vectemp;
	vectemp = SplitStr(Desc.strFilePath, '\\');
	vectemp = SplitStr(vectemp[vectemp.size() - 1], '.');
	Desc.strFileName = vectemp.front();

	m_vecGroup.push_back(Desc);

	return S_OK;
}
void CEffect_Group::Delete_Effect(_uint iIndex)
{
	// 툴 전용
	if (m_vecGroup.empty())
		return;

	if (iIndex >= m_vecGroup.size())
		return;

	m_vecGroup[iIndex].pEffect->TurnOn_State(OBJSTATE::WillRemoved);

	m_vecGroup.erase(m_vecGroup.begin() + iIndex);
}
void CEffect_Group::Reset_Group()
{
	// 툴 전용
	if (m_vecGroup.empty())
		return;

	for (auto& iter : m_vecGroup)
		iter.pEffect->TurnOn_State(OBJSTATE::WillRemoved);
	m_vecGroup.clear();
}
void CEffect_Group::Update_Effect(_cref_time fTimeDelta) 
{
	if (m_bUpdate)
	{
		m_fTimeDelta += fTimeDelta;

		for (auto& iter : m_vecGroup)
		{
			if (iter.fStartTime <= m_fTimeDelta)
			{
				if (!iter.pEffect->Get_IsEffectDead())
				{
					if (!(iter.pEffect->IsState(OBJSTATE::Active)))
					{
						iter.pEffect->TurnOn_State(OBJSTATE::Active);
						iter.pEffect->Set_Owner(Get_Owner());
					}
						
					if (!(iter.pEffect->IsState(OBJSTATE::Tick)))
						iter.pEffect->TurnOn_State(OBJSTATE::Tick);
					// 기준 시간 값에 따라 활성화 시킨다.
				}
			}
		}
	}
	else {
		for (auto& iter : m_vecGroup)
		{
			if (!iter.pEffect->Get_IsEffectDead())
			{
				if ((iter.pEffect->IsState(OBJSTATE::Tick)))
					iter.pEffect->TurnOff_State(OBJSTATE::Tick);
			}
			
		}
	}
}

void CEffect_Group::Reset_Effect(_bool bActivate)
{
	for (auto& iter : m_vecGroup)
	{
		iter.pEffect->Reset_Effect(bActivate);
		// 리셋만 시키면 그룹 객체가 업데이트에서 순차적으로 활성화 시킴.

		if (iter.pEffect->IsState(OBJSTATE::Active))
		{
			iter.pEffect->TurnOff_State(OBJSTATE::Active);
			// 툴에서 이펙트 객체가 삭제 처리가 안된 상태에서 리셋을 시켰을 때.
		}
	}
	//
	m_fTimeDelta = 0.f;
	m_bParticleDead = false;
}

void CEffect_Group::Replicate_Effect()
{
	// 툴 전용

	if (m_vecGroup.empty())
		return;

	vector<EFFECT_GROUP_DESC> vecTemp = m_vecGroup;

	for (auto& iter : vecTemp)
	{
		EFFECT_GROUP_DESC Desc = {};
		Desc.eEffectType = iter.eEffectType;
		Desc.bEnd = iter.bEnd;
		Desc.strFilePath = iter.strFilePath;
		Desc.strFileName = iter.strFileName;
		Desc.fStartTime = iter.fStartTime;

		switch (Desc.eEffectType)
		{
		case EFFECT_TYPE::EFFECT_PARTICLE:
		{
			Desc.pEffect = CObjPool_Manager::GetInstance()->Create_Object<CParticle>(
				StrToWstr(Desc.strFileName), (LAYERTYPE)m_pGameInstance->Get_CreateLevelIndex());
		}
		break;
		case EFFECT_TYPE::EFFECT_RECT:
		{
			Desc.pEffect = CObjPool_Manager::GetInstance()->Create_Object<CSprite>(
				StrToWstr(Desc.strFileName), (LAYERTYPE)m_pGameInstance->Get_CreateLevelIndex());
		}
		break;
		case EFFECT_TYPE::EFFECT_MESH:
		{
			Desc.pEffect = CObjPool_Manager::GetInstance()->Create_Object<CTrail_Effect>(
				StrToWstr(Desc.strFileName), (LAYERTYPE)m_pGameInstance->Get_CreateLevelIndex());
		}
		break;
		case EFFECT_TYPE::EFFECT_TRAIL:
		{
			Desc.pEffect = CObjPool_Manager::GetInstance()->Create_Object<CTrail_Buffer>(
				StrToWstr(Desc.strFileName), (LAYERTYPE)m_pGameInstance->Get_CreateLevelIndex());
		}
		break;
		}

		_float4x4 matWorld = iter.pEffect->Get_TransformCom().lock()
			->Get_WorldFloat4x4();

		matWorld.m[3][0] += 1.f;
		matWorld.m[3][1] += 0.f;
		matWorld.m[3][2] += 1.f;

		Desc.pEffect->Get_TransformCom().lock()->Set_WorldFloat4x4(matWorld);

		Desc.pEffect->TurnOff_State(OBJSTATE::Active);

		m_vecGroup.push_back(Desc);
	}
}

void CEffect_Group::All_Add_Time(_float fTime)
{
	// 툴 전용

	if (m_vecGroup.empty())
		return;

	for (auto& iter : m_vecGroup)
	{
		iter.fStartTime += fTime;
	}
}

// 툴 전용

void CEffect_Group::Create_Effect()
{
	if (!m_bLoad || m_vecGroup.empty())
		return;

	for (auto& iter : m_vecGroup)
	{
		if (iter.pEffect == nullptr)
		{
			iter.pEffect = static_pointer_cast<CEffect>(m_pGameInstance->CloneObject(StrToWstr(iter.strFileName), nullptr));
			// 오브젝트 매니저에서 관리하는 것이 아닌 그룹 객체가 관리하기 위함.

			if (iter.pEffect)
			{
				iter.pEffect->Get_TransformCom().lock()->Set_WorldFloat4x4(iter.matWorld);
				iter.pEffect->TurnOff_State(OBJSTATE::Active);
				iter.pEffect->Set_IsInGroupEffect(true);
			}
			// 처음 생성되면 비활성화 및 리셋 -> 이펙트들을 시간 기준으로 차례대로 실행시키기 위함
		}
	}

	m_fTimeDelta = 0.f; // 기준 시간 값 초기화
}

void CEffect_Group::Tool_Create_Effect()
{ // 툴 전용 -> 오브젝트 풀이 아닌 프로토타입으로 생성한다.
	if (!m_bLoad || m_vecGroup.empty())
		return;

	for (auto& iter : m_vecGroup)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
			StrToWstr(iter.strFileName), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&iter.pEffect))))
			return;

		iter.pEffect->Get_TransformCom().lock()->Set_WorldFloat4x4(iter.matWorld);
		iter.pEffect->TurnOff_State(OBJSTATE::Active);
		iter.pEffect->Set_IsInGroupEffect(true);
		// 처음 생성되면 비활성화 및 리셋 -> 이펙트들을 시간 기준으로 차례대로 실행시키기 위함
		// 툴이면 다시 만들어준다.
	}

	m_fTimeDelta = 0.f; // 기준 시간 값 초기화
	// 툴 상에서 데이터를 다시 가져오기 위한 구조에 맞춰짐
}

void CEffect_Group::Judge_Dead()
{
	if (!m_bLoad) // 파일 데이터를 불러오기가 아니면 리턴
		return;
	
	_bool bDead = true;
	for (auto iter = m_vecGroup.begin(); iter != m_vecGroup.end();)
	{		
		if (!(*iter).pEffect->Get_IsEffectDead())
			bDead = false;
		// 그룹 객체의 속한 이펙트들은 알아서 비활성화 시킴.
		iter++;
	}

	if (bDead)
	{
		m_bParticleDead = true;

		if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
			Set_Dead();
	}
	// 모든 이펙트들이 죽으면 그룹 객체 죽음
	// 오브젝트 풀로 돌아감.
}

void CEffect_Group::Update_GroupEffect_Matrix()
{
	// 그룹 객체의 월드행렬을 따라간다.
				// 그룹 또한 오너를 가질 수 있으며, 타입에 따라 계산식이 달라져야 한다.
	if (m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{
		if (m_eOwnerType == CEffect::USE_TYPE::USE_NONE)
		{	
			
			m_matWorld = m_pTransformCom->Get_WorldFloat4x4() * Get_MatrixNormalize(m_matOneTImeWorld);

			// 단발성 이펙트들은 만들어 질 때, 오너의 행렬을 저장해 와서 계산해준다.
			// 행렬은 변하지 않는 정적인 행렬이다.

			// 그룹에 속한 이펙트들은 오너가 없는 경우이다.
			// 초기 위치만 잡아주면 되기 때문에 이펙트 매니저를 통해서 얻은 m_matWorld 행렬을 적용
			// m_matWorld 갱신할 필요가 없음.
		}
		else if (m_eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_NORMAL)
		{
			if (m_pOwner.lock() == nullptr || m_pOwnerTransformCom.lock() == nullptr)
				return;

			if (m_pSocketBoneGroup)
			{
				m_matWorld = m_pTransformCom->Get_WorldFloat4x4()
					* Get_MatrixNormalize(m_pSocketBoneGroup->CRef_BoneCombinedTransforms()[m_iSocketBoneIndex])
					* Get_MatrixNormalize(m_pOwnerTransformCom.lock()->Get_WorldMatrix());
			}
			else {
				m_matWorld = m_pTransformCom->Get_WorldMatrix() *
					Get_MatrixNormalize(m_pOwnerTransformCom.lock()->Get_WorldFloat4x4());
			}
			// 일반적인 객체 계산식 행렬 적용
		}
		else if (m_eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
		{
			if (m_pOwner.lock() == nullptr)
				return;

			if (m_pSocketBoneGroup)
			{
				m_matWorld = m_pTransformCom->Get_WorldFloat4x4()
					* Get_MatrixNormalize(m_pSocketBoneGroup->CRef_BoneCombinedTransforms()[m_iSocketBoneIndex])
					* Get_MatrixNormalize(static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix());
			}
			else {
				m_matWorld = m_pTransformCom->Get_WorldMatrix() *
					Get_MatrixNormalize(static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix());
			}
			// 파츠 오브젝트 객체 계산식 행렬 적용

		}
		else if (m_eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_EFFECT)
		{
			if (m_pOwner.lock() == nullptr)
				return;

			m_matWorld = m_pTransformCom->Get_WorldMatrix() *
				Get_MatrixNormalize(static_pointer_cast<CBlendObject>(m_pOwner.lock())->Get_matWorld());
			// 이펙트 객체 계산식 행렬 적용.
		}
	}
	else {
		m_matWorld = m_pTransformCom->Get_WorldMatrix();
		// 툴 레벨 전용
	}

	if (!m_vecGroup.empty())
	{
		for (auto& iter : m_vecGroup)
			if (iter.pEffect != nullptr)
			{

				iter.pEffect->Set_matWorld(iter.pEffect->Get_TransformCom().lock()->Get_WorldMatrix()
					* m_matWorld);
				//그룹 이펙트도 월드행렬 변수에 계산식을 채워 넣어 줘야 한다.
				// 하위 객체의 로컬 영역에서 그룹 객체의 행렬을 곱해준다.
			}
	}
}

void CEffect_Group::Write_Json(json& Out_Json)
{
	if (m_vecGroup.empty())
		return;

	Out_Json["OwnerType"] = (_int)m_eOwnerType;
	Out_Json["iObjPool_MaxNum"] = m_iObjPool_MaxNum;

	_uint iSize = m_vecGroup.size();
	for (_uint i = 0; i < iSize; i++)
	{
		Write_Data(Out_Json["Data"][i],i);
	}
}

void CEffect_Group::Connection_Owner()
{
	if (m_vecGroup.empty())
		return;

	for (auto& iter : m_vecGroup)
	{
		iter.pEffect->Set_Owner(Get_Owner());
	}
}

void CEffect_Group::Reset_Prototype_Data()
{
	json In_Json;
	string strFilePath = WstrToStr(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, Get_PrototypeTag()));

	if (Compare_Str(strFilePath, "Not_Find"))
		return;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;
}

void CEffect_Group::Load_Json(const json& In_Json)
{
	m_vecGroup.clear();

	m_eOwnerType = (CEffect::USE_TYPE)In_Json["OwnerType"];
	if (In_Json.contains("iObjPool_MaxNum"))
		m_iObjPool_MaxNum = In_Json["iObjPool_MaxNum"];

	_uint iIndex = 0;
	for (auto& iter : In_Json["Data"])
	{
		Load_Data(iter,iIndex);
	}
	
}

void CEffect_Group::Write_Data(json& Out_Json, _uint iIndex)
{
	Out_Json["bEnd"] = m_vecGroup[iIndex].bEnd;
	Out_Json["eEffectType"] = (_int)m_vecGroup[iIndex].eEffectType;
	Out_Json["strFilePath"] = m_vecGroup[iIndex].strFilePath;
	Out_Json["strFileName"] = m_vecGroup[iIndex].strFileName;
	Out_Json["fStartTime"] = m_vecGroup[iIndex].fStartTime;

	_matrix matWorld = m_vecGroup[iIndex].pEffect->Get_TransformCom().lock()->Get_WorldFloat4x4();
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][0], matWorld.r[0]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][1], matWorld.r[1]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][2], matWorld.r[2]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][3], matWorld.r[3]);
}

void CEffect_Group::Load_Data(const json& In_Json,_uint iIndex)
{
	EFFECT_GROUP_DESC Desc = {};

	Desc.bEnd = In_Json["bEnd"];
	Desc.eEffectType = (EFFECT_TYPE)In_Json["eEffectType"];
	Desc.strFilePath = In_Json["strFilePath"];
	Desc.strFileName = In_Json["strFileName"];
	Desc.fStartTime = In_Json["fStartTime"];

	_float4x4 matWorld;
	_float4 vTemp;
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][0], vTemp);
	memcpy(&matWorld.m[0], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][1], vTemp);
	memcpy(&matWorld.m[1], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][2], vTemp);
	memcpy(&matWorld.m[2], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][3], vTemp);
	memcpy(&matWorld.m[3], &vTemp, sizeof(_float4));

	Desc.matWorld = matWorld;

	m_vecGroup.push_back(Desc);
}

void CEffect_Group::Free()
{
	m_vecGroup.clear();

	__super::Free();

}