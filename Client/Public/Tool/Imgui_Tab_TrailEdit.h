#ifdef _DEBUG
#pragma once
#include "Imgui_Tab_EffectTabBase.h"
#include "Trail_Effect.h"
 
BEGIN(Client)

class CImgui_Tab_TrailEdit : public CImgui_Tab_EffectTabBase
{

private:
	CImgui_Tab_TrailEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_TrailEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	shared_ptr<CTrail_Effect> m_pTrailEffect = { nullptr };
private:
	CTrail_Effect::TRAIL_DESC m_TrailDesc;

private:
	_int m_iParticleType = { 0 };

	wstring m_strModelTag = {};
	// ----- DeadType -------

	_int	m_iDeadType = { 0 };

private:
	_int    m_iSelectedEffect_Index = { -1 };

public:
	static CImgui_Tab_TrailEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

#pragma region 그룹

public: // 수정전용
	void	Set_Effect(shared_ptr<CTrail_Effect> pEffect);
	void	Remove_Effect();

#pragma endregion



};

END

#endif // DEBUG
