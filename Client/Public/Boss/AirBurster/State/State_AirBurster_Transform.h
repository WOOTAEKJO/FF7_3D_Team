#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define ROCKETCONDITIONNUM1 0
#define ROCKETCONDITIONNUM2 100

BEGIN(Client)

class CState_AirBurster_Transform final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Transform, CState_AirBurster)

public:
	enum AIRBURSTER_PATTERN {
		P1_REARMACHINEGUN = 1,
		P1_FRONTMACHINEGUN = 2,
		P1_ENERGYBALL = 3,
		P1_FINGERBEAM = 4,
		P1_EMFIELD = 5,
		P1_TANKBURSTER = 6,
		P1_BURNUR = 7,
		P1_SHOULDERBEAM = 8,
		P1_ROCKETARM = 9,
		P1_Docking = 10,
		P1_Dead = 11,
		P1_PHASE1ENTER = 97,
		P1_PHASE2ENTER = 98,
		P1_PHASE3ENTER = 99,
		P1_PATTERN_END
	};

public:
	explicit CState_AirBurster_Transform(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Transform() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint					m_iCurPhaseNum = { 0 }; // 현재 페이즈를 나타냄
	queue<_int>				m_quePreNum; // 같은거 연속 방지
	AIRBURSTER_PATTERN		m_eCurPattern = { P1_PATTERN_END }; // 현재 패턴을 나타낸다.

private: // Phase1
	_uint					m_iPhase1Num[5] = { 1,2,3,4,5 };

private: // Phase2
	_uint					m_iPhase2Num[4] = { 3,4,7,8 };
	_uint					m_iPhase2RocketArmNum[3] = { 3,6,8 };
	_uint					m_iPhase2Count = { 0 };
	_bool					m_bRocketPattern = { false };

private:
	_bool					m_bCheckPattern = { true };

private:
	_uint RandNum(_uint iPatternNum);

	
private:
	virtual void		Free();
};

END