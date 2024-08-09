#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

BEGIN(Client)

class CState_Bahamut_Grab final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_Grab, CState_Bahamut)

public:
	explicit CState_Bahamut_Grab(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_Grab() = default;

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
	_bool					m_bGrabSucces = {false};
	PLAYER_TYPE				m_eGrab_Player = { PLAYER_TYPE::PLAYER_END };

private:
	shared_ptr<class CBahamut_Grab>		m_pGrabBullet = { nullptr };
	shared_ptr<class CBahamut_FireBall>	m_pFireBall = { nullptr };

private:
	virtual void		Free();

};

END