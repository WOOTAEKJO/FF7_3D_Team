#pragma once

#include "Effect.h"
#include "VIBuffer_Instancing.h"

/*
	��ƼŬ ����Ʈ�� �����ϴ� Ŭ�����Դϴ�.
	CEffect Ŭ������ ��ӹ޾ҽ��ϴ�.

*/

BEGIN(Engine)

END

BEGIN(Client)

class CParticle final : public CEffect
{
public:
	typedef struct tagParticleDesc : public CEffect::EFFECT_DESC
	{
		_uint iNumInstnace;

		// -----  MaskArray --------
		
		_bool	bMaskArray = { false };
		_uint	iMaskArrayNum = { 0 };
		wstring strMaskArray[18];
		
		CVIBuffer_Instancing::INSTANCING_DESC Instancing_Desc = {};
		
		// ----- Soft -------
		_bool		bParticleSoft = { true };

	}PARTICLE_DESC; // ��ƼŬ ���� ����ü
private:
	CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext); // ����̽��� ����̽� ���ؽ�Ʈ�� �޾� �ʱ�ȭ
	CParticle(const CParticle& rhs); // ���� ������
	virtual ~CParticle() = default; // ���� �Ҹ���

public:
	virtual HRESULT Initialize_Prototype() override; // ���� ��ü �ʱ�ȭ
	virtual HRESULT Initialize_Prototype(string strFilePath); // ���ڷ� ���� �ּҸ� �޴� ���� ��ü �ʱ�ȭ
	virtual HRESULT Initialize(void* pArg) override; // �纻 ��ü �ʱ�ȭ
	virtual void Begin_Play(_cref_time fTimeDelta) override; // �ʱ�ȭ �� ó�� �� �� ȣ��
	virtual void Priority_Tick(_cref_time fTimeDelta) override; // �켱���� ƽ
	virtual void Tick(_cref_time fTimeDelta) override; // �Ϲ����� ƽ
	virtual void Late_Tick(_cref_time fTimeDelta) override; // ���� ƽ
	virtual void Before_Render(_cref_time fTimeDelta) override; // ���� �Լ� ȣ�� �� ȣ��
	virtual HRESULT Render() override; // ��ü ����

public:
	virtual	void	Reset_Effect(_bool bActivate = false) override; // ����Ʈ ����
	virtual	void	Reset_Prototype_Data() override; // ���� ������ ����

private:
	CVIBuffer_Instancing::INSTANCING_DESC* m_pInstancingDesc = { nullptr }; // �ν��Ͻ� ����

	_bool		  m_bPlay = { true };

	PARTICLE_DESC	m_tParticle_Desc;

private:
	shared_ptr<class CMaterialComponent> m_pMaskArrayCom = { nullptr };

private:
	HRESULT Ready_Components(); // ������Ʈ �غ�
	HRESULT Bind_ShaderResources(); // ���̴� ���ҽ� ���ε�

private:
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc) override; // ���� �Ǵ�

public:
		void Reset_AllParticles(); // ��� ��ƼŬ �ν��Ͻ� ����

		GETSET_EX2(_bool, m_bPlay, Play, GET, SET)
		GETSET_EX2(PARTICLE_DESC, m_tParticle_Desc, ParticleDesc, GET, SET)

public:
	virtual void Write_Json(json& Out_Json); // ���̽� ����
	virtual void Load_Json(const json& In_Json); // ���̽� �ε�

public:
	/* ������ü�� �����Ѵ�. */
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* �纻��ü�� �����Ѵ�. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END