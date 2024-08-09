#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define EMFIELDCOUNT 2
#define TURNCOUNT 3

BEGIN(Client)

class CAirBurster_EMField;

class CState_AirBurster_Control_Phase1 final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Control_Phase1, CState_AirBurster)

public:
	explicit CState_AirBurster_Control_Phase1(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Control_Phase1() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool				m_bIntro = { false }; // 인트로 실행 유무

private: // 근접 전자기파.
	_bool				m_bEMField = { false };	// 전자기파 실행 중 유무
	//_float				m_fTimeAcc = { 0.f }; // 전자기파 유지 시간 저장
	_uint				m_iPatternCount = { 0 };	// 전자기파를 실행하고 삭제할지 판단 유무
	shared_ptr<CAirBurster_EMField> m_pEMField = { nullptr }; // 전자기파 총알

private: // 앞 뒤 턴
	_uint				m_iTurnCount = { 0 }; // 턴 타이밍

private:
	virtual void		Free();

};

END