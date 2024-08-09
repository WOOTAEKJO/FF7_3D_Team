#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

BEGIN(Client)

class CState_AirBurster_Arm_IDLE final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Arm_IDLE, CState_AirBurster)

public:
	explicit CState_AirBurster_Arm_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Arm_IDLE() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_float3		m_vArmLocation_Offset = { 0.f,0.f,20.f };

private:
	_bool		m_bVisit_SeparateState = { false };

private:
	virtual void		Free();
};

END