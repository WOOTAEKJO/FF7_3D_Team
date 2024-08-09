#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

BEGIN(Client)

class CState_AirBurster_Hook_Return final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Hook_Return, CState_AirBurster)

public:
	explicit CState_AirBurster_Hook_Return(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Hook_Return() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool				m_bArrive = { false };
	_bool				m_bMediate = { false };

private:
	_float4				m_vOriginPos = { 0.f,0.f,0.f,1.f }; // 오리진 위치를 저장
	_float				m_fSpeed = { 40.f };	// 최종적으로 나오는 스피드의 영향을 주는 값
	_float				m_fOriginDistance = { 0.f }; // 오리진 위치에서 타겟 위치까지의 총 거리

private:
	_float				m_fMaxSpeed = { 30.f };
	_float				m_fMinSpeed = { 5.f };
	_float				m_fAcceleration = { -0.1f };

private:
	virtual void		Free();

};

END