#include "stdafx.h"
#include "Effect_Manager.h"

#include "GameInstance.h"

#include "Utility_File.h"
#include <filesystem>

#include "Particle.h"
#include "Sprite.h"
#include "Trail_Effect.h"
#include "Trail_Buffer.h"
#include "Effect_Group.h"

#include "ModelContainer.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CEffect_Manager::Initialize()
{// �ʱ�ȭ �Լ�
	return S_OK;
}

shared_ptr<CEffect_Group> CEffect_Manager::Get_EffectGroup()
{// ����Ʈ �׷� ��ȯ
	if (m_pEffectGroup == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
			TEXT("Prototype_GameObject_EffectGroup"), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pEffectGroup))))
			return nullptr;
	}

	return m_pEffectGroup;
}

void CEffect_Manager::Set_EffectGroup(weak_ptr<CEffect_Group> pEffectGroup)
{// ����Ʈ �׷� ����
	if (m_pEffectGroup != nullptr)
	{
		m_pEffectGroup->TurnOn_State(OBJSTATE::WillRemoved);
		m_pEffectGroup = nullptr;
	}

	m_pEffectGroup = pEffectGroup.lock();
}

HRESULT CEffect_Manager::Create_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{// ���� ����Ʈ ����

	_uint iCloneNum(0);

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator("../Bin/Data/Effect_Data/"))
	{
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;
		_bool	bNumCheck = false;
		CPath_Mgr::FILE_TYPE eType = CPath_Mgr::FILE_TYPE::FILE_TYPE_END;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if (!strcmp(strTmp.c_str(), ""))
			continue;

		strTmp = szFileName;

		vector<string> vecTemp = SplitStr(szDirectory, '/');
		vecTemp = SplitStr(vecTemp.back(), '\\');
		string strDataType = vecTemp.front();
		
		if (!strcmp(strDataType.c_str(), "Particle"))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
				CParticle::Create(pDevice, pContext, strFIlePath))))
				RETURN_EFAIL;
		}
		else if (!strcmp(strDataType.c_str(), "Rect"))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
				CSprite::Create(pDevice, pContext, strFIlePath))))
				RETURN_EFAIL;
		}
		else if (!strcmp(strDataType.c_str(), "Trail"))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
				CTrail_Effect::Create(pDevice, pContext, strFIlePath))))
				RETURN_EFAIL;
		}
		else if (!strcmp(strDataType.c_str(), "TrailBuffer"))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
				CTrail_Buffer::Create(pDevice, pContext, strFIlePath))))
				RETURN_EFAIL;
		}
		else {
			continue;
		}

		auto pProtoObj = m_pGameInstance->Get_ObjectPrototypeList()->find(StrToWstr(strTmp));
		iCloneNum = dynamic_pointer_cast<CEffect>(pProtoObj->second)->Get_ObjPool_MaxNum();

#if EFFECT_LOAD == 2
		iCloneNum = 1;
#endif

		CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE,StrToWstr(strTmp), StrToWstr(strFIlePath));
		CObjPool_Manager::GetInstance()->Add_ObjectPool(StrToWstr(strTmp), iCloneNum);
		// ������Ʈ Ǯ�� �߰�
	}

	// ���� ����Ʈ�� �ε��ϰ� �� �Ŀ� �׷� ����Ʈ�� �ε��ؾ��Ѵ�.

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator("../Bin/Data/Effect_Data/Group/"))
	{
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if (!strcmp(strTmp.c_str(), ""))
			continue;

		strTmp = szFileName;

		if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
			CEffect_Group::Create(pDevice, pContext, strFIlePath))))
			RETURN_EFAIL;

		auto pProtoObj = m_pGameInstance->Get_ObjectPrototypeList()->find(StrToWstr(strTmp));
		iCloneNum = dynamic_pointer_cast<CEffect>(pProtoObj->second)->Get_ObjPool_MaxNum();

#if EFFECT_LOAD == 2
		iCloneNum = 1;
#endif

		CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, StrToWstr(strTmp), StrToWstr(strFIlePath));
		CObjPool_Manager::GetInstance()->Add_ObjectPool(StrToWstr(strTmp), iCloneNum);
	}

	return S_OK;
}

void CEffect_Manager::Reset_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{// ���� ����Ʈ ������ ����
	auto PrototypeList = m_pGameInstance->Get_ObjectPrototypeList(); // ���� ����Ʈ

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator("../Bin/Data/Effect_Data/"))
	{ // ���� ����Ʈ ����
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;
		_bool	bNumCheck = false;
		CPath_Mgr::FILE_TYPE eType = CPath_Mgr::FILE_TYPE::FILE_TYPE_END;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if (!strcmp(strTmp.c_str(), ""))
			continue;

		strTmp = szFileName;

		auto iter = PrototypeList->find(StrToWstr(strTmp));
		// ����Ʈ���� �����͸� �˻�.

		if (iter == PrototypeList->end())
		{// �����Ͱ� ������
			vector<string> vecTemp = SplitStr(szDirectory, '/');
			vecTemp = SplitStr(vecTemp.back(), '\\');
			string strDataType = vecTemp.front();

			if (!strcmp(strDataType.c_str(), "Particle"))
			{
				if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
					CParticle::Create(pDevice, pContext, strFIlePath))))
					return;
			}
			else if (!strcmp(strDataType.c_str(), "Rect"))
			{
				if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
					CSprite::Create(pDevice, pContext, strFIlePath))))
					return;
			}
			else if (!strcmp(strDataType.c_str(), "Trail"))
			{
				if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
					CTrail_Effect::Create(pDevice, pContext, strFIlePath))))
					return;
			}
			else if (!strcmp(strDataType.c_str(), "TrailBuffer"))
			{
				if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
					CTrail_Buffer::Create(pDevice, pContext, strFIlePath))))
					return;
			}
			else {
				continue;
			}

			CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, StrToWstr(strTmp), StrToWstr(strFIlePath));
		}
		else {// �����Ͱ� ������ �ٽ� ����
			shared_ptr<CEffect> pEffectProto = dynamic_pointer_cast<CEffect>(iter->second);
			if (pEffectProto != nullptr)
			{
				pEffectProto->Reset_Prototype_Data();
				// ��� ����Ʈ�� �����͵��� �ٽ� �޾ƿ�
				// ������ �����͸� ����
			}
		}
	}

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator("../Bin/Data/Effect_Data/Group/"))
	{// �׷� ����Ʈ. ���� ����Ʈ�� ���� ������ �׷��� �����ؾ���.
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if (!strcmp(strTmp.c_str(), ""))
			continue;

		strTmp = szFileName;

		auto iter = PrototypeList->find(StrToWstr(strTmp));
		// ����Ʈ���� �����͸� �˻�.

		if (iter == PrototypeList->end())
		{// �����Ͱ� ������
			if (FAILED(m_pGameInstance->Add_Prototype(StrToWstr(strTmp),
				CEffect_Group::Create(pDevice, pContext, strFIlePath))))
				return;

			CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, StrToWstr(strTmp), StrToWstr(strFIlePath));
		}
		else
		{// �����Ͱ� ������
			shared_ptr<CEffect> pEffectProto = dynamic_pointer_cast<CEffect>(iter->second);
			if (pEffectProto != nullptr)
			{
				pEffectProto->Reset_Prototype_Data();
				// ��� ����Ʈ�� �����͵��� �ٽ� �޾ƿ�
				// ������ �����͸� ����
			}
		}
	}

	auto EffectClonList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(),L_EFFECT);

	if (EffectClonList == nullptr)
		return;

	for (auto& iter : *EffectClonList)
	{
		if(dynamic_pointer_cast<CEffect_Group>(iter) == nullptr)
			iter->Set_Dead(); // �׷� ����Ʈ�� ����.]
	}// ���� ����Ʈ Ŭ�е��� ����ó��
}

shared_ptr<CParticle> CEffect_Manager::Create_Mesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner,
	CEffect::USE_TYPE eOwnerType
	, CVIBuffer_Instancing::MESH_START_TYPE eStartType)
{// �� ��ƼŬ�� ����

#if EFFECT_LOAD != 0 
	return nullptr;
#endif
	_matrix matOwnerWorld = XMMatrixIdentity();

	if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_NORMAL)
		matOwnerWorld = pOwner->Get_TransformCom().lock()->Get_WorldMatrix(); // �Ϲ� Ʈ������
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
		matOwnerWorld = dynamic_pointer_cast<CPartObject>(pOwner)->Get_WorldMatrix(); // ���� �������
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_EFFECT)
		matOwnerWorld = dynamic_pointer_cast<CBlendObject>(pOwner)->Get_matWorld(); // ����Ʈ �������

	shared_ptr<CParticle> pParticle = GET_SINGLE(CObjPool_Manager)->Create_Object<CParticle>(strEffectName, L_EFFECT, nullptr, pOwner);

	_matrix matWorld = pParticle->Get_TransformCom().lock()->Get_WorldMatrix();

	pParticle->Set_matWorld(matWorld * matOwnerWorld);
	// �ʱ� ��ġ ���
	pParticle->Set_OneTImeWorld(matOwnerWorld);
	// �ܹ߼� ��ġ�� ����

	shared_ptr<CVIBuffer_Instancing> pBuffer = dynamic_pointer_cast<CVIBuffer_Instancing>(pParticle->Get_VIBufferCom().lock());

	pBuffer->Get_InstancingDesc()->bMeshVTXParticle = true; // �޽� ����Ʈ ���
	pBuffer->Get_InstancingDesc()->eMeshStartType = eStartType; // �޽� ���� ��ƼŬ ���� ��ġ Ÿ�� ����
	pBuffer->Get_InstancingDesc()->fMeshSize = 1.f; // �޽� ����� �����Ѵ�.
	pBuffer->Get_InstancingDesc()->vecMeshVTX.clear(); // �޽� ���� �ʱ�ȭ

	vector<shared_ptr<CMeshComp>> vecMehsComp = pOwner->Get_ModelCom().lock()->Get_MeshComps();
	CBoneGroup* pBoneGroup = pOwner->Get_ModelCom().lock()->Get_BoneGroup();
	auto ModelType = pOwner->Get_ModelCom().lock()->Get_ModelType();
	
	for (auto MeshComp : vecMehsComp)
	{
		if (ModelType == CCommonModelComp::TYPE::TYPE_ANIM) // �ִϸ��̼� �� ����
		{
			// ����, ����ġ
			const vector<_float3>& vecVtx = MeshComp->Get_MeshData()->vecVertices;
			const vector<_int4>& vecBoneIDs = MeshComp->Get_MeshData()->vecBoneIDs;
			const vector<_float4>& vecWeights = MeshComp->Get_MeshData()->vecWeights;
			// ������, Combined ���
			const vector<FMeshBoneData>& MeshOffsets = MeshComp->Get_MeshData()->Provide_Offsets();
			const vector<_float4x4>& Transforms = pBoneGroup->CRef_BoneCombinedTransforms();

			for (_uint i = 0; i < Cast<_uint>(vecVtx.size()); ++i)
			{
				const _int4& VtxBoneIDs = vecBoneIDs[i];
				_matrix FinalTransform =
					(MeshOffsets[VtxBoneIDs.x].matOffset * Transforms[MeshOffsets[VtxBoneIDs.x].iBoneID]) * vecWeights[i].x +
					(MeshOffsets[VtxBoneIDs.y].matOffset * Transforms[MeshOffsets[VtxBoneIDs.y].iBoneID]) * vecWeights[i].y +
					(MeshOffsets[VtxBoneIDs.z].matOffset * Transforms[MeshOffsets[VtxBoneIDs.z].iBoneID]) * vecWeights[i].z +
					(MeshOffsets[VtxBoneIDs.w].matOffset * Transforms[MeshOffsets[VtxBoneIDs.w].iBoneID]) * vecWeights[i].w;
				// �������� ��ġ�� ���� �ִϸ��̼� �� ����� �����ش�.
				_vector vLocalPos = XMVector3TransformCoord(XMVectorSetW(vecVtx[i],1.f), FinalTransform);
				pBuffer->Get_InstancingDesc()->vecMeshVTX.push_back(vLocalPos);
			}
		}
		else if(ModelType == CCommonModelComp::TYPE::TYPE_NONANIM)// �� �ִϸ��̼� �� ����
		{
			const vector<_float3>& vecVtx = MeshComp->Get_MeshData()->vecVertices;

			for (auto LocalPos : vecVtx)
			{
				_vector vLocalPos = XMVectorSetW(LocalPos, 1.f);
				pBuffer->Get_InstancingDesc()->vecMeshVTX.push_back(vLocalPos);
			}
		}
	}
	// �޽����� ������ ä��
	pBuffer->Reset_AllParticles(); // ���� ���Ѽ� ���� ���� ����

	return pParticle;
}

shared_ptr<CParticle> CEffect_Manager::Create_EffectMesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner, CVIBuffer_Instancing::MESH_START_TYPE eStartType)
{// ����Ʈ �޽� ��ƼŬ�� ����
#if EFFECT_LOAD != 0 
	return nullptr;
#endif

	if (dynamic_pointer_cast<CTrail_Effect>(pOwner) == nullptr)
		return nullptr; // ����Ʈ �޽� ��ü�� �ƴϸ� ����

	_matrix matOwnerWorld = XMMatrixIdentity();
	matOwnerWorld = dynamic_pointer_cast<CBlendObject>(pOwner)->Get_matWorld(); // ����Ʈ �������
		
	shared_ptr<CParticle> pParticle = GET_SINGLE(CObjPool_Manager)->Create_Object<CParticle>(strEffectName, L_EFFECT, nullptr, pOwner);

	if (pParticle == nullptr)
		return nullptr;

	_matrix matWorld = pParticle->Get_TransformCom().lock()->Get_WorldMatrix();

	pParticle->Set_matWorld(matWorld * matOwnerWorld);
	// �ʱ� ��ġ ���

	pParticle->Set_OneTImeWorld(matOwnerWorld);
	// �ܹ߼� ��ġ�� ����

	shared_ptr<CVIBuffer_Instancing> pBuffer = dynamic_pointer_cast<CVIBuffer_Instancing>(pParticle->Get_VIBufferCom().lock());

	pBuffer->Get_InstancingDesc()->bMeshVTXParticle = true; // �޽� ����Ʈ ���
	pBuffer->Get_InstancingDesc()->eMeshStartType = eStartType; // �޽� ���� ��ƼŬ ���� ��ġ Ÿ�� ����
	pBuffer->Get_InstancingDesc()->fMeshSize = 1.f; // �޽� ����� �����Ѵ�.
	pBuffer->Get_InstancingDesc()->vecMeshVTX.clear(); // �޽� ���� �ʱ�ȭ.

	shared_ptr<CTrail_Effect> pEffectOwner = dynamic_pointer_cast<CTrail_Effect>(pOwner);

	pBuffer->Get_InstancingDesc()->vColor[0] = pEffectOwner->Get_TrailDesc()->vColor;
	pBuffer->Get_InstancingDesc()->vColor[1] = pEffectOwner->Get_TrailDesc()->vColor;
	// ���� ������ ����

	vector<shared_ptr<CMeshComp>> vecMehsComp = pOwner->Get_ModelCom().lock()->Get_MeshComps();

	for (auto MeshComp : vecMehsComp)
	{
		const vector<_float3>& vecVtx = MeshComp->Get_MeshData()->vecVertices;

		for (auto LocalPos : vecVtx)
		{
			_vector vLocalPos = XMVectorSetW(LocalPos, 1.f);
			pBuffer->Get_InstancingDesc()->vecMeshVTX.push_back(vLocalPos);
		}
	}// �޽����� ������ ä��

	pBuffer->Reset_AllParticles(); // ���� ���Ѽ� ���� ���� ����

	return pParticle;
}

void CEffect_Manager::Free()
{
}
