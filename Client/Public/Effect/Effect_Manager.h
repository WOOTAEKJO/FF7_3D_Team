#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "Effect_Group.h"
#include "Particle.h"
#include "Trail_Effect.h"
#include "Trail_Buffer.h"

#include "ObjPool_Manager.h"

#include "PartObject.h"

#include "BoneContainer.h"

#include "Level_Test_Defines.h"

/*
	����Ʈ ���� ���� �� ������ ó���� �����ִ� �Ŵ��� Ŭ�����Դϴ�.
*/

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEffect;

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager) // �̱���
private:
	CEffect_Manager();
	virtual	~CEffect_Manager() = default;

public:
	HRESULT	Initialize(); // �ʱ�ȭ

public: // �� ����
	shared_ptr<CEffect_Group> Get_EffectGroup(); // �׷� ����Ʈ�� ��ȯ
	void	Set_EffectGroup(weak_ptr<CEffect_Group> pEffectGroup); // �׷� ����Ʈ ����

public: // ������ �ε���� �޼����
	HRESULT	Create_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext); 
	void	Reset_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	template<class T>
	shared_ptr<T>	Create_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL,
		_float3 vPos = _float3(0.f,0.f,0.f),
		_float4 vAxis = _float4(0.f,1.f,0.f,0.f),_float fAngle = 0.f);
	// ����Ʈ�� �����ϴ� ���ø� �Լ�

	shared_ptr<CParticle>	Create_Mesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL,
		CVIBuffer_Instancing::MESH_START_TYPE eStartType = CVIBuffer_Instancing::MESH_START_TYPE::RANDOM_POS);
	// �� ��ƼŬ�� ����

	shared_ptr<CParticle>	Create_EffectMesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CVIBuffer_Instancing::MESH_START_TYPE eStartType = CVIBuffer_Instancing::MESH_START_TYPE::RANDOM_POS);
	// ����Ʈ �޽� ��ƼŬ�� ����

	template<class T>
	shared_ptr<T>	Create_Hit_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		_float3 vHitPos, _float3 vHitDir);
	// ��Ʈ ����Ʈ ���� ���ø� �Լ�

	template<class T>
	shared_ptr<T>	Create_Bone_Effect_One_Time(wstring strEffectName,wstring strBoneName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL);
	// ���� ������ �޴� ����Ʈ ���� ���ø� �Լ�

	template<class T>
	shared_ptr<T>	Create_Map_Effect(wstring strEffectName,_float3 vOffset = _float3(0.f,0.f,0.f));
	// ȯ�� ����Ʈ ���� ���ø� �Լ�

private:
	CGameInstance* m_pGameInstance = { nullptr };

private: // �� ����
	shared_ptr<CEffect_Group> m_pEffectGroup = { nullptr };

public:
	virtual	void	Free() override;
};

template<class T>
shared_ptr<T>   CEffect_Manager::Create_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
	CEffect::USE_TYPE eOwnerType, _float3 vPos, _float4 vAxis, _float fAngle)
{// ����Ʈ ���� ���ø� �Լ�
#if EFFECT_LOAD != 0 
	return nullptr;
#endif
	_matrix matOwnerWorld;

	if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_NORMAL)
		matOwnerWorld = Get_MatrixNormalize(pOwner->Get_TransformCom().lock()->Get_WorldMatrix()); // �Ϲ� Ʈ������
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
		matOwnerWorld = Get_MatrixNormalize(dynamic_pointer_cast<CPartObject>(pOwner)->Get_WorldMatrix()); // ���� �������
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_EFFECT)
		matOwnerWorld = Get_MatrixNormalize(dynamic_pointer_cast<CBlendObject>(pOwner)->Get_matWorld()); // ����Ʈ �������
	// ���⿡ ����Ʈ�� �����ϴ� �ڵ�
	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner, vPos);

	pEffect->Get_TransformCom().lock()->Rotation(vAxis, XMConvertToRadians(fAngle));
	// ������� ���� ����

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * matOwnerWorld);
	// �ʱ� ��ġ ���

	pEffect->Set_OneTImeWorld(matOwnerWorld);
	// �ܹ߼� ��ġ�� ����

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T>	CEffect_Manager::Create_Hit_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
	_float3 vHitPos, _float3 vHitDir)
{// ��Ʈ ����Ʈ ���� ���ø� �Լ�

#if EFFECT_LOAD != 0 
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner, vHitPos);

	pEffect->Get_TransformCom().lock()->Set_Up(vHitDir);
	// Ÿ�� ����

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld);
	// �ʱ� ��ġ ���

	pEffect->Set_OneTImeWorld(XMMatrixIdentity());
	// �ܹ߼� ��ġ�� ����

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T> CEffect_Manager::Create_Bone_Effect_One_Time(wstring strEffectName, wstring strBoneName,
	shared_ptr<CGameObject> pOwner, CEffect::USE_TYPE eOwnerType)
{// ���� ������ �޴� ����Ʈ ���� ���ø� �Լ�

#if EFFECT_LOAD != 0
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner);

	_matrix MuzzleMatrix = Get_MatrixNormalize(pOwner->Get_ModelCom().lock()->
		Get_BoneTransformMatrixWithParents(strBoneName));
	// �� ��� ��������

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * MuzzleMatrix);
	// �ʱ� ��ġ ���

	pEffect->Set_OneTImeWorld(MuzzleMatrix);
	// �ܹ߼� ��ġ�� ����

	CBoneGroup* pBoneGroup = pOwner->Get_ModelCom().lock()->Get_BoneGroup();
	if (pBoneGroup)
	{
		pEffect->Set_SocketBoneGroup(pBoneGroup);
		pEffect->Set_SocketBoneIndex(pBoneGroup->Find_BoneData(strBoneName)->iID);
	} // �� �׷�� Ư�� ���� �ε����� ����

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T> CEffect_Manager::Create_Map_Effect(wstring strEffectName, _float3 vOffset)
{// ȯ�� ����Ʈ ���� ���ø� �Լ�

#if EFFECT_LOAD != 0
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, nullptr, vOffset);

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * XMMatrixIdentity());
	// �ʱ� ��ġ ���

	pEffect->Set_OneTImeWorld(XMMatrixIdentity());
	// �ܹ߼� ��ġ�� ����

	return DynPtrCast<T>(pEffect);
}

END

