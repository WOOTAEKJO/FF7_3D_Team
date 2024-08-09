#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define ROCKETCONDITIONNUM1 3
#define TANKCANNONCOUNT 3

BEGIN(Client)

class CAirBurster_EMField;

class CState_AirBurster_Control_Phase2 final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Control_Phase2, CState_AirBurster)

public:
	explicit CState_AirBurster_Control_Phase2(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Control_Phase2() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint				m_iRocketCount = { 0 }; // �� �и� �Ǵ�
	_uint				m_iTankCannonCount = { 0 }; // ������ ĳ�� �Ǵ�
	_bool				m_bSeparated = { false };	// �и� ��������
	_bool				m_bIntro = { false }; // ��Ʈ�� ���� ����

private:
	_bool				m_bVisitRocket = { false };
	 
private:
	virtual void		Free();

};

END