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
	이펙트 생성 관리 및 데이터 처리를 도와주는 매니저 클래스입니다.
*/

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEffect;

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager) // 싱글톤
private:
	CEffect_Manager();
	virtual	~CEffect_Manager() = default;

public:
	HRESULT	Initialize(); // 초기화

public: // 툴 전용
	shared_ptr<CEffect_Group> Get_EffectGroup(); // 그룹 이펙트를 반환
	void	Set_EffectGroup(weak_ptr<CEffect_Group> pEffectGroup); // 그룹 이펙트 저장

public: // 데이터 로드관련 메서드들
	HRESULT	Create_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext); 
	void	Reset_Data_Prototype(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	template<class T>
	shared_ptr<T>	Create_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL,
		_float3 vPos = _float3(0.f,0.f,0.f),
		_float4 vAxis = _float4(0.f,1.f,0.f,0.f),_float fAngle = 0.f);
	// 이펙트를 생성하는 템플릿 함수

	shared_ptr<CParticle>	Create_Mesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL,
		CVIBuffer_Instancing::MESH_START_TYPE eStartType = CVIBuffer_Instancing::MESH_START_TYPE::RANDOM_POS);
	// 모델 파티클을 생성

	shared_ptr<CParticle>	Create_EffectMesh_VTX_Particle(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		CVIBuffer_Instancing::MESH_START_TYPE eStartType = CVIBuffer_Instancing::MESH_START_TYPE::RANDOM_POS);
	// 이펙트 메쉬 파티클을 생성

	template<class T>
	shared_ptr<T>	Create_Hit_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
		_float3 vHitPos, _float3 vHitDir);
	// 히트 이펙트 생성 템플릿 함수

	template<class T>
	shared_ptr<T>	Create_Bone_Effect_One_Time(wstring strEffectName,wstring strBoneName, shared_ptr<CGameObject> pOwner,
		CEffect::USE_TYPE eOwnerType = CEffect::USE_TYPE::USE_FOLLOW_NORMAL);
	// 뼈에 영향을 받는 이펙트 생성 템플릿 함수

	template<class T>
	shared_ptr<T>	Create_Map_Effect(wstring strEffectName,_float3 vOffset = _float3(0.f,0.f,0.f));
	// 환경 이펙트 생성 템플릿 함수

private:
	CGameInstance* m_pGameInstance = { nullptr };

private: // 툴 전용
	shared_ptr<CEffect_Group> m_pEffectGroup = { nullptr };

public:
	virtual	void	Free() override;
};

template<class T>
shared_ptr<T>   CEffect_Manager::Create_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
	CEffect::USE_TYPE eOwnerType, _float3 vPos, _float4 vAxis, _float fAngle)
{// 이펙트 생성 템플릿 함수
#if EFFECT_LOAD != 0 
	return nullptr;
#endif
	_matrix matOwnerWorld;

	if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_NORMAL)
		matOwnerWorld = Get_MatrixNormalize(pOwner->Get_TransformCom().lock()->Get_WorldMatrix()); // 일반 트랜스폼
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
		matOwnerWorld = Get_MatrixNormalize(dynamic_pointer_cast<CPartObject>(pOwner)->Get_WorldMatrix()); // 파츠 월드행렬
	else if (eOwnerType == CEffect::USE_TYPE::USE_FOLLOW_EFFECT)
		matOwnerWorld = Get_MatrixNormalize(dynamic_pointer_cast<CBlendObject>(pOwner)->Get_matWorld()); // 이펙트 월드행렬
	// 여기에 이펙트를 생성하는 코드
	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner, vPos);

	pEffect->Get_TransformCom().lock()->Rotation(vAxis, XMConvertToRadians(fAngle));
	// 로컬행렬 각도 변경

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * matOwnerWorld);
	// 초기 위치 잡기

	pEffect->Set_OneTImeWorld(matOwnerWorld);
	// 단발성 위치를 위함

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T>	CEffect_Manager::Create_Hit_Effect(wstring strEffectName, shared_ptr<CGameObject> pOwner,
	_float3 vHitPos, _float3 vHitDir)
{// 히트 이펙트 생성 템플릿 함수

#if EFFECT_LOAD != 0 
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner, vHitPos);

	pEffect->Get_TransformCom().lock()->Set_Up(vHitDir);
	// 타격 방향

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld);
	// 초기 위치 잡기

	pEffect->Set_OneTImeWorld(XMMatrixIdentity());
	// 단발성 위치를 위함

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T> CEffect_Manager::Create_Bone_Effect_One_Time(wstring strEffectName, wstring strBoneName,
	shared_ptr<CGameObject> pOwner, CEffect::USE_TYPE eOwnerType)
{// 뼈에 영향을 받는 이펙트 생성 템플릿 함수

#if EFFECT_LOAD != 0
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, pOwner);

	_matrix MuzzleMatrix = Get_MatrixNormalize(pOwner->Get_ModelCom().lock()->
		Get_BoneTransformMatrixWithParents(strBoneName));
	// 뼈 행렬 가져오기

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * MuzzleMatrix);
	// 초기 위치 잡기

	pEffect->Set_OneTImeWorld(MuzzleMatrix);
	// 단발성 위치를 위함

	CBoneGroup* pBoneGroup = pOwner->Get_ModelCom().lock()->Get_BoneGroup();
	if (pBoneGroup)
	{
		pEffect->Set_SocketBoneGroup(pBoneGroup);
		pEffect->Set_SocketBoneIndex(pBoneGroup->Find_BoneData(strBoneName)->iID);
	} // 뼈 그룹과 특정 뼈의 인덱스를 저장

	return dynamic_pointer_cast<T>(pEffect);
}

template<class T>
inline shared_ptr<T> CEffect_Manager::Create_Map_Effect(wstring strEffectName, _float3 vOffset)
{// 환경 이펙트 생성 템플릿 함수

#if EFFECT_LOAD != 0
	return nullptr;
#endif

	shared_ptr<CEffect> pEffect = { nullptr };
	pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<T>(strEffectName, L_EFFECT, nullptr, nullptr, vOffset);

	_matrix matWorld = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();

	pEffect->Set_matWorld(matWorld * XMMatrixIdentity());
	// 초기 위치 잡기

	pEffect->Set_OneTImeWorld(XMMatrixIdentity());
	// 단발성 위치를 위함

	return DynPtrCast<T>(pEffect);
}

END

