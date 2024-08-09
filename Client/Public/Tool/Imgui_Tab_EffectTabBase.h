#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"
#include "ImGuizmo.h"

#include "Imgui_Window_EffectEdit.h"

#define COLORBLENDTYPENUM 10

BEGIN(Client)

class CParticle;

class CImgui_Tab_EffectTabBase abstract : public CImgui_Tab
{
public:
	
protected:
	CImgui_Tab_EffectTabBase(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_EffectTabBase() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

protected:
	vector<wstring> m_PrototypeList[CImgui_Window_EffectEdit::EFFECT_TEXTURE_LIST_TYPE::ET_END];

protected:

	_int	m_iPickIndex_PrtTag[5] = { 0,0,0,0,0}; // ����Ʈ�� Index�� ����

	_int	m_iTexTypeRadio = { 0 }; // �ؽ��� ����
	_int	m_iUseTypeRadio = { 0 }; // ���� ���� ����
	 
	_int	m_iNumLoad = { 0 }; // ���� ������Ÿ���� ����

protected:
	vector<wstring>	m_vecFilter;
	_int	m_iPickIndex_Filter[5] = {0,0,0,0,0};
	string	m_strFilterTemp[5] = {};

protected:
	void	ComboBox(const string& strTitle, const char** strList, _uint iListSize, _uint* iEasingType);

public:
	void	Copy_Effect_Texture_List(CImgui_Window_EffectEdit::EFFECT_TEXTURE_LIST_TYPE eType, 
		vector<wstring>& vecProtoList);

	void	ResourceList();

public:
	virtual void	Free() override;

#pragma region �׷�

protected:
	_bool	m_bModify = { false }; // ��������
	_bool	m_bLoad = { false };
	string	m_strLoadFilePath;

#pragma endregion

public:
	static	const char* g_strColorBlendType[];
	static	const char* g_strListTag[];

};

END

#endif // DEBUG
