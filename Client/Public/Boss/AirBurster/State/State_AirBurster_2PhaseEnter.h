#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

BEGIN(Client)

class CState_AirBurster_2PhaseEnter final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_2PhaseEnter, CState_AirBurster)

public:
	explicit CState_AirBurster_2PhaseEnter(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_2PhaseEnter() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool		m_bTurn = { true };
	_float		m_fMovement_amount = { 13.f };
	_float		m_fStartPosX = { 0.f };
	_bool		m_bMove = { true };

private:
	_uint		m_iTurnType = { 0 };

private:
	_float3		m_vCurPlayerPos[PLAYER_TYPE::PLAYER_END] = {};

private:
	virtual void		Free();
};

END