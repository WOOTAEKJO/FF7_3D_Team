#pragma once

#include "Boss/AirBurster/State/State_AirBurster.h"
#include "Utility/LogicDeviceBasic.h"

BEGIN(Client)

class CState_AirBurster_FrontMachineGun final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_FrontMachineGun, CState_AirBurster)

public:
	explicit CState_AirBurster_FrontMachineGun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_FrontMachineGun() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	virtual void		Free();

private:
	FTimeChecker	m_bTimeCheckerL;
	FTimeChecker	m_bTimeCheckerR;
};

END