#pragma once
#include "Client_Defines.h"
#include "State.h"

#include "Utility/LogicDeviceBasic.h"

#include "Camera_Manager.h"

BEGIN(Client)

class CPlayer;

class CState_Boss abstract : public CState
{
	INFO_CLASS(CState_Boss, CState)

public:
	explicit CState_Boss(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Boss() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

public:
	void					Cur_Player_Look(_cref_time fTimeDelta, _bool bYMediate = true);
	_vector					Get_CurPlayer_Y_Mediate_Dir();
	void					TurnOnOff_MotionBlur(_float fStart, _float fEnd,
		_bool bActorCenter,_float fBlurScale = 0.125f, _float fBlurAmount = 64.f,_float2 vCenter = _float2(0.5f,0.5f));
	void					MotionBlur_On(_bool bActorCenter, _float fBlurScale = 0.125f, _float fBlurAmount = 64.f, _float2 vCenter = _float2(0.5f, 0.5f));
	void					Camera_Shaking(string strTag,_float fStart, _float fEnd);
	void					Camera_Shaking(string strTag,_bool bCheck);

protected:
	_float					m_fTimeAcc = { 0.f };

protected:
	FTimeChecker			m_CameraShaking_Time;
	_bool					m_bShakingCheck = { true };

protected:
	virtual void		Free();
};

END