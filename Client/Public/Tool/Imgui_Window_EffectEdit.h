#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_EffectGroup;
class CImgui_Tab_EffectTabBase;

class CImgui_Window_EffectEdit : public CImgui_Window
{

public:
	enum EFFECT_TEXTURE_LIST_TYPE {
		ET_DIFFUSE,ET_MASK,ET_NOISE,ET_DISSOLVE,ET_MODEL,ET_END
	};

private:
	CImgui_Window_EffectEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_EffectEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	class CImgui_Tab_ParticleEdit* m_pParticleEditTab = { nullptr };
	class CImgui_Tab_TrailEdit* m_pTrailEditTab = { nullptr };
	class CImgui_Tab_EffectEdit* m_pEffectEditTab = { nullptr };
	class CImgui_Tab_TrailBufferEdit* m_pTrailBufferEditTab = { nullptr };

private:
	CImgui_Window_EffectGroup* m_pEffectGroupWindow = { nullptr };

private:
	EFFECT_TYPE	m_eCurrentEffectType = { EFFECT_TYPE::EFFECT_PARTICLE };
	_bool		m_bModifyCheck = { false };

private:
	vector<wstring> m_PrototypeList[ET_END];

public:
	_bool	m_bParticleEditTabOn = true;

	class CImgui_Tab_ParticleEdit* Get_ParticleTab() { return m_pParticleEditTab; }
	class CImgui_Tab_TrailEdit* Get_TrailTab() { return m_pTrailEditTab; }
	class CImgui_Tab_EffectEdit* Get_EffectTab() { return m_pEffectEditTab; }
	class CImgui_Tab_TrailBufferEdit* Get_TrailBufferTab() { return m_pTrailBufferEditTab; }

private:
	void	Modify();
	void	Effect_Render();

	void	Load_Effect_Texture(wstring strMiddlePath,EFFECT_TEXTURE_LIST_TYPE eType);
	void	Save_Effect_Texture_List(CImgui_Tab_EffectTabBase* pTab);

public:
	static CImgui_Window_EffectEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
