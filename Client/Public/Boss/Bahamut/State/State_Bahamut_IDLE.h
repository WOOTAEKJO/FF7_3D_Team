#pragma once
#include "Boss/Bahamut/State/State_Bahamut.h"

#define  ULTIMATECOUNT 5	// �ñر������ ī��Ʈ ��
#define	 COUNTNUM 9			// ī��Ʈ ���ϱ����� ���� ����
#define	 SPECIALNUM 4		// ����� �����ϱ� ������ ���� ����

#define	MIDDLEDISTANCE	18	// �߰��Ÿ�
#define CLOSEDISTANCE  8	// �����Ÿ�

#define MELEEATTACKCOUNT 2 // ���� ���� ī��Ʈ
#define LONGATTACKCOUNT 2  // ���Ÿ� ���� ī��Ʈ 
#define SPINATTACKCOUNT 2  // ���� ���� ī��Ʈ

/*
	���Ϲ�Ʈ��� ������ ���� �� �ϳ��� IDLE ���� Ŭ�����Դϴ�.
	CState_Bahamut Ŭ������ ��� �޾ҽ��ϴ�.
*/

BEGIN(Client)

class CState_Bahamut_IDLE final : public CState_Bahamut
{
	INFO_CLASS(CState_Bahamut_IDLE, CState_Bahamut)

public:
	explicit CState_Bahamut_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Bahamut_IDLE() = default;

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
	_uint					m_iUltimateCount = { 0 }; // �ñر���� ���� ī��Ʈ�� ��Ÿ��
	_uint					m_iPatternCount = { 0 };	// ������ ��� ���Ҵ��� �ľ��ϱ� ����
	
private:
	_uint					m_iRandomNum = { 0 };				// ���Ͽ� ����� ������ ���ڸ� ����

private:
	_uint					m_iLong_Distance_Attack_Count = { 0 };	// ���Ÿ� ������ ������� üũ
	_uint					m_iSpinRush_Count = { 0 };	// ���� ���� ������� üũ
	_uint					m_iMelee_Count = { 0 };	// ���� ���� ������� üũ

private:
	virtual void		Free();

};

END