#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_EffectGroup.h"

#include "Imgui_Manager.h"

#include "ImGuiFileDialog.h"
#include "Imgui_Tab_ParticleEdit.h"
#include "Imgui_Tab_TrailEdit.h"
#include "Imgui_Tab_EffectEdit.h"

#include "Data_Manager.h"

#include "Effect_Manager.h"

CImgui_Window_EffectGroup::CImgui_Window_EffectGroup(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{

}

HRESULT CImgui_Window_EffectGroup::Initialize()
{

#if EFFECT_LOAD == 0 || EFFECT_LOAD ==2
	m_pEffectGroup = CEffect_Manager::GetInstance()->Get_EffectGroup();
#endif

	return S_OK;
}

void CImgui_Window_EffectGroup::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_EffectGroup::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar;//| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 400.f,400.f };
	string strName = "Group";

	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("GroupEffect", tab_bar_flags))
	{
		Group_Render();

		ImGui::EndTabBar();
	}

	ImGui::End();

	return S_OK;
}

shared_ptr<CEffect> CImgui_Window_EffectGroup::Get_CurrentEffect()
{
	if (m_pEffectGroup == nullptr)
		return nullptr;

	shared_ptr<CEffect> pEffect = (*m_pEffectGroup->Get_GroupList())[m_iGroupListIndex].pEffect;

	return pEffect;
}

void CImgui_Window_EffectGroup::Group_Render()
{
	if (ImGui::Button("Save_Group"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveGroup", "Save Group File", ".json", "../Bin/Data/Effect_Data/Group/");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load_Group"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("LoadGroup", "Load Group File", ".json", "../Bin/Data/Effect_Data/Group/");
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset_Proto_Data"))
	{
		GET_SINGLE(CEffect_Manager)->Reset_Data_Prototype(m_pDevice,m_pContext);
		if (m_pEffectGroup)
		{
			m_iGroupListIndex = 0;
			m_pEffectGroup->Reset_Group();
			m_bModify = false;
		}
	}

	if (ImGuiFileDialog::Instance()->Display("SaveGroup"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pEffectGroup);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadGroup"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			if (m_pEffectGroup != nullptr)
				m_pEffectGroup->Reset_Group();

			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
				StrToWstr(CUtility_File::Get_FileName(filePathName)), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pEffectGroup))))
				return;

			CEffect_Manager::GetInstance()->Set_EffectGroup(m_pEffectGroup);

			m_iOwnerTypeRadio = m_pEffectGroup->Get_OwnerType();	// 오너 타입

			m_iGroupListIndex = 0; // 그룹 이펙트 인덱스 초기화

			m_bGizmo = false;
		}
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::Separator();
	ImVec2 vSize = ImVec2(200, 100);
	if (m_pEffectGroup)
	{
		if (ImGui::BeginListBox("GroupList", vSize))
		{
			_uint iSize = m_pEffectGroup->Get_GroupList()->size();
			for (_uint i = 0; i < iSize; i++)
			{
				string  str = (*m_pEffectGroup->Get_GroupList())[i].strFileName;
				string  str2 = to_string((*m_pEffectGroup->Get_GroupList())[i].fStartTime);
				str = "[" + str + "_" + to_string(i) + " : " + str2 + "]";
				if (ImGui::Selectable(str.c_str(), m_iGroupListIndex == i))
				{
					m_iGroupListIndex = i;
					m_bModify = false;
				}
			}
			ImGui::EndListBox();
		}

		if (!(*m_pEffectGroup->Get_GroupList()).empty())
		{
			ImGui::SeparatorText("Survival");
			if (m_pEffectGroup->Get_IsEffectDead())
				ImGui::Text("Effect Dead");
			else
				ImGui::Text("Effect Alive");

			ImGui::SeparatorText("ObjPoolNum");
			_int iObjPoolNum = m_pEffectGroup->Get_ObjPool_MaxNum();
			ImGui::InputInt("PoolNum", &iObjPoolNum);
			m_pEffectGroup->Set_ObjPool_MaxNum(iObjPoolNum);

			// 오너의 타입을 정함
			ImGui::SeparatorText("OwnerType");
			ImGui::RadioButton("None", &m_iOwnerTypeRadio, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Nor", &m_iOwnerTypeRadio, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Part", &m_iOwnerTypeRadio, 2);
			ImGui::SameLine();
			ImGui::RadioButton("Effect", &m_iOwnerTypeRadio, 3);
			
			m_pEffectGroup->Set_OwnerType((CEffect::USE_TYPE)m_iOwnerTypeRadio);

			ImGui::Separator();
			if (ImGui::Button("Reset_Group"))
			{
				m_iGroupListIndex = 0;
				m_pEffectGroup->Reset_Group();
				m_bModify = false;
				return;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete_Effect"))
			{
				m_pEffectGroup->Delete_Effect(m_iGroupListIndex);

				m_bModify = false;

				if (m_iGroupListIndex != 0)
					m_iGroupListIndex -= 1;
				return;
			}

			ImGui::Separator();
			if (ImGui::Button("Play"))
			{
				m_pEffectGroup->Set_Update(true);
			}

			ImGui::SameLine();

			if (ImGui::Button("Stop"))
			{
				m_pEffectGroup->Set_Update(false);
			}

			if (ImGui::Button("Reset"))
			{
				m_pEffectGroup->Reset_Effect(true);
			}

			ImGui::Separator();
			m_pImgui_Manger->InputFloat("StartTime", &(*m_pEffectGroup->Get_GroupList())[m_iGroupListIndex].fStartTime);

			ImGui::Separator();
			m_pImgui_Manger->InputFloat("AddTime", &m_fApplyTime);
			if (ImGui::Button("All_Add_Time_Apply"))
			{
				m_pEffectGroup->All_Add_Time(m_fApplyTime);
				m_fApplyTime = 0.f;
			}

			ImGui::Separator();
			ImGui::Checkbox("Gizmo", &m_bGizmo);
			if (m_bGizmo)
			{
				ImGui::SameLine();
				ImGui::Checkbox("Modify", &m_bModify);

				CEffect_Group::EFFECT_GROUP_DESC Desc = {};
				Desc = (*m_pEffectGroup->Get_GroupList())[m_iGroupListIndex];
				m_eCurrentModifyType = Desc.eEffectType;
				Use_ImGuizmo(Desc.pEffect, true);
			}

			ImGui::Separator();
			ImGui::Checkbox("GroupGizmo", &m_bGropuGizmo);
			if (m_bGropuGizmo)
			{
				Use_ImGuizmo(m_pEffectGroup, true);
			}
		}
	}
}

CImgui_Window_EffectGroup* CImgui_Window_EffectGroup::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_EffectGroup* pInstance = new CImgui_Window_EffectGroup(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_EffectGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_EffectGroup::Free()
{
	__super::Free();
}

#endif // DEBUG
