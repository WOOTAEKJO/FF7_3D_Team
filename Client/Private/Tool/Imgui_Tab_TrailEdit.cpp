#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_TrailEdit.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"
#include "Utility_File.h"

#include "Effect_Manager.h"

const char* strTrailBufferTypes[] = { "Default","Clamp","Distortion","Dissolve","Effect_RadialBlur"};

CImgui_Tab_TrailEdit::CImgui_Tab_TrailEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab_EffectTabBase(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_TrailEdit::Initialize()
{

	return S_OK;
}

void CImgui_Tab_TrailEdit::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_TrailEdit::Render()
{
	ImGuiColorEditFlags flags;

	flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs;
	// 컬러창을 사용하기 위함

	if (m_pTrailEffect)
	{
		if (m_pTrailEffect->IsState(OBJSTATE::WillRemoved))
			m_pTrailEffect = nullptr;
		// 삭제 예정이면 비워두기
	}

	if (ImGui::Button("Save_Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveEffect", "Save Effect File", ".json", "../Bin/Data/Effect_Data/Trail/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveEffect"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pTrailEffect);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (!m_bModify)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load_Effect"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadEffect", "Load Effect File", ".json", "../Bin/Data/Effect_Data/Trail/");
		}

		if (ImGuiFileDialog::Instance()->Display("LoadEffect"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string filePathName = m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

				if (m_pTrailEffect != nullptr)
					m_pTrailEffect->TurnOn_State(OBJSTATE::WillRemoved);

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(),
					L_EFFECT, StrToWstr(CUtility_File::Get_FileName(filePathName)), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pTrailEffect))))
					RETURN_EFAIL;

				m_pImgui_Manger->Update_List(L_EFFECT);

				m_TrailDesc = *(m_pTrailEffect->Get_TrailDesc());

				m_iDeadType = (_int)m_TrailDesc.eDeadType;	// 죽음 조건
				m_iUseTypeRadio = m_TrailDesc.eUseType;		// 오너 타입

				m_bLoad = true;
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
		ImGui::SameLine();
		ImGui::RadioButton("Dissolve", &m_iTexTypeRadio, 3);
		ImGui::SameLine();
		ImGui::RadioButton("Model", &m_iTexTypeRadio, 4);

		char* strInputText = "";
		strInputText = (char*)m_strFilterTemp[m_iTexTypeRadio].c_str();
		string sss;
		ImGui::InputText("Search", strInputText, 100);
		m_strFilterTemp[m_iTexTypeRadio] = strInputText;
		wstring strFindTempW = StrToWstr(m_strFilterTemp[m_iTexTypeRadio]);
		m_vecFilter.clear(); // 검색기능을 위함

		if (ImGui::TreeNode("TotalList"))
		{
			if (ImGui::BeginListBox(g_strListTag[m_iTexTypeRadio], vSize))
			{
				if (!m_PrototypeList[m_iTexTypeRadio].empty())
				{
					if (m_pGameInstance->Key_Down(DIK_DOWN) && m_iPickIndex_PrtTag[m_iTexTypeRadio] < m_PrototypeList[m_iTexTypeRadio].size() - 1)
					{
						m_iPickIndex_PrtTag[m_iTexTypeRadio]++;

						switch (m_iTexTypeRadio)
						{
						case 0:
							m_TrailDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailDesc.strMask= m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 3:
							m_TrailDesc.strDissolve = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 4:
							m_TrailDesc.strModelTag = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						}
					}
					if (m_pGameInstance->Key_Down(DIK_UP) && m_iPickIndex_PrtTag[m_iTexTypeRadio] > 0 && m_iPickIndex_PrtTag[m_iTexTypeRadio] <= m_PrototypeList[m_iTexTypeRadio].size() - 1)
					{
						m_iPickIndex_PrtTag[m_iTexTypeRadio]--;

						switch (m_iTexTypeRadio)
						{					
						case 0:
							m_TrailDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailDesc.strMask = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 3:
							m_TrailDesc.strDissolve = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 4:
							m_TrailDesc.strModelTag = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						}
					}
				}

				_uint iListSize = m_PrototypeList[m_iTexTypeRadio].size();
				for (int i = 0; i < iListSize; i++)
				{
					wstring wstrTag = m_PrototypeList[m_iTexTypeRadio][i];

					if (!Compare_Wstr(strFindTempW, L""))
					{
						size_t found = wstrTag.find(strFindTempW);
						if (found != std::wstring::npos)
						{
							m_vecFilter.push_back(wstrTag);
						}// 검색시 있으면 추가
					}

					wstrTag = L"[" + wstrTag + L"] ";

					string  str;
					str = WstrToStr(wstrTag);
					if (ImGui::Selectable(str.c_str(), m_iPickIndex_PrtTag[m_iTexTypeRadio] == i))
					{
						m_iPickIndex_PrtTag[m_iTexTypeRadio] = i;
						switch (m_iTexTypeRadio)
						{
						case 0:
							m_TrailDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 1:
							m_TrailDesc.strMask = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 2:
							m_TrailDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 3:
							m_TrailDesc.strDissolve = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 4:
							m_strModelTag = m_PrototypeList[m_iTexTypeRadio][i];
							m_TrailDesc.strModelTag = m_strModelTag;
							break;
						}
					}
				}
				ImGui::EndListBox();
			}
		ImGui::TreePop();
		}
		else {
			for (auto strTag : m_PrototypeList[m_iTexTypeRadio])
			{

				if (!Compare_Wstr(strFindTempW, L""))
				{
					size_t found = strTag.find(strFindTempW);
					if (found != std::wstring::npos)
					{
						m_vecFilter.push_back(strTag);
					}// 검색시 있으면 추가
				}
			}
		}

		if (!m_vecFilter.empty())
		{// 검색한 애들만 모아서 출력
			if (ImGui::BeginListBox("Search", vSize))
			{
				_uint iFilterSize = m_vecFilter.size();
				for (int i = 0; i < iFilterSize; i++)
				{
					wstring wstrTag = m_vecFilter[i];

					wstrTag = L"[" + wstrTag + L"] ";

					string  str;
					str = WstrToStr(wstrTag);
					if (ImGui::Selectable(str.c_str(), m_iPickIndex_Filter[m_iTexTypeRadio] == i))
					{

						m_iPickIndex_Filter[m_iTexTypeRadio] = i;
						switch (m_iTexTypeRadio)
						{
						case 0:
							m_TrailDesc.strDiffuse = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailDesc.strMask = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailDesc.strNoise = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 3:
							m_TrailDesc.strDissolve = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 4:
							m_strModelTag = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							m_TrailDesc.strModelTag = m_strModelTag;
							break;
						}
					}
				}

				ImGui::EndListBox();
			}
		}

		switch (m_iTexTypeRadio)
		{
		case 0:
			m_pImgui_Manger->Load_Image(m_TrailDesc.strDiffuse, _float2(200.f, 200.f));
			break;
		case 1:
			m_pImgui_Manger->Load_Image(m_TrailDesc.strMask, _float2(200.f, 200.f));
			break;
		case 2:
			m_pImgui_Manger->Load_Image(m_TrailDesc.strNoise, _float2(200.f, 200.f));
			break;
		case 3:
			m_pImgui_Manger->Load_Image(m_TrailDesc.strDissolve, _float2(200.f, 200.f));
			break;
		}

		if (ImGui::Button("Create_Effect"))
		{
			if (m_pTrailEffect)
				return S_OK;

			if (Compare_Wstr(m_TrailDesc.strModelTag, L""))
				return S_OK;

			if (Compare_Wstr(m_TrailDesc.strDiffuse, L""))
				m_TrailDesc.strDiffuse = TEXT("0Effect_Diffuse_None");

			if (Compare_Wstr(m_TrailDesc.strMask, L""))
				m_TrailDesc.strMask = TEXT("0Effect_Mask_None");

			if (Compare_Wstr(m_TrailDesc.strNoise, L""))
				m_TrailDesc.strNoise = TEXT("0Effect_Noise_None");

			if (Compare_Wstr(m_TrailDesc.strDissolve, L""))
				m_TrailDesc.strDissolve = TEXT("0Effect_Dissolve_None");

			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
				TEXT("Prototype_GameObject_TrailEffect"), &m_TrailDesc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pTrailEffect))))
				RETURN_EFAIL;

			m_bLoad = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove_Effect"))
		{
			if (m_pTrailEffect != nullptr)
			{
				m_pTrailEffect->TurnOn_State(OBJSTATE::WillRemoved);
				m_pTrailEffect = nullptr;
			}

			m_bLoad = false;
		}
	}

	ImGui::Separator();
	if (m_TrailDesc.strModelTag.size() > 0)
		ImGui::Text(("Model : " + WstrToStr(SplitWstr(m_TrailDesc.strModelTag, L'/').back())).c_str());
	if (m_TrailDesc.strDiffuse.size() > 0)
		ImGui::Text(("Diffuse : " + WstrToStr(SplitWstr(m_TrailDesc.strDiffuse, L'/').back())).c_str());
	if (m_TrailDesc.strMask.size() > 0)
		ImGui::Text(("Mask : " + WstrToStr(SplitWstr(m_TrailDesc.strMask, L'/').back())).c_str());
	if (m_TrailDesc.strNoise.size() > 0)
		ImGui::Text(("Noise : " + WstrToStr(SplitWstr(m_TrailDesc.strNoise, L'/').back())).c_str());
	if (m_TrailDesc.strDissolve.size() > 0)
		ImGui::Text(("Dissolve : " + WstrToStr(SplitWstr(m_TrailDesc.strDissolve, L'/').back())).c_str());

	ImGui::Separator();
	if (m_pTrailEffect != nullptr)
	{
		ImGui::SeparatorText("Survival");
		if (m_pTrailEffect->Get_IsEffectDead())
			ImGui::Text("Effect Dead");
		else
			ImGui::Text("Effect Alive");
		HelpMarker(u8"생존여부");

		ImGui::SeparatorText("ObjPoolNum");
		_int iObjPoolNum = m_pTrailEffect->Get_ObjPool_MaxNum();
		ImGui::InputInt("PoolNum", &iObjPoolNum);
		m_pTrailEffect->Set_ObjPool_MaxNum(iObjPoolNum);
		HelpMarker(u8"오브젝트 풀 클론 생성 갯수 설정");

		ImGui::Separator();
		if (!m_bModify && m_bLoad)
		{
			if (ImGui::Button("AddGroup"))
			{
				CEffect_Manager::GetInstance()->Get_EffectGroup()->Add_Effect(EFFECT_TYPE::EFFECT_MESH, m_strLoadFilePath, m_pTrailEffect);
				m_pTrailEffect = nullptr;
				return S_OK;
			}
		}

		ImGui::SeparatorText("OwnerType");
		ImGui::RadioButton("UNone", &m_iUseTypeRadio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("UNor", &m_iUseTypeRadio, 1);
		ImGui::SameLine();
		ImGui::RadioButton("UPart", &m_iUseTypeRadio, 2);
		ImGui::SameLine();
		ImGui::RadioButton("UEffect", &m_iUseTypeRadio, 3);
		m_TrailDesc.eUseType = (CEffect::USE_TYPE)m_iUseTypeRadio;
		// 오너 타입 설정

		ImGui::Separator();
		if (ImGui::BeginCombo("ShaderPass_Type", strTrailBufferTypes[m_TrailDesc.iShaderPassType]))
		{
			for (int n = 0; n < IM_ARRAYSIZE(strTrailBufferTypes); n++)
			{
				_bool bSelected = (m_TrailDesc.iShaderPassType == n);
				if (ImGui::Selectable(strTrailBufferTypes[n], bSelected))
				{
					m_TrailDesc.iShaderPassType = n;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		_bool bTransformCheck = m_pTrailEffect->Get_ApplyTransform();
		ImGui::Checkbox("ApplyTransform", &bTransformCheck);
		m_pTrailEffect->Set_ApplyTransform(bTransformCheck);

		if (ImGui::Button("Play"))
		{
			m_pTrailEffect->Set_Play(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_pTrailEffect->Set_Play(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			m_pTrailEffect->Reset_Effect(true);
		}
		if (m_pGameInstance->Key_Down(DIK_N))
		{
			m_pTrailEffect->Reset_Effect(true);
		}

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("Discard_Alpha", &m_TrailDesc.fDiscard_Alpha);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat2("DiffuseUV", &m_TrailDesc.vDiffuseUV);
		m_pImgui_Manger->InputFloat2("MaskUV", &m_TrailDesc.vMaskUV);
		m_pImgui_Manger->InputFloat2("NoiseUV", &m_TrailDesc.vNoiseUV);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("DiffuseWeigh", &m_TrailDesc.vWeight.x);
		m_pImgui_Manger->InputFloat("MaskWeight", &m_TrailDesc.vWeight.y);
		m_pImgui_Manger->InputFloat("NoiseWeight", &m_TrailDesc.vWeight.z);

		ImGui::Separator();
		ImGui::Checkbox("DiffuseClamp", &m_TrailDesc.bDiffuseClamp);
		ImGui::Checkbox("MaskClamp", &m_TrailDesc.bMaskClamp);
		ImGui::Checkbox("NoiseClamp", &m_TrailDesc.bNoiseClamp);

		if (m_TrailDesc.iShaderPassType == 2)
		{
			/*ImGui::Separator();
			m_pImgui_Manger->InputFloat3("DistortionValue", &m_TrailDesc.vDistortionValue);
			m_pImgui_Manger->InputFloat("DistortinScale", &m_TrailDesc.fDistortionScale);
			m_pImgui_Manger->InputFloat("distortionBias", &m_TrailDesc.fDistortionBias);
			m_pImgui_Manger->InputFloat("DisNoiseWeight", &m_TrailDesc.fNoiseWeight);*/
		}
		else if (m_TrailDesc.iShaderPassType == 3)
		{
			ImGui::SeparatorText("DissolveSetUp");
			m_pImgui_Manger->InputFloat("AmountSpeed", &m_TrailDesc.fDissolveAmountSpeed);
			m_pImgui_Manger->InputFloat("GradiationDistanceSpeed", &m_TrailDesc.fDissolveGradiationDistanceSpeed);
			ImGui::ColorEdit3("StartColor", (float*)&m_TrailDesc.vDissolveGradiationStartColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine();
			ImGui::ColorEdit3("EndColor", (float*)&m_TrailDesc.vDissolveGradiationEndColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);

			ImGui::Checkbox("DissolveParticle", &m_TrailDesc.bDissolveParticle);
			if (m_TrailDesc.bDissolveParticle)
			{
				auto DataPathes = CUtility_File::CRef_DataPathes();
				vector<string> EffectFileName;
				for (auto iter = DataPathes.begin(); iter != DataPathes.end(); ++iter)
				{
					if ((*iter).second.find(StrToWstr("Particle")) != wstring::npos)
						EffectFileName.push_back(WstrToStr((*iter).first));
				}

				string strDisplay;
				if (m_iSelectedEffect_Index == -1)
				{
					strDisplay = u8"선택해 주세요";
				}
				else
					strDisplay = EffectFileName[m_iSelectedEffect_Index];

				if (ImGui::BeginCombo(u8"이펙트 이름 선택하기", strDisplay.c_str()))
				{
					for (_uint i = 0; i < EffectFileName.size(); i++)
					{
						_bool bIsSelected = (m_iSelectedEffect_Index == i);
						if (ImGui::Selectable((EffectFileName[i]).c_str(), bIsSelected))
						{
							m_iSelectedEffect_Index = i;
						}
						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				m_TrailDesc.strDissolveParticleTag = ConvertToWstring(strDisplay);

				ImGui::Text("MeshStartPoint");
				_int iEnum(0);
				iEnum = (_int)m_TrailDesc.eDissolveParticleStartType;
				ImGui::RadioButton("Zero", &iEnum, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Order", &iEnum, 1);
				ImGui::SameLine();
				ImGui::RadioButton("Random", &iEnum, 2);
				m_TrailDesc.eDissolveParticleStartType = (CVIBuffer_Instancing::MESH_START_TYPE)iEnum;
			}
		}
		else if (m_TrailDesc.iShaderPassType == 4)
		{// RadialBlur
			
		}

		ImGui::Separator();
		ImGui::Checkbox("MaskAlphaInverse", &m_TrailDesc.bMaskInverse);
		
		ImGui::Separator();
		ImGui::Checkbox("MoveUV_Y", &m_TrailDesc.bMoveUV_Y);
		m_pImgui_Manger->Arrow_Button("UVStart", 0.1f, &m_TrailDesc.fUVStart);
		m_pImgui_Manger->Arrow_Button("UVSpeed", 0.1f, &m_TrailDesc.fUVSpeed);
		m_pImgui_Manger->Arrow_Button("UVForce", 0.1f, &m_TrailDesc.fUVForce);

		ImGui::SeparatorText("LifeTime");
		m_pImgui_Manger->InputFloat("LifeTime", &m_TrailDesc.fLifeTime);

		ImGui::SeparatorText("Move");

		m_pImgui_Manger->InputFloat3("InitSpeed", &m_TrailDesc.vMove_Speed);
		m_TrailDesc.vMove_Speed.x = Max(m_TrailDesc.vMove_Speed.x, m_TrailDesc.vMove_Speed.x);
		m_TrailDesc.vMove_Speed.y = Max(m_TrailDesc.vMove_Speed.y, m_TrailDesc.vMove_Speed.y);
		m_TrailDesc.vMove_Speed.z = Max(m_TrailDesc.vMove_Speed.z, m_TrailDesc.vMove_Speed.z);
		m_pImgui_Manger->InputFloat3("Move_Force", &m_TrailDesc.vMove_Force);
		m_pImgui_Manger->InputFloat3("Move_Speed_Min", &m_TrailDesc.vMove_MinSpeed);
		m_pImgui_Manger->InputFloat3("Move_Speed_Max", &m_TrailDesc.vMove_MaxSpeed);
		m_TrailDesc.vMove_MaxSpeed.x = Max(m_TrailDesc.vMove_MinSpeed.x, m_TrailDesc.vMove_MaxSpeed.x);
		m_TrailDesc.vMove_MaxSpeed.y = Max(m_TrailDesc.vMove_MinSpeed.y, m_TrailDesc.vMove_MaxSpeed.y);
		m_TrailDesc.vMove_MaxSpeed.z = Max(m_TrailDesc.vMove_MinSpeed.z, m_TrailDesc.vMove_MaxSpeed.z);

		ImGui::SeparatorText("Scale");
		m_pImgui_Manger->InputFloat3("InitScale", &m_TrailDesc.vScale);
		m_TrailDesc.vScale.x = Max(m_TrailDesc.vScale.x, m_TrailDesc.vScale.x);
		m_TrailDesc.vScale.y = Max(m_TrailDesc.vScale.y, m_TrailDesc.vScale.y);
		m_TrailDesc.vScale.z = Max(m_TrailDesc.vScale.z, m_TrailDesc.vScale.z);
		m_pImgui_Manger->InputFloat3("Scale_Speed", &m_TrailDesc.vScale_Speed);
		m_pImgui_Manger->InputFloat3("Scale_Force", &m_TrailDesc.vScale_Force);
		m_pImgui_Manger->InputFloat3("Scale_Min", &m_TrailDesc.vScale_Min);
		m_pImgui_Manger->InputFloat3("Scale_Max", &m_TrailDesc.vScale_Max);
		m_TrailDesc.vScale_Max.x = Max(m_TrailDesc.vScale_Min.x, m_TrailDesc.vScale_Max.x);
		m_TrailDesc.vScale_Max.y = Max(m_TrailDesc.vScale_Min.y, m_TrailDesc.vScale_Max.y);
		m_TrailDesc.vScale_Max.z = Max(m_TrailDesc.vScale_Min.z, m_TrailDesc.vScale_Max.z);

		ImGui::SeparatorText("Rotation");
		ImGui::Checkbox("IsRotation", &m_TrailDesc.bRotation);

		if (m_TrailDesc.bRotation)
		{
			m_pImgui_Manger->InputFloat3("InitRotation", &m_TrailDesc.vRotation);
			m_TrailDesc.vRotation.x = Max(m_TrailDesc.vRotation.x, m_TrailDesc.vRotation.x);
			m_TrailDesc.vRotation.y = Max(m_TrailDesc.vRotation.y, m_TrailDesc.vRotation.y);
			m_TrailDesc.vRotation.z = Max(m_TrailDesc.vRotation.z, m_TrailDesc.vRotation.z);

			m_pImgui_Manger->InputFloat3("Rotation_Speed", &m_TrailDesc.vRotation_Speed);
			m_pImgui_Manger->InputFloat3("Rotation_Force", &m_TrailDesc.vRotation_Force);
			m_pImgui_Manger->InputFloat3("Rotation_Min", &m_TrailDesc.vRotation_Min);
			m_pImgui_Manger->InputFloat3("Rotation_Max", &m_TrailDesc.vRotation_Max);
			m_TrailDesc.vRotation_Max.x = Max(m_TrailDesc.vRotation_Min.x, m_TrailDesc.vRotation_Max.x);
			m_TrailDesc.vRotation_Max.y = Max(m_TrailDesc.vRotation_Min.y, m_TrailDesc.vRotation_Max.y);
			m_TrailDesc.vRotation_Max.z = Max(m_TrailDesc.vRotation_Min.z, m_TrailDesc.vRotation_Max.z);

			ImGui::Checkbox("AxisUse", &m_TrailDesc.bCurAxisUse);
			if (m_TrailDesc.bCurAxisUse)
			{
				m_pImgui_Manger->InputFloat3("AxisDir", &m_TrailDesc.vAxisDir);
				m_pImgui_Manger->InputFloat2("AxisAngleMinMax", &m_TrailDesc.vAxisAngleMinMax);
				m_pImgui_Manger->InputFloat("AxisAngleSpeed", &m_TrailDesc.fAxiAngleSpeed);
				m_pImgui_Manger->InputFloat("AxisAngleForce", &m_TrailDesc.fAxiAngleForce);
			}
		}

		ImGui::SeparatorText("Color");
		HelpMarker(u8"디스토션 사용시 컬러 알파 값으로 조절가능.");
		ImGui::ColorEdit4("InitColor", (float*)&m_TrailDesc.vColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);
		m_pImgui_Manger->InputFloat4("Color_Speed", &m_TrailDesc.vColor_Speed);
		m_pImgui_Manger->InputFloat4("Color_Force", &m_TrailDesc.vColor_Force);
		m_pImgui_Manger->InputFloat4("Color_Max", &m_TrailDesc.vColor_Max);
		m_TrailDesc.vColor_Max.x = Max(0.f, m_TrailDesc.vColor_Max.x);
		m_TrailDesc.vColor_Max.y = Max(0.f, m_TrailDesc.vColor_Max.y);
		m_TrailDesc.vColor_Max.z = Max(0.f, m_TrailDesc.vColor_Max.z);
		m_TrailDesc.vColor_Max.w = Max(0.f, m_TrailDesc.vColor_Max.w);

		_uint iColorBlendType = m_TrailDesc.eColorBlendType;
		ComboBox("ColorBlendType", CImgui_Tab_EffectTabBase::g_strColorBlendType, COLORBLENDTYPENUM,
			&iColorBlendType);
		m_TrailDesc.eColorBlendType = (CEffect::COLORBLEND)iColorBlendType;

		ImGui::SeparatorText("DeadType");
		ImGui::RadioButton("DTime", &m_iDeadType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("DScale", &m_iDeadType, 1);
		ImGui::SameLine();
		ImGui::RadioButton("DColor", &m_iDeadType, 2);
		ImGui::SameLine();
		ImGui::RadioButton("DRange", &m_iDeadType, 3);
		ImGui::RadioButton("DOwner", &m_iDeadType, 4);
		m_TrailDesc.eDeadType = (CTrail_Effect::DEADTYPE)m_iDeadType;
		// 죽음 조건 설정

		ImGui::Separator();
		ImGui::Checkbox("Gizmo", &m_bGizmo);
		if (m_bGizmo)
		{
			Use_ImGuizmo(m_pTrailEffect, true);
		}

		if (m_pGameInstance->Key_Down(DIK_Y))
		{
			if (m_bGizmo)
				m_bGizmo = false;
			else
				m_bGizmo = true;
		}

		m_pTrailEffect->Set_TrailDesc(m_TrailDesc);
	}

	return S_OK;
}


CImgui_Tab_TrailEdit* CImgui_Tab_TrailEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_TrailEdit* pInstance = new CImgui_Tab_TrailEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_TrailEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_TrailEdit::Free()
{
	__super::Free();
}

void CImgui_Tab_TrailEdit::Set_Effect(shared_ptr<CTrail_Effect> pEffect)
{
	if (m_pTrailEffect != nullptr)
	{
		m_pTrailEffect->TurnOn_State(OBJSTATE::WillRemoved);
		m_pTrailEffect = nullptr;

	}

	m_pTrailEffect = pEffect;

	m_pImgui_Manger->Update_List(L_EFFECT);

	m_TrailDesc = *(m_pTrailEffect->Get_TrailDesc());

	m_iDeadType = (_int)m_TrailDesc.eDeadType;

	m_bModify = true;
}

void CImgui_Tab_TrailEdit::Remove_Effect()
{
	m_pTrailEffect = nullptr;
	m_bModify = false;
}

#endif // DEBUG
