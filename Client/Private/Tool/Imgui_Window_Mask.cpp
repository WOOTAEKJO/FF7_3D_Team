#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_Mask.h"

#include "Imgui_Manager.h"

#include "Imgui_Tab_ParticleEdit.h"
#include "Imgui_Tab_TrailEdit.h"
#include "Imgui_Tab_EffectEdit.h"

CImgui_Window_Mask::CImgui_Window_Mask(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{

}

HRESULT CImgui_Window_Mask::Initialize()
{

	return S_OK;
}

void CImgui_Window_Mask::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_Mask::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 200.f,300.f };
	string strName = "Mask";


	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MaskImage", tab_bar_flags))
	{
		for (_uint i = 0; i < 18; i++)
		{
			string strText = "MaskNum :" + to_string(i);
			ImGui::Text(strText.c_str());
			m_pImgui_Manger->Load_Image(m_strMaskArrayTag[i], _float2(100.f, 100.f));
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	return S_OK;
}

void CImgui_Window_Mask::Reset_MaskArrayTag()
{
	for (_uint i = 0; i < 18; i++)
	{
		m_strMaskArrayTag[i] = TEXT("");
	}
}

CImgui_Window_Mask* CImgui_Window_Mask::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_Mask* pInstance = new CImgui_Window_Mask(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_Mask");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_Mask::Free()
{
	__super::Free();
}

#endif // 
