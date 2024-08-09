#pragma once

#include "Boss/AirBurster/State/State_AirBurster.h"
#include "Utility/LogicDeviceBasic.h"

#define HOOKRUSHMAXLANGTH 15
#define HOOKRUSHMINLANGTH 3
#define NONTARGETDISTANCE 3

BEGIN(Client)

class CState_AirBurster_RearMachineGun final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_RearMachineGun, CState_AirBurster)

public:
	explicit CState_AirBurster_RearMachineGun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_RearMachineGun() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	FTimeChecker	m_bTimeCheckerL;
	FTimeChecker	m_bTimeCheckerR;

private:
	_bool			m_bArrive = { false }; // ���� �Ǵ�

private:
	_float4			m_vOriginPos = { 0.f,0.f,0.f,1.f }; // ������ ��ġ�� ����

private:
	_float			m_fSpeed = { 7.f };	// ���������� ������ ���ǵ��� ������ �ִ� ��
	_float			m_fOriginDistance = { 0.f }; // ������ ��ġ���� Ÿ�� ��ġ������ �� �Ÿ�

private:
	_float			m_fMaxSpeed = { 40.f };
	_float			m_fMinSpeed = { 7.f };
	_float			m_fAcceleration = { -0.1f };

private:
	FTimeChecker	m_TimeChecker;
	_bool			m_bMotionBlur = { true };

private:
	virtual void		Free();

};

END