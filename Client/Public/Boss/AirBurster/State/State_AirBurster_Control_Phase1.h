#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define EMFIELDCOUNT 2
#define TURNCOUNT 3

BEGIN(Client)

class CAirBurster_EMField;

class CState_AirBurster_Control_Phase1 final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Control_Phase1, CState_AirBurster)

public:
	explicit CState_AirBurster_Control_Phase1(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Control_Phase1() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool				m_bIntro = { false }; // ��Ʈ�� ���� ����

private: // ���� ���ڱ���.
	_bool				m_bEMField = { false };	// ���ڱ��� ���� �� ����
	//_float				m_fTimeAcc = { 0.f }; // ���ڱ��� ���� �ð� ����
	_uint				m_iPatternCount = { 0 };	// ���ڱ��ĸ� �����ϰ� �������� �Ǵ� ����
	shared_ptr<CAirBurster_EMField> m_pEMField = { nullptr }; // ���ڱ��� �Ѿ�

private: // �� �� ��
	_uint				m_iTurnCount = { 0 }; // �� Ÿ�̹�

private:
	virtual void		Free();

};

END