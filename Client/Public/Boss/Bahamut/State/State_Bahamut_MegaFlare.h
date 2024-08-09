#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

BEGIN(Client)

class CBahamut_Meteor;
class CBahamut_FlareBall;

class CState_Bahamut_MegaFlare final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_MegaFlare, CState_Bahamut)

public:
	explicit CState_Bahamut_MegaFlare(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_MegaFlare() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	FTimeChecker			m_TimeChecker;
	_bool					m_bRepeat = { true };

private:
	shared_ptr<CBahamut_Meteor>	m_pMeteor = nullptr;
	shared_ptr<CBahamut_FlareBall>	m_pFlareBall = nullptr;

private:
	virtual void		Free();

};

END