#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_Mask : public CImgui_Window
{
private:
	CImgui_Window_Mask(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_Mask() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	void	Set_MaskArrayTag(_uint iIndx, const wstring& strMaskTag) { m_strMaskArrayTag[iIndx] = strMaskTag; }
	void	Reset_MaskArrayTag();

private:
	wstring m_strMaskArrayTag[18];

public:
	static CImgui_Window_Mask* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
