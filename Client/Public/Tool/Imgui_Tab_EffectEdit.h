#ifdef _DEBUG
#pragma once
#include "Imgui_Tab_EffectTabBase.h"
#include "ImGuizmo.h"
#include "VIBuffer_Instancing.h"
#include "Sprite.h"

BEGIN(Client)

class CImgui_Tab_EffectEdit : public CImgui_Tab_EffectTabBase
{
private:
	CImgui_Tab_EffectEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_EffectEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CSprite> m_pEffect = { nullptr };

private:
	CSprite::EFFECT_RECT_DESC m_tEffect_Desc;

public:
	static CImgui_Tab_EffectEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

#pragma region 그룹

public: // 수정전용
	void	Set_Effect(shared_ptr<CSprite> pEffect);
	void	Remove_Effect();

#pragma endregion

};

END

#endif // DEBUG
