#ifdef _DEBUG
#pragma once
#include "Imgui_Tab_EffectTabBase.h"
#include "Trail_Buffer.h"
#include "VIBuffer_Trail.h"

BEGIN(Client)

class CImgui_Tab_TrailBufferEdit : public CImgui_Tab_EffectTabBase
{
private:
	CImgui_Tab_TrailBufferEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_TrailBufferEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	shared_ptr<CTrail_Buffer> m_pTrailBufferEffect = { nullptr };
	shared_ptr<CVIBuffer_Trail> m_pBuffer = { nullptr };

private:
	CTrail_Buffer::TRAILBUFFER_DESC m_TrailBufferDesc;
	CVIBuffer_Trail::VIBUFFER_TRAIL_DESC m_BufferDesc;

private:
	_int m_iPickIndex_Object = { -1 };
	_int m_iMaxCntTemp = { 1 };
	_int m_iLerPointNum = { 1 };

private:
	_int	m_iBoneSelect_index = { -1 };	// 본 인덱스

private:
	LAYERTYPE	m_eLayerType = { LAYERTYPE::LAYERTYPE_END };

private:
	_bool	m_bModelGizmo = { false };
	shared_ptr<CGameObject>	m_pModel = { nullptr };

public:
	static CImgui_Tab_TrailBufferEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

#pragma region 그룹

public: // 수정전용
	void	Set_Effect(shared_ptr<CTrail_Buffer> pEffect);
	void	Remove_Effect();

#pragma endregion

};

END

#endif // DEBUG
