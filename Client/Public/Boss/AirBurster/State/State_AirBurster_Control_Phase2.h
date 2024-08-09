#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define ROCKETCONDITIONNUM1 3
#define TANKCANNONCOUNT 3

BEGIN(Client)

class CAirBurster_EMField;

class CState_AirBurster_Control_Phase2 final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Control_Phase2, CState_AirBurster)

public:
	explicit CState_AirBurster_Control_Phase2(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Control_Phase2() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint				m_iRocketCount = { 0 }; // 팔 분리 판단
	_uint				m_iTankCannonCount = { 0 }; // 버스터 캐논 판단
	_bool				m_bSeparated = { false };	// 분리 상태인지
	_bool				m_bIntro = { false }; // 인트로 실행 유무

private:
	_bool				m_bVisitRocket = { false };
	 
private:
	virtual void		Free();

};

END