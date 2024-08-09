#pragma once
#include "PartObject.h"
#include "Client_Defines.h"

#include "IStatusInterface.h"
#include "StatusComp.h"

BEGIN(Client)

class CBoss_Parts abstract : public CPartObject, public IStatusInterface
{
	INFO_CLASS(CBoss_Parts, CPartObject)

protected:
	CBoss_Parts(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBoss_Parts(const CBoss_Parts& rhs);
	virtual ~CBoss_Parts() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_uint	m_iShaderPassIndex = { 0 };

protected:
	_uint	m_iCommand = { 0 };

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual	HRESULT	Ready_State() { return S_OK; }

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	GETSET_EX2(_uint, m_iCommand, Command, GET, SET)

#pragma region IStatusInterface
public:
	// GameObject중에 Status를 가지는 하위 클래스를 찾아 스테이터스를 전달 받습니다.
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) override { return S_OK; }
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) override { return S_OK; }
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override;
	virtual weak_ptr<class CStatusComp> Get_StatusCom() override { return m_pStatusCom; }

protected:
	shared_ptr<CStatusComp>		m_pStatusCom = { nullptr };
#pragma endregion

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END