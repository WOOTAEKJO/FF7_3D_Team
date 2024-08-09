#pragma once
#include "VIBuffer_Instancing.h"

class ENGINE_DLL CVIBuffer_Particle_Rect : public CVIBuffer_Instancing
{
public:
	typedef struct Particle_Rect_Desc : public CVIBuffer_Instancing::INSTANCING_DESC
	{

	}PARTICLE_RECT_DESC;

private:
	CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& rhs);
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static shared_ptr<CVIBuffer_Particle_Rect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

