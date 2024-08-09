#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

BEGIN(Client)

class CState_Bahamut_Count final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_Count, CState_Bahamut)

public:
	explicit CState_Bahamut_Count(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_Count() = default;

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
	_uint					m_iFrame = { 0 };

private:
	virtual void		Free();

};

END