#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

BEGIN(Client)

class CState_Bahamut_HeavyStrike final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_HeavyStrike, CState_Bahamut)

public:
	explicit CState_Bahamut_HeavyStrike(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_HeavyStrike() = default;

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
	shared_ptr<class CBahamut_HeavyStrike> m_pStrikeBulletL = { nullptr };
	shared_ptr<class CBahamut_HeavyStrike> m_pStrikeBulletR = { nullptr };

private:
	virtual void		Free();

};

END