#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include "Easing.h"

#include "Utility_File.h"

/*
	����Ʈ���� �θ���� �Ǵ� �߻�Ŭ�����Դϴ�.
*/

BEGIN(Engine)


class CBoneGroup;
END

BEGIN(Client)

class CEffect abstract  : public CBlendObject
{
public:
	enum DEADTYPE {TIME,SCALE,COLOR,RANGE,OWNER,TYPE_END}; // ����Ʈ ���� ���� ����ü

	enum USE_TYPE {
		USE_NONE = 0,				// �ܹ߼� ����Ʈ
		USE_FOLLOW_NORMAL = 1,		// ��� ����Ʈ�� �Ϲ� ���� ��ü�� ����
		USE_FOLLOW_PARTS = 2,		// ��� ����Ʈ�� ��Ʈ ������Ʈ�� ����
		USE_FOLLOW_EFFECT = 3,		// ��� ����Ʈ�� ����Ʈ�� ����
		USE_TYPE_END};
	// ����Ʈ ��� Ÿ�� ����ü

	enum COLORBLEND {COLOR_MULTIPLY,COLOR_SCREEN,COLOR_OVERLAY,COLOR_PLUS,COLOR_BURN};
	// ����Ʈ �÷� ���� Ÿ�� ����ü

	typedef struct tagEffect_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iShaderPassType = {0}; // ���̴� �н�

		_float fDiscard_Alpha = {0.f}; // ��ī�� ó�� ���� ��

		wstring strDiffuse;
		wstring strMask;
		wstring strNoise;

		_float2 vDiffuseUV = {0.f,0.f};
		_float2 vMaskUV = { 0.f,0.f };
		_float2 vNoiseUV = { 0.f,0.f };

		_float2 vSize = { 1.f,1.f };

		_float3 vRotation = {0.f,0.f,0.f};

		_float4 vSolid_Color = {1.f,1.f,1.f,1.f};

		// ------ Sprite ---------
		_bool bSpriteAnim = { false };	// ��������Ʈ ����

		_bool	bLoop = { true };		// ���� ����
		_float	fSpeed = { 1.f };		// ������ �ӵ� ����

		_float	fDuration = { 0.1f };	// ���� ���������� �Ѿ �ð� ��

		_uint			iSpriteType = {0};	// ��������Ʈ�� ����ϴ� �ؽ��İ� Diffuse �Ǵ� Mask
		
		_float2			vSpriteColRow = {1.f,1.f}; // ũ�� �� ������ �, ���� �

		// ----- Distortion ---------
		_float3	vDistortionValue = {1.f,1.f,1.f}; // ������ �ؽ����� ������ ���� �ø��� ���� �� 3��
		_float	fDistortionScale = {0.01f};		// �ְ� ����
		_float	fDistortionBias = {0.01f};		// �ְ�
		_float	fNoiseWeight = {1.f};	// ������ �ؽ��Ŀ� ���� �ð����� ����ġ

		// ------- Easing --------
		_bool	bEasing = { false };
		_uint	eEasingType = { 0 };	// Easing�Լ��� ����ϱ� ���� ������

		// -------- DeadType ------
		DEADTYPE eDeadType = { DEADTYPE::TYPE_END}; // ���� ���� Ÿ��

		// ------ Owner -------
		USE_TYPE eUseType = { USE_NONE }; // ������ Ÿ�Կ� ���� ������� ������ �޶�����.

		// ------ ColorBlendType --------
		COLORBLEND eColorBlendType = { COLORBLEND::COLOR_MULTIPLY};

		// ------ SolidColor -------- 
		_bool		bSolidColor = {false}; //���̴����� �ܻ� ����Ÿ���� ������� ����
	}EFFECT_DESC; // ����Ʈ ���� ����ü

protected:
	CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext); // ����̽��� ����̽� ���ؽ�Ʈ�� �޾� �ʱ�ȭ
	CEffect(const CEffect& rhs); // ���� ������
	virtual ~CEffect() = default; // ���� �Ҹ���

public:
	virtual HRESULT Initialize_Prototype() override; // ���� ��ü �ʱ�ȭ
	virtual HRESULT Initialize_Prototype(string strFilePath); // ���ڷ� ���� �ּҸ� �޴� ���� ��ü �ʱ�ȭ
	virtual HRESULT Initialize(void* pArg) override; // �纻 �ʱ�ȭ
	virtual void Begin_Play(_cref_time fTimeDelta) override; // ��ü�� ó������ �켱���� ƽ �Լ��� �� ��, �� �� ����.
	virtual void Priority_Tick(_cref_time fTimeDelta) override; // �켱���� ƽ
	virtual void Tick(_cref_time fTimeDelta) override; // �Ϲ����� ƽ
	virtual void Late_Tick(_cref_time fTimeDelta) override; // ���� ƽ
	virtual void Before_Render(_cref_time fTimeDelta) override; // ���� �Լ� ȣ�� �� ȣ��
	virtual HRESULT Render() override; // ��ü ����

	void Reset_Frame() {
		m_fFrame = 0; m_bIsFinished = false;
		m_vSpriteIndex = _float2(0.f, 0.f);
	} // ��������Ʈ �ִϸ��̼� ����

public:
	virtual void Write_Json(json& Out_Json); // ���̽� ����
	virtual void Load_Json(const json& In_Json); // ���̽� �ε�
	virtual	void Effect_Write_Json(json& Out_Json, EFFECT_DESC eDesc); // ����Ʈ ���̽� ����
	virtual	void Effect_Load_Json(const json& In_Json, EFFECT_DESC* eDesc); // ����Ʈ ���̽� �ε�

public:
	virtual	void	Reset_Effect(_bool bActivate = false) {};
	// ���� ����� �����ַ���

	virtual	void	Reset_Prototype_Data() {};
	//���� �����͸� ����

public:
	GETSET_EX2(_bool, m_bParticleDead, IsEffectDead, GET, SET)
	GETSET_EX2(_bool, m_bApplyTransform, ApplyTransform, GET, SET)
	GETSET_EX2(_float4x4, m_matOneTImeWorld, OneTImeWorld, GET, SET)
	GETSET_EX2(weak_ptr<class CTransform>, m_pOwnerTransformCom, OwnerTransformCom, GET, SET)

	GETSET_EX2(CBoneGroup*, m_pSocketBoneGroup, SocketBoneGroup, GET, SET)
	GETSET_EX2(_uint, m_iSocketBoneIndex, SocketBoneIndex, GET, SET)
	GETSET_EX2(_uint, m_iObjPool_MaxNum, ObjPool_MaxNum, GET, SET)
	

protected:
	_bool	m_bLoad = { false };
	_float	m_fTimeDelta = { 0.f };

protected:
	_float			m_fFrame = { 0.f };		// ������ ����� ���� ��
	_float2			m_vSpriteIndex = { 0.f,0.f }; // ���� ��������Ʈ(�� UV), ������
	_bool			m_bIsFinished = { false };// �ִϸ��̼��� ��� ��� �� ������ �ƴϸ� ������ �˸�

protected:
	_bool			m_bParticleDead = { false }; // ����Ʈ�� ���� ������ �����ߴ��� �Ǵ�.
	
protected:
		_float4x4	m_matJsonWorld;	// �ε�� �������
		_float4x4	m_matOneTImeWorld; // �ܹ߼� ���
	
protected:
	weak_ptr<class CTransform>	m_pOwnerTransformCom; // ����Ʈ ���ʸ� �޾ƿ��� �Ѵ�.

protected:
	_bool			m_bApplyTransform = { true }; // �� �󿡼� ����Ʈ ���� ������ �ƴ�. Ʈ������ ������� �������

protected:
	CBoneGroup*			m_pSocketBoneGroup = { nullptr }; // ������ �� �׷�.
	_uint				m_iSocketBoneIndex = { 0 }; // ������ ���� �ε���.

protected:
	_uint				m_iObjPool_MaxNum = { 200 }; // ������Ʈ Ǯ Ŭ�� ���� ����.

#pragma region ������ ���

protected:
	shared_ptr<class CTransform>	m_pOffsetTransform = { nullptr }; // ������ ��� Ʈ������.

public:
	_bool		m_bUseOffsetTransform = { false };

	weak_ptr<class CTransform> Get_OffsetTransform() { return m_pOffsetTransform; }
	GETSET_EX2(_bool, m_bUseOffsetTransform, UseOffsetTransform, GET, SET)

#pragma endregion

protected:
	HRESULT Ready_Components(); // ������Ʈ �غ�
	HRESULT Bind_ShaderResources(EFFECT_DESC EffectDesc); // ���̴� ���ҽ� ���ε�

protected:
	void	SpriteAnim(_cref_time fTimeDelta, EFFECT_DESC EffectDesc); // ��������Ʈ �ִϸ��̼�
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc); // ������ �Ǵ�
	void	UpdateWorldMatrix(EFFECT_DESC EffectDesc); // ��������� ������ ���ؿ� �°� ������ִ� �Լ�

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

/*
	
	- ����Ʈ�� �⺻������ m_matWorld��� ��� ������ ���Ѵ�.

	- Transform�� ��������� ���� ��ķ� ����Ѵ�.
		- ���ÿ������� ����, ��ġ���� ������ ���� ���.
	- m_matWorld ������ ������ķ� ����Ѵ�.
		- ���̴��� ���ε���Ű�� �������

*/