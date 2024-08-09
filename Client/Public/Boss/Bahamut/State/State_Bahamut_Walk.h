#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

BEGIN(Client)

class CState_Bahamut_Walk final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_Walk, CState_Bahamut)

public:
	explicit CState_Bahamut_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_Walk() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_float3		m_vOriginLook = { 0.f,0.f,1.f };
	_float		m_fOriginAngle = { 0.f };
	_bool		m_bFinishLook = { false };

private:
	virtual void		Free();

};

END