#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

#define	DIVETRAILNUM	8

BEGIN(Client)

class CState_Bahamut_AerialRave final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_AerialRave, CState_Bahamut)

public:
	explicit CState_Bahamut_AerialRave(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_AerialRave() = default;

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
	shared_ptr<class CBahamut_Body>	m_pBodyBullet = { nullptr };

private:
	shared_ptr<CTrail_Buffer>		m_pSpinTrail[DIVETRAILNUM] = {};

private:
	void	Create_Trail();
	void	Activate_Trail(_bool bActivate);

private:
	virtual void		Free();

};

END