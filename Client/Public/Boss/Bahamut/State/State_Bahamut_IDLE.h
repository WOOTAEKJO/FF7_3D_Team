#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

#define  ULTIMATECOUNT 5	// 궁극기까지의 카운트 수
#define	 COUNTNUM 9			// 카운트 패턴까지의 패턴 갯수
#define	 SPECIALNUM 4		// 스페셜 공격하기 까지의 패턴 갯수

#define	MIDDLEDISTANCE	18	// 중간거리
#define CLOSEDISTANCE  8	// 근접거리

#define MELEEATTACKCOUNT 2 // 근접 공격 카운트
#define LONGATTACKCOUNT 2  // 원거리 공격 카운트 
#define SPINATTACKCOUNT 2  // 스핀 공격 카운트

/*
	바하무트라는 보스의 상태 중 하나인 IDLE 상태 클래스입니다.
	CState_Bahamut 클래스를 상속 받았습니다.
*/

BEGIN(Client)

class CState_Bahamut_IDLE final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_IDLE, CState_Bahamut)

public:
	explicit CState_Bahamut_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_IDLE() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	FTimeChecker			m_TimeChecker;

private:
	_uint					m_iUltimateCount = { 0 }; // 궁극기까지 남은 카운트를 나타냄
	_uint					m_iPatternCount = { 0 };	// 패턴을 몇번 돌았는지 파악하기 위함
	
private:
	_uint					m_iRandomNum = { 0 };				// 패턴에 사용할 랜덤한 숫자를 저장

private:
	_uint					m_iLong_Distance_Attack_Count = { 0 };	// 원거리 공격이 몇번인지 체크
	_uint					m_iSpinRush_Count = { 0 };	// 스핀 공격 몇번인지 체크
	_uint					m_iMelee_Count = { 0 };	// 근접 공격 몇번인지 체크

private:
	virtual void		Free();

};

END