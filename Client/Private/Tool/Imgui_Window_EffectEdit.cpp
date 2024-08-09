#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_EffectEdit.h"

#include "Imgui_Manager.h"

#include "Imgui_Tab_ParticleEdit.h"
#include "Imgui_Tab_TrailEdit.h"
#include "Imgui_Tab_EffectEdit.h"
#include "Imgui_Tab_TrailBufferEdit.h"

#include "Imgui_Window_EffectGroup.h"

#include "Imgui_Tab_EffectTabBase.h"

CImgui_Window_EffectEdit::CImgui_Window_EffectEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_EffectEdit::Initialize()
{
	Load_Effect_Texture(L"../Bin/Resources/Textures/Effect/Diffuse/",ET_DIFFUSE);
	Load_Effect_Texture(L"../Bin/Resources/Textures/Effect/Mask/", ET_MASK);
	Load_Effect_Texture(L"../Bin/Resources/Textures/Effect/Noise/", ET_NOISE);
	Load_Effect_Texture(L"../Bin/Resources/Textures/Effect/Dissolve/", ET_DISSOLVE);
	Load_Effect_Texture(L"../Bin/Resources/Model/Effect/", ET_MODEL);

	sort(m_PrototypeList[ET_DIFFUSE].begin(), m_PrototypeList[ET_DIFFUSE].end());
	sort(m_PrototypeList[ET_MASK].begin(),	m_PrototypeList[ET_MASK].end());
	sort(m_PrototypeList[ET_NOISE].begin(), m_PrototypeList[ET_NOISE].end());
	sort(m_PrototypeList[ET_DISSOLVE].begin(), m_PrototypeList[ET_DISSOLVE].end());
	sort(m_PrototypeList[ET_MODEL].begin(), m_PrototypeList[ET_MODEL].end());

	m_pParticleEditTab = CImgui_Tab_ParticleEdit::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTrailEditTab = CImgui_Tab_TrailEdit::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pEffectEditTab = CImgui_Tab_EffectEdit::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTrailBufferEditTab = CImgui_Tab_TrailBufferEdit::Create(*m_pGameObjectList, m_pDevice, m_pContext);

	Save_Effect_Texture_List(m_pParticleEditTab);
	Save_Effect_Texture_List(m_pTrailEditTab);
	Save_Effect_Texture_List(m_pEffectEditTab);
	Save_Effect_Texture_List(m_pTrailBufferEditTab);

	m_pEffectGroupWindow = CImgui_Window_EffectGroup::Create(*m_pGameObjectList, m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Window_EffectEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_EffectEdit::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 350.f,700.f };
	string strName = "EffectEdit";


	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	Modify();
	if (!m_bModifyCheck)
	{
		if (ImGui::BeginTabBar("Effect", tab_bar_flags))
		{
			if (m_bParticleEditTabOn = ImGui::BeginTabItem("Particle_Edit"))
			{
				m_eCurrentEffectType = EFFECT_TYPE::EFFECT_PARTICLE;

				ImGui::EndTabItem();
			}

			if (m_bParticleEditTabOn = ImGui::BeginTabItem("Mesh_Edit"))
			{
				m_eCurrentEffectType = EFFECT_TYPE::EFFECT_MESH;

				ImGui::EndTabItem();
			}

			if (m_bParticleEditTabOn = ImGui::BeginTabItem("Trail_Edit"))
			{
				m_eCurrentEffectType = EFFECT_TYPE::EFFECT_TRAIL;

				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}
	}
	else {
		ImGui::Text("Modify");
		ImGui::Separator();
	}

	Effect_Render();

	m_pEffectGroupWindow->Render();

	ImGui::End();

	return S_OK;
}

void CImgui_Window_EffectEdit::Modify()
{
	if (m_pEffectGroupWindow == nullptr)
		return;

	if (m_pEffectGroupWindow->Is_Modify())
	{
		if (!m_bModifyCheck)
		{
			m_bModifyCheck = true;

			switch (m_pEffectGroupWindow->Get_CurrentModifyType())
			{
			case EFFECT_TYPE::EFFECT_PARTICLE:
				m_eCurrentEffectType = EFFECT_PARTICLE;
				m_pParticleEditTab->Set_Effect(dynamic_pointer_cast<CParticle>(m_pEffectGroupWindow->Get_CurrentEffect()));
				break;
			case EFFECT_TYPE::EFFECT_RECT:
				m_eCurrentEffectType = EFFECT_RECT;
				m_pEffectEditTab->Set_Effect(dynamic_pointer_cast<CSprite>(m_pEffectGroupWindow->Get_CurrentEffect()));
				break;
			case EFFECT_TYPE::EFFECT_MESH:
				m_eCurrentEffectType = EFFECT_MESH;
				m_pTrailEditTab->Set_Effect(dynamic_pointer_cast<CTrail_Effect>(m_pEffectGroupWindow->Get_CurrentEffect()));
				break;
			case EFFECT_TYPE::EFFECT_TRAIL:
				m_eCurrentEffectType = EFFECT_TRAIL;
				m_pTrailBufferEditTab->Set_Effect(dynamic_pointer_cast<CTrail_Buffer>(m_pEffectGroupWindow->Get_CurrentEffect()));
				break;
			default:
				break;
			}
		}
	}
	else {

		if (m_bModifyCheck)
		{
			m_bModifyCheck = false;
			switch (m_eCurrentEffectType)
			{
			case EFFECT_TYPE::EFFECT_PARTICLE:
				m_pParticleEditTab->Remove_Effect();
				break;
			case EFFECT_TYPE::EFFECT_RECT:
				m_pEffectEditTab->Remove_Effect();
				break;
			case EFFECT_TYPE::EFFECT_MESH:
				m_pTrailEditTab->Remove_Effect();
				break;
			case EFFECT_TYPE::EFFECT_TRAIL:
				m_pTrailBufferEditTab->Remove_Effect();
				break;
			default:
				break;
			}
		}
	}
}

void CImgui_Window_EffectEdit::Effect_Render()
{
	switch (m_eCurrentEffectType)
	{
	case Client::EFFECT_PARTICLE:
		m_pParticleEditTab->Render();
		break;
	case Client::EFFECT_RECT:
		m_pEffectEditTab->Render();
		break;
	case Client::EFFECT_MESH:
		m_pTrailEditTab->Render();
		break;
	case Client::EFFECT_TRAIL:
		m_pTrailBufferEditTab->Render();
		break;
	default:
		break;
	}
}

void CImgui_Window_EffectEdit::Load_Effect_Texture(wstring strMiddlePath, EFFECT_TEXTURE_LIST_TYPE eType)
{
	
	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(strMiddlePath))
	{
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;
		_bool	bNumCheck = false;
	
		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		string strExc[3] = { ".amodel" ,".dds" ,".png" };

		_uint iCheckNum(0);

		for (_uint i = 0; i < 3; i++)
		{
			if (!Compare_Str(strTmp.c_str(), strExc[i]))
				iCheckNum++;
		}

		if(iCheckNum >= 3)
			continue;

		vector<wstring> vecPath = SplitWstr(entry.path(), L'/');
		vector<wstring> vecElement;

		if (vecPath.size() < 3)
			continue;
		// 사이즈가 3 이상이 아니면 컨티뉴

		for (auto& iter : vecPath)
		{
			vector<wstring> strTemp = SplitWstr(iter, L'\\');
			for (auto& iter2 : strTemp)
			{
				vecElement.push_back(iter2);
			}
		} // 한 번 더 짤라준다.

		switch (eType)
		{
		case Client::CImgui_Window_EffectEdit::ET_DIFFUSE:
			if (Compare_Wstr(vecElement[5], TEXT("Diffuse")))
				m_PrototypeList[eType].push_back(StrToWstr(szFileName));
			break;
		case Client::CImgui_Window_EffectEdit::ET_MASK:
			if (Compare_Wstr(vecElement[5], TEXT("Mask")))
				m_PrototypeList[eType].push_back(StrToWstr(szFileName));
			break;
		case Client::CImgui_Window_EffectEdit::ET_NOISE:
			if (Compare_Wstr(vecElement[5], TEXT("Noise")))
				m_PrototypeList[eType].push_back(StrToWstr(szFileName));
			break;
		case Client::CImgui_Window_EffectEdit::ET_DISSOLVE:
			if (Compare_Wstr(vecElement[5], TEXT("Dissolve")))
				m_PrototypeList[eType].push_back(StrToWstr(szFileName));
			break;
		case Client::CImgui_Window_EffectEdit::ET_MODEL:
			if (Compare_Wstr(vecElement[4], TEXT("Effect")))
				m_PrototypeList[eType].push_back(StrToWstr(szFileName));
			break;
		default:
			break;
		}
	}
}

void CImgui_Window_EffectEdit::Save_Effect_Texture_List(CImgui_Tab_EffectTabBase* pTab)
{
	pTab->Copy_Effect_Texture_List(ET_DIFFUSE,m_PrototypeList[ET_DIFFUSE]);
	pTab->Copy_Effect_Texture_List(ET_MASK,m_PrototypeList[ET_MASK]);
	pTab->Copy_Effect_Texture_List(ET_NOISE,m_PrototypeList[ET_NOISE]);
	pTab->Copy_Effect_Texture_List(ET_DISSOLVE,m_PrototypeList[ET_DISSOLVE]);
	pTab->Copy_Effect_Texture_List(ET_MODEL,m_PrototypeList[ET_MODEL]);
}

CImgui_Window_EffectEdit* CImgui_Window_EffectEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_EffectEdit* pInstance = new CImgui_Window_EffectEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_EffectEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_EffectEdit::Free()
{
	Safe_Release(m_pParticleEditTab);
	Safe_Release(m_pEffectEditTab);
	Safe_Release(m_pTrailEditTab);
	Safe_Release(m_pTrailBufferEditTab);

	Safe_Release(m_pEffectGroupWindow);
	__super::Free();
}

#endif // DEBUG
