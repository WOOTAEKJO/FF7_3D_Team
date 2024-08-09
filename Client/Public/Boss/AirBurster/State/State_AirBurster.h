#pragma once
#include "../Public/Boss/State_Boss.h"

#define AIRBURSTERANIMSPEED 1.5f

BEGIN(Client)

class CState_AirBurster abstract : public CState_Boss
{
	INFO_CLASS(CState_AirBurster, CState)

public:
	explicit CState_AirBurster(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

public:
	void	SetUp_AI_Action_Num(_int iNum);

protected:
	_float	m_iFrame = { 0 };
	_bool	m_bNextAnim = { false };
	string		m_strCurAnimTag;

protected:
	_float3		m_vTargetPos = { 0.f,0.f,0.f };

protected:
	_uint		m_iCount = { 0 };

protected:

protected:
	virtual void		Free();
};

END