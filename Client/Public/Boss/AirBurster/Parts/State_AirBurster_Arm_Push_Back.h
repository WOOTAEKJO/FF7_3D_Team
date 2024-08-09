#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

BEGIN(Client)

class CState_AirBurster_Arm_Push_Back final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Arm_Push_Back, CState_AirBurster)

public:
	explicit CState_AirBurster_Arm_Push_Back(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Arm_Push_Back() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_float4				m_vStartPos = { 0.f,0.f,0.f,1.f };
	_float4				m_vOriginPos = { 0.f,0.f,0.f,1.f };
	_float				m_fDistance = { 10.f };

private:
	FTimeChecker		m_TimeChecker;

private:
	_bool				m_bFinished = { false };

private:
	virtual void		Free();
};

END