#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_EffectTabBase.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"

#include "Utility_File.h"
#include "Texture_Data.h"

#include "Imgui_Tab_EffectEdit.h"

#include "Effect_Manager.h"

const char* CImgui_Tab_EffectTabBase::g_strColorBlendType[] = { "Multiply","Screen","Overlay","Plus","Burn",
	"VividLight","SoftLight","HardLight","Dodge","MixBurn","Strong_Overlay"};

const char* CImgui_Tab_EffectTabBase::g_strListTag[] = { "Diffuse","Mask","Noise","Dissolve","Model" };

CImgui_Tab_EffectTabBase::CImgui_Tab_EffectTabBase(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab(pGameObjectList, pDevice, pContext)
{

}

HRESULT CImgui_Tab_EffectTabBase::Initialize()
{

	return S_OK;
}

void CImgui_Tab_EffectTabBase::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_EffectTabBase::Render()
{

	return S_OK;
}

void CImgui_Tab_EffectTabBase::ComboBox(const string& strTitle, const char** strList, _uint iListSize, _uint* iEasingType)
{
	if (ImGui::BeginCombo(strTitle.c_str(), strList[*iEasingType]))
	{
		for (_uint n = 0; n < iListSize; n++)
		{
			_bool bSelected = (*iEasingType == n);
			if (ImGui::Selectable(strList[n], bSelected))
			{
				*iEasingType = n;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void CImgui_Tab_EffectTabBase::Copy_Effect_Texture_List(CImgui_Window_EffectEdit::EFFECT_TEXTURE_LIST_TYPE eType,
	vector<wstring>& vecProtoList)
{
	switch (eType)
	{
	case Client::CImgui_Window_EffectEdit::ET_DIFFUSE:
		m_PrototypeList[eType] = vecProtoList;
		break;
	case Client::CImgui_Window_EffectEdit::ET_MASK:
		m_PrototypeList[eType] = vecProtoList;
		break;
	case Client::CImgui_Window_EffectEdit::ET_NOISE:
		m_PrototypeList[eType] = vecProtoList;
		break;
	case Client::CImgui_Window_EffectEdit::ET_DISSOLVE:
		m_PrototypeList[eType] = vecProtoList;
		break;
	case Client::CImgui_Window_EffectEdit::ET_MODEL:
		m_PrototypeList[eType] = vecProtoList;
		break;
	default:
		break;
	}
}

void CImgui_Tab_EffectTabBase::ResourceList()
{

}

void CImgui_Tab_EffectTabBase::Free()
{
	__super::Free();
}

#endif // DEBUG
