#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

BEGIN(Client)

class CState_AirBurster_Arm_Separate final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Arm_Separate, CState_AirBurster)

public:
	explicit CState_AirBurster_Arm_Separate(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Arm_Separate() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool			m_bLerp = { false };
	_float			m_fPosY = { 0.f };
	_float			m_fMovement_amount = { 12.f };
	_float			m_fStartPosX = { 0.f };
	_bool			m_bMove = { true };

private:
	virtual void		Free();
};

END