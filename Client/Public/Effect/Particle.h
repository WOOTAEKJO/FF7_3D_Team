#pragma once

#include "Effect.h"
#include "VIBuffer_Instancing.h"

/*
	파티클 이펙트를 관리하는 클래스입니다.
	CEffect 클래스를 상속받았습니다.

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

	}PARTICLE_DESC; // 파티클 정의 구조체
private:
	CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext); // 디바이스와 디바이스 컨텍스트를 받아 초기화
	CParticle(const CParticle& rhs); // 복사 생성자
	virtual ~CParticle() = default; // 가상 소멸자

public:
	virtual HRESULT Initialize_Prototype() override; // 원형 객체 초기화
	virtual HRESULT Initialize_Prototype(string strFilePath); // 인자로 파일 주소를 받는 원형 객체 초기화
	virtual HRESULT Initialize(void* pArg) override; // 사본 객체 초기화
	virtual void Begin_Play(_cref_time fTimeDelta) override; // 초기화 후 처음 한 번 호출
	virtual void Priority_Tick(_cref_time fTimeDelta) override; // 우선순위 틱
	virtual void Tick(_cref_time fTimeDelta) override; // 일반적인 틱
	virtual void Late_Tick(_cref_time fTimeDelta) override; // 늦은 틱
	virtual void Before_Render(_cref_time fTimeDelta) override; // 렌더 함수 호출 전 호출
	virtual HRESULT Render() override; // 객체 렌더

public:
	virtual	void	Reset_Effect(_bool bActivate = false) override; // 이펙트 리셋
	virtual	void	Reset_Prototype_Data() override; // 원형 데이터 리셋

private:
	CVIBuffer_Instancing::INSTANCING_DESC* m_pInstancingDesc = { nullptr }; // 인스턴스 버퍼

	_bool		  m_bPlay = { true };

	PARTICLE_DESC	m_tParticle_Desc;

private:
	shared_ptr<class CMaterialComponent> m_pMaskArrayCom = { nullptr };

private:
	HRESULT Ready_Components(); // 컴포넌트 준비
	HRESULT Bind_ShaderResources(); // 셰이더 리소스 바인딩

private:
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc) override; // 죽음 판단

public:
		void Reset_AllParticles(); // 모든 파티클 인스턴스 리셋

		GETSET_EX2(_bool, m_bPlay, Play, GET, SET)
		GETSET_EX2(PARTICLE_DESC, m_tParticle_Desc, ParticleDesc, GET, SET)

public:
	virtual void Write_Json(json& Out_Json); // 제이슨 저장
	virtual void Load_Json(const json& In_Json); // 제이슨 로드

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string strFilePath);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END