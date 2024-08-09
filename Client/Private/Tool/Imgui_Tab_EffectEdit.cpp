#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_EffectEdit.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"
#include "Utility_File.h"

#include "Effect_Manager.h"

const char* strEffectSpriteTypes[] = { "Diffuse","Mask" };
const char* strEffectTypes[] = { "Default","Trail","Depth","Fire" };

CImgui_Tab_EffectEdit::CImgui_Tab_EffectEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab_EffectTabBase(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_EffectEdit::Initialize()
{
	
	return S_OK;
}

void CImgui_Tab_EffectEdit::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_EffectEdit::Render()
{

	if (ImGui::Button("Save_Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveEffect", "Save Effect File", ".json", "../Bin/Data/Effect_Data/Rect/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveEffect"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pEffect);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (!m_bModify)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load_Effect"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadEffect", "Load Effect File", ".json", "../Bin/Data/Effect_Data/Rect/");
		}

		if (ImGuiFileDialog::Instance()->Display("LoadEffect"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string filePathName = m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();


				if (m_pEffect != nullptr)
					m_pEffect->TurnOn_State(OBJSTATE::WillRemoved);

				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("ToolEffect_") + to_wstring(m_iNumLoad),
					CSprite::Create(m_pDevice, m_pContext, filePathName))))
					RETURN_EFAIL;

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
					TEXT("ToolEffect_") + to_wstring(m_iNumLoad), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pEffect))))
					RETURN_EFAIL;

				m_tEffect_Desc = m_pEffect->Get_Effect_Desc();

				m_iUseTypeRadio = m_tEffect_Desc.eUseType;

				m_bLoad = true;

				++m_iNumLoad;
			}
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Separator();
		ImVec2 vSize = ImVec2(200, 100);

		// 0 Diffuse, 1 Mask, 2 Noise, 3 Dissolve, 4 Model
		ImGui::RadioButton("Diffuse", &m_iTexTypeRadio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Mask", &m_iTexTypeRadio, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Noise", &m_iTexTypeRadio, 2);

		if (ImGui::BeginListBox(g_strListTag[m_iTexTypeRadio], vSize))
		{
			for (int i = 0; i < m_PrototypeList[m_iTexTypeRadio].size(); i++)
			{
				wstring wstrTag = m_PrototypeList[m_iTexTypeRadio][i];
				wstrTag = L"[" + wstrTag + L"] ";

				string  str;
				str = WstrToStr(wstrTag);
				if (ImGui::Selectable(str.c_str(), m_iPickIndex_PrtTag[m_iTexTypeRadio] == i))
				{
					m_iPickIndex_PrtTag[m_iTexTypeRadio] = i;
					switch (m_iTexTypeRadio)
					{
					case 0:
						m_tEffect_Desc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][i];
						break;
					case 1:
						m_tEffect_Desc.strMask = m_PrototypeList[m_iTexTypeRadio][i];
						break;
					case 2:
						m_tEffect_Desc.strNoise = m_PrototypeList[m_iTexTypeRadio][i];
						break;
					}
				}
			}
			ImGui::EndListBox();
		}

		switch (m_iTexTypeRadio)
		{
		case 0:
			m_pImgui_Manger->Load_Image(m_tEffect_Desc.strDiffuse, _float2(200.f, 200.f));
			break;
		case 1:
			m_pImgui_Manger->Load_Image(m_tEffect_Desc.strMask, _float2(200.f, 200.f));
			break;
		case 2:
			m_pImgui_Manger->Load_Image(m_tEffect_Desc.strNoise, _float2(200.f, 200.f));
			break;
		}

		if (ImGui::Button("Create_Effect"))
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
				TEXT("Prototype_GameObject_Effect_Rect"), &m_tEffect_Desc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pEffect))))
				RETURN_EFAIL;

			m_bLoad = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove_Effect"))
		{
			if (m_pEffect != nullptr)
			{
				m_pEffect->TurnOn_State(OBJSTATE::WillRemoved);
				m_pEffect = nullptr;
			}

			m_bLoad = false;
		}
	}

	ImGui::Separator();
	if (m_tEffect_Desc.strDiffuse.size() > 0)
		ImGui::Text(("Diffuse : " + WstrToStr(SplitWstr(m_tEffect_Desc.strDiffuse, L'/').back())).c_str());
	if (m_tEffect_Desc.strMask.size() > 0)
		ImGui::Text(("Mask : " + WstrToStr(SplitWstr(m_tEffect_Desc.strMask, L'/').back())).c_str());
	if (m_tEffect_Desc.strNoise.size() > 0)
		ImGui::Text(("Noise : " + WstrToStr(SplitWstr(m_tEffect_Desc.strNoise, L'/').back())).c_str());

	ImGui::Separator();
	if (m_pEffect != nullptr)
	{
		if (!m_bModify && m_bLoad)
		{
			if (ImGui::Button("AddGroup"))
			{
				CEffect_Manager::GetInstance()->Get_EffectGroup()->Add_Effect(EFFECT_TYPE::EFFECT_RECT, m_strLoadFilePath, m_pEffect);
				m_pEffect = nullptr;
				return S_OK;
			}
		}

		ImGui::Separator();
		ImGui::RadioButton("UseNone", &m_iUseTypeRadio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("UseFollow", &m_iUseTypeRadio, 1);
		ImGui::SameLine();
		m_tEffect_Desc.eUseType = (CEffect::USE_TYPE)m_iUseTypeRadio;

		ImGui::Separator();
		if (ImGui::BeginCombo("ShaderPass_Type", strEffectTypes[m_tEffect_Desc.iShaderPassType]))
		{
			for (int n = 0; n < IM_ARRAYSIZE(strEffectTypes); n++)
			{
				_bool bSelected = (m_tEffect_Desc.iShaderPassType == n);
				if (ImGui::Selectable(strEffectTypes[n], bSelected))
				{
					m_tEffect_Desc.iShaderPassType = n;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("Discard_Alpha", &m_tEffect_Desc.fDiscard_Alpha);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat2("DiffuseUV", &m_tEffect_Desc.vDiffuseUV);
		m_pImgui_Manger->InputFloat2("MaskUV", &m_tEffect_Desc.vMaskUV);
		m_pImgui_Manger->InputFloat2("NoiseUV", &m_tEffect_Desc.vNoiseUV);

		if (m_tEffect_Desc.iShaderPassType == 3)
		{
			ImGui::Separator();
			m_pImgui_Manger->InputFloat3("DistortionValue", &m_tEffect_Desc.vDistortionValue);
			m_pImgui_Manger->InputFloat("DistortinScale", &m_tEffect_Desc.fDistortionScale);
			m_pImgui_Manger->InputFloat("distortionBias", &m_tEffect_Desc.fDistortionBias);
			m_pImgui_Manger->InputFloat("NoiseWeight", &m_tEffect_Desc.fNoiseWeight);
		}

		ImGui::Separator();
		ImGui::Checkbox("Gizmo", &m_bGizmo);
		if (m_bGizmo)
		{
			Use_ImGuizmo(m_pEffect, true);
		}

		ImGui::Separator();
		m_pImgui_Manger->InputFloat3("Position", &m_tEffect_Desc.vInitialPosition);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat2("Size", &m_tEffect_Desc.vSize);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat3("RotationAngle", &m_tEffect_Desc.vRotation);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat4("Solid_Color", &m_tEffect_Desc.vSolid_Color);

		ImGui::Separator();
		ImGui::Checkbox("Sprite", &m_tEffect_Desc.bSpriteAnim);

		if (m_tEffect_Desc.bSpriteAnim)
		{

			ImGui::Separator();
			if (ImGui::BeginCombo("SpriteType", strEffectSpriteTypes[m_tEffect_Desc.iSpriteType]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strEffectSpriteTypes); n++)
				{
					_bool bSelected = (m_tEffect_Desc.iSpriteType == n);
					if (ImGui::Selectable(strEffectSpriteTypes[n], bSelected))
					{
						m_tEffect_Desc.iSpriteType = n;
					}

					if (bSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Loop", &m_tEffect_Desc.bLoop);

			ImGui::Separator();
			m_pImgui_Manger->InputFloat2("ColRow", &m_tEffect_Desc.vSpriteColRow);

			ImGui::Separator();
			m_pImgui_Manger->InputFloat("Duration", &m_tEffect_Desc.fDuration);
			m_pImgui_Manger->InputFloat("Speed", &m_tEffect_Desc.fSpeed);

			if (ImGui::Button("Reset"))
			{
				m_pEffect->Reset_Effect();
			}

		}

		m_pEffect->Set_Effect_Desc(m_tEffect_Desc);
	}

	return S_OK;
}


CImgui_Tab_EffectEdit* CImgui_Tab_EffectEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_EffectEdit* pInstance = new CImgui_Tab_EffectEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_EffectEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_EffectEdit::Free()
{
	__super::Free();
}

void CImgui_Tab_EffectEdit::Set_Effect(shared_ptr<CSprite> pEffect)
{
	if (m_pEffect != nullptr)
	{
		m_pEffect->TurnOn_State(OBJSTATE::WillRemoved);
		m_pEffect = nullptr;
	}

	m_pEffect = pEffect;

	m_tEffect_Desc = m_pEffect->Get_Effect_Desc();

	m_bModify = true;
}

void CImgui_Tab_EffectEdit::Remove_Effect()
{
	m_pEffect = nullptr;

	m_bModify = false;
}

#endif // DEBUG
