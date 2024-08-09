#ifdef _DEBUG
#pragma once
#include "Imgui_Tab_EffectTabBase.h"
#include "ImGuizmo.h"
#include "VIBuffer_Instancing.h"
#include "Particle.h"
#include "Imgui_Window_Mask.h"

BEGIN(Client)

class CParticle;

class CImgui_Tab_ParticleEdit : public CImgui_Tab_EffectTabBase
{
private:
	CImgui_Tab_ParticleEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_ParticleEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CParticle> m_pParticle = { nullptr };

private:
	shared_ptr<CVIBuffer_Instancing> m_pVIBufferCom = { nullptr };
	CVIBuffer_Instancing::INSTANCING_DESC m_InstancingDesc;

private:
	_int	m_iControlType = { 0 };

	/* For. ModelMesh*/
	_int m_iPickIndex_ModelTag = { 0 };

	/* For. MoveControlType*/
	_int m_iMoveControlType = { 0 };

	/* For. DeathCondition*/
	_int m_iDeathType = { 0 };

	/* For. MeshStartPoint*/
	_int m_iMeshStartPointType = { 0 };

private:
	CParticle::PARTICLE_DESC m_tParticle_Desc;

private:
	CImgui_Window_Mask* m_pMask_Window = { nullptr };

public:
	static CImgui_Tab_ParticleEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

#pragma region 그룹

public: // 수정전용
	void	Set_Effect(shared_ptr<CParticle> pEffect);
	void	Remove_Effect();

#pragma endregion

};

END

#endif // DEBUG
