#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

#define CLORIGHT 0
#define CLOLEFT 1	

BEGIN(Client)

class CState_Bahamut_Clorush final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_Clorush, CState_Bahamut)

public:
	explicit CState_Bahamut_Clorush(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_Clorush() = default;

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
	_bool					m_bStrong = { false };

private:
	shared_ptr<class CBahamut_Clo>	m_pCloL = { nullptr };
	shared_ptr<class CBahamut_Clo>	m_pCloR = { nullptr };

private:
	_uint		m_iLeftRight_Dir = { 0 }; // 0 Right/ 1 Left

private:
	shared_ptr<CTrail_Buffer>		m_pCloTrail[8] = {};

private:
	void	Create_Clo();
	void	Activate_Clo(_bool bActivate,_uint iDir);
	void	Clear_Clo();

	void	Clo_Anim_Range(_float fMin, _float fMax,_uint iDir);

private:
	virtual void		Free();

};

END