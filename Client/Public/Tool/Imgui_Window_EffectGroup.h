#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"
#include "Effect_Group.h"

#include "Level_Test_Defines.h"

BEGIN(Client)

class CImgui_Window_EffectGroup : public CImgui_Window
{
private:
	CImgui_Window_EffectGroup(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_EffectGroup() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	_bool	Is_Modify() { return m_bModify; }
	EFFECT_TYPE	Get_CurrentModifyType() { return m_eCurrentModifyType; }
	shared_ptr<CEffect>	Get_CurrentEffect();

public:
	shared_ptr<CEffect_Group>	Get_EffectGroup() { return m_pEffectGroup; }

private:
	shared_ptr<CEffect_Group> m_pEffectGroup = { nullptr };

private:
	_uint	m_iGroupListIndex = { 0 };
	_bool	m_bModify = { false };
	EFFECT_TYPE	m_eCurrentModifyType = { EFFECT_TYPE::EFFECT_TYPE_END };
	_uint	m_iNumLoad = { 0 };

	_bool	m_bOwnerFollow = { false };

private:
	_float	m_fApplyTime = { 0.f };

private:
	_bool	m_bGropuGizmo = { false };

private:
	_int	m_iOwnerTypeRadio = { 0 };

private:
	void	Group_Render();

public:
	static CImgui_Window_EffectGroup* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
