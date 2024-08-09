#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_TrailBufferEdit.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"
#include "Utility_File.h"

#include "Effect_Manager.h"

#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "BoneAnimContainer.h"
#include "GameInstance.h"
#include "CommonModelComp.h"
#include "BoneContainer.h"
#include "Model_Manager.h"

#include "Imgui_Window_AnimMainEdit.h"
#include "Imgui_Tab_AnimObject.h"
#include "AnimObject.h"

#include "ModelContainer.h"

const char* strTrailTypes[] = { "Default","Trail","Distortion" };

CImgui_Tab_TrailBufferEdit::CImgui_Tab_TrailBufferEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab_EffectTabBase(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_TrailBufferEdit::Initialize()
{
	m_eLayerType = LAYERTYPE::L_PLAYER;

	m_eLayerType = LAYERTYPE::L_PLAYER;

	return S_OK;
}

void CImgui_Tab_TrailBufferEdit::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_TrailBufferEdit::Render()
{
	if (m_pTrailBufferEffect)
	{
		if (m_pTrailBufferEffect->IsState(OBJSTATE::WillRemoved))
			m_pTrailBufferEffect = nullptr;
		// ���� �����̸� ����α�
	}

	auto pAnimEdit = m_pImgui_Manger->Find_Window<CImgui_Window_AnimEdit>(CImgui_Manager::TOOL_ANIM);
	auto pAnimObjectTab = pAnimEdit->Tab_AnimObject();

	if (ImGui::Button("Save_Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveEffect", "Save Effect File", ".json", "../Bin/Data/Effect_Data/TrailBuffer/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveEffect"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pTrailBufferEffect);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (!m_bModify)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load_Effect"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadEffect", "Load Effect File", ".json", "../Bin/Data/Effect_Data/TrailBuffer/");
		}

		if (ImGuiFileDialog::Instance()->Display("LoadEffect"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				if (m_iPickIndex_Object != -1)
				{

					m_TrailBufferDesc.pOwner = (*m_pGameObjectList[m_eLayerType])[m_iPickIndex_Object];
					// ���� ����

					if (m_TrailBufferDesc.pOwner == nullptr)
						return S_OK;

					string filePathName = m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

					if (m_pTrailBufferEffect != nullptr)
						m_pTrailBufferEffect->TurnOn_State(OBJSTATE::WillRemoved);
					// ���� Ʈ���� ���� ó��

					if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(),
						L_EFFECT, StrToWstr(CUtility_File::Get_FileName(filePathName)), &m_TrailBufferDesc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pTrailBufferEffect))))
						RETURN_EFAIL;
					// ����

					m_pBuffer = dynamic_pointer_cast<CVIBuffer_Trail>(m_pTrailBufferEffect->Get_VIBufferCom().lock());
					// ���ο� ���� ����

					m_TrailBufferDesc = (m_pTrailBufferEffect->Get_TrailBuffer_Desc());

					m_BufferDesc = m_pBuffer->Get_Buffer_Desc();

					m_iMaxCntTemp = m_BufferDesc.iMaxCnt;
					m_iLerPointNum = m_BufferDesc.iLerpPointNum;

					m_iUseTypeRadio = m_TrailBufferDesc.eUseType; // ���� Ÿ���� �޾ƿ´�.

					m_bLoad = true;
				}
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

		char* strInputText = "";
		strInputText = (char*)m_strFilterTemp[m_iTexTypeRadio].c_str();
		string sss;
		ImGui::InputText("Search", strInputText, 100);
		m_strFilterTemp[m_iTexTypeRadio] = strInputText;
		wstring strFindTempW = StrToWstr(m_strFilterTemp[m_iTexTypeRadio]);
		m_vecFilter.clear(); // �˻������ ����

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
							m_TrailBufferDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailBufferDesc.strMask = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailBufferDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						}
					}
					if (m_pGameInstance->Key_Down(DIK_UP) && m_iPickIndex_PrtTag[m_iTexTypeRadio] > 0 && m_iPickIndex_PrtTag[m_iTexTypeRadio] <= m_PrototypeList[m_iTexTypeRadio].size() - 1)
					{
						m_iPickIndex_PrtTag[m_iTexTypeRadio]--;

						switch (m_iTexTypeRadio)
						{
						case 0:
							m_TrailBufferDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailBufferDesc.strMask = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailBufferDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						}
					}
				}

				size_t iTextureVecSize = m_PrototypeList[m_iTexTypeRadio].size();
				for (int i = 0; i < iTextureVecSize; i++)
				{
					wstring wstrTag = m_PrototypeList[m_iTexTypeRadio][i];

					if (!Compare_Wstr(strFindTempW, L""))
					{
						size_t found = wstrTag.find(strFindTempW);
						if (found != std::wstring::npos)
						{
							m_vecFilter.push_back(wstrTag);
						}// �˻��� ������ �߰�
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
							m_TrailBufferDesc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 1:
							m_TrailBufferDesc.strMask = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 2:
							m_TrailBufferDesc.strNoise = m_PrototypeList[m_iTexTypeRadio][i];
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
					}// �˻��� ������ �߰�
				}
			}
		}
		
		if (!m_vecFilter.empty())
		{// �˻��� �ֵ鸸 ��Ƽ� ���
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
							m_TrailBufferDesc.strDiffuse = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 1:
							m_TrailBufferDesc.strMask = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 2:
							m_TrailBufferDesc.strNoise = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
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
			m_pImgui_Manger->Load_Image(m_TrailBufferDesc.strDiffuse, _float2(200.f, 200.f));
			break;
		case 1:
			m_pImgui_Manger->Load_Image(m_TrailBufferDesc.strMask, _float2(200.f, 200.f));
			break;
		case 2:
			m_pImgui_Manger->Load_Image(m_TrailBufferDesc.strNoise, _float2(200.f, 200.f));
			break;
		}

#pragma region ModelList

		ImGui::SeparatorText("OwnerSetUp");
		ImGui::RadioButton("UNor", &m_iUseTypeRadio, 1);
		ImGui::SameLine();
		ImGui::RadioButton("UPart", &m_iUseTypeRadio, 2);
		ImGui::SameLine();
		ImGui::RadioButton("UEffect", &m_iUseTypeRadio, 3);
		m_TrailBufferDesc.eUseType = (CTrail_Buffer::USE_TYPE)m_iUseTypeRadio;
		// ���� Ÿ�� ����

		if (m_iUseTypeRadio == 1)
			m_eLayerType = LAYERTYPE::L_PLAYER; // �ִϸ��̼� ������ ��� �ִ� ��ü
		else if (m_iUseTypeRadio == 2)
			m_eLayerType = LAYERTYPE::L_NPC; // �ִϸ��̼� ��ü�� ������ �ִ� ���� ��ü
		else if (m_iUseTypeRadio == 3)
			m_eLayerType = LAYERTYPE::L_EFFECT; // ����Ʈ���� ��� ��ü
		// �Ϲ����� ��ü�� �׳� ������ ����� �ؾ��ҵ�

		// �� Ÿ���� ���⼭ ����

		m_pImgui_Manger->Update_List(m_eLayerType);
		ImGui::Separator();
		if (!(m_pGameObjectList[m_eLayerType])->empty())
		{
			if (ImGui::BeginListBox("ObjectList", vSize))
			{
				size_t iObjectListSize = (*m_pGameObjectList[m_eLayerType]).size();
				for (int i = 0; i < iObjectListSize; i++)
				{
					wstring wstrTag = {};
					vector<wstring> vecTemp;
					if ((m_iUseTypeRadio == 1) || (m_iUseTypeRadio == 2))
					{
						vecTemp = SplitWstr((*m_pGameObjectList[m_eLayerType])[i]
							->Get_ModelCom().lock()->Get_ModelPath(), L'/');
						// ���� �� �𵨵��� �� ������Ʈ�� ���� �ִٰ� ����
					}
					else {
						vecTemp = SplitWstr((*m_pGameObjectList[m_eLayerType])[i]->Get_PrototypeTag(), L'_');
						// �� ������Ʈ�� ���� �ʴ� ����Ʈ �޽��� ���
					}

					wstrTag = vecTemp[vecTemp.size() - 1];
					wstrTag = L"[" + wstrTag + L"] ";

					string  str;
					str = WstrToStr(wstrTag);
					if (ImGui::Selectable(str.c_str(), m_iPickIndex_Object == i))
					{
						m_iPickIndex_Object = i;

						m_pModel = (*m_pGameObjectList)[m_eLayerType][m_iPickIndex_Object];
					}
				}
				ImGui::EndListBox();
			}

#pragma endregion

			ImGui::Separator();
			m_pImgui_Manger->InputFloat3("Pos_0", &m_TrailBufferDesc.vPos_0);
			m_BufferDesc.vPos_0 = m_TrailBufferDesc.vPos_0;
			m_pImgui_Manger->InputFloat3("Pos_1", &m_TrailBufferDesc.vPos_1);
			m_BufferDesc.vPos_1 = m_TrailBufferDesc.vPos_1;
			ImGui::InputInt("MaxCnt", &m_iMaxCntTemp);
			if (m_iMaxCntTemp < 1)
				m_iMaxCntTemp = 1;
			m_TrailBufferDesc.iMaxCnt = m_iMaxCntTemp;
			m_BufferDesc.iMaxCnt = m_TrailBufferDesc.iMaxCnt;

			ImGui::InputInt("LerPointNum", &m_iLerPointNum);
			if (m_iLerPointNum < 1)
				m_iLerPointNum = 1;
			m_TrailBufferDesc.iLerpPointNum = m_iLerPointNum;
			m_BufferDesc.iLerpPointNum = m_TrailBufferDesc.iLerpPointNum;

#pragma region �� ����

			ImGui::Checkbox("Socket", &m_TrailBufferDesc.bSocket);
			if (m_TrailBufferDesc.bSocket)
			{

				ImGui::SeparatorText(u8"��");
				const auto& pBoneGroup = m_pModel->Get_ModelCom().lock()->Get_BoneGroup();

				if (pBoneGroup)
				{
					

					ImGui::SetNextItemWidth(280.f);

					// �� ���� �ε�
					_uint iNumBoneCount = pBoneGroup->CRef_BoneDatas_Count();
					vector<FBoneData*>& BoneDatas = pBoneGroup->Get_BoneDatasVector();
					vector<string> BoneNames;
					BoneNames.reserve(iNumBoneCount);
					for (auto iter = BoneDatas.begin(); iter != BoneDatas.end(); ++iter)
					{
						BoneNames.push_back(ConvertToString((*iter)->strName));
					}
					sort(BoneNames.begin(), BoneNames.end());

					string strPreviewBone;
					if (iNumBoneCount == 0)
					{
						strPreviewBone = "����";
						m_iBoneSelect_index = -1;
					}
					else
					{
						if (m_iBoneSelect_index == -1)
							m_iBoneSelect_index = 0;
						strPreviewBone = BoneNames[m_iBoneSelect_index];
					}

					ImVec2 vSize = ImVec2(280, 100);
					if (ImGui::TreeNode("BoneList"))
					{
						if (ImGui::BeginListBox(u8"1", vSize))
						{
							if (m_pGameInstance->Key_Down(DIK_DOWN) && m_iBoneSelect_index < Cast<_int>(iNumBoneCount - 1))
							{
								m_iBoneSelect_index++;
								m_TrailBufferDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
							}
							if (m_pGameInstance->Key_Down(DIK_UP) && m_iBoneSelect_index > 0)
							{
								m_iBoneSelect_index--;
								m_TrailBufferDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
							}
							for (_uint i = 0; i < iNumBoneCount; i++)
							{
								_bool bIsSelected = (m_iBoneSelect_index == i);
								if (ImGui::Selectable((BoneNames[i]).c_str(), bIsSelected))
								{
									m_iBoneSelect_index = i;
									m_TrailBufferDesc.strBoneName = StrToWstr(BoneNames[m_iBoneSelect_index]);
									// �� �̸� ���
								}
								if (bIsSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndListBox();
						}
						ImGui::TreePop();
					}
				}
			}
#pragma  endregion

			if (ImGui::Button("Create_Effect"))
			{
				if (m_pTrailBufferEffect)
					return S_OK;

				if (m_iPickIndex_Object != -1)
				{
					m_TrailBufferDesc.pOwner = (*m_pGameObjectList[m_eLayerType])[m_iPickIndex_Object];

					auto pModelCom = m_pModel->Get_ModelCom().lock();

					if (pModelCom)
					{
						auto pBoneGroup = pModelCom->Get_BoneGroup();
						if (pBoneGroup)
							m_TrailBufferDesc.pBoneGroup = pBoneGroup;
						// �� �׷� �߰�
					}

					if (Compare_Wstr(m_TrailBufferDesc.strDiffuse, L""))
						m_TrailBufferDesc.strDiffuse = TEXT("0Effect_Diffuse_None");

					if (Compare_Wstr(m_TrailBufferDesc.strNoise, L""))
						m_TrailBufferDesc.strNoise = TEXT("0Effect_Noise_None");

					if (Compare_Wstr(m_TrailBufferDesc.strMask, L""))
					{
						m_TrailBufferDesc.strMask = TEXT("0Effect_Mask_None");
					}

					if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
						TEXT("Prototype_GameObject_TrailBuffer"), &m_TrailBufferDesc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pTrailBufferEffect))))
						RETURN_EFAIL;

					m_pBuffer = dynamic_pointer_cast<CVIBuffer_Trail>(m_pTrailBufferEffect->Get_VIBufferCom().lock());

					m_bLoad = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove_Effect"))
			{
				if (m_pTrailBufferEffect != nullptr)
				{
					m_pTrailBufferEffect->TurnOn_State(OBJSTATE::WillRemoved);
					m_pTrailBufferEffect = nullptr;
				}

				m_bLoad = false;
			}
		}
	}

	ImGui::Separator();
	if (m_TrailBufferDesc.strDiffuse.size() > 0)
		ImGui::Text(("Diffuse : " + WstrToStr(SplitWstr(m_TrailBufferDesc.strDiffuse, L'/').back())).c_str());
	if (m_TrailBufferDesc.strMask.size() > 0)
		ImGui::Text(("Mask : " + WstrToStr(SplitWstr(m_TrailBufferDesc.strMask, L'/').back())).c_str());
	if (m_TrailBufferDesc.strNoise.size() > 0)
		ImGui::Text(("Noise : " + WstrToStr(SplitWstr(m_TrailBufferDesc.strNoise, L'/').back())).c_str());

	ImGui::Separator();
	if (m_pTrailBufferEffect != nullptr)
	{
		if (m_bLoad)
		{
			const auto& pBoneGroup = m_pTrailBufferEffect->Get_TrailBuffer_Desc().pBoneGroup;

			if ((m_TrailBufferDesc.pBoneGroup ==  nullptr) &&
				pBoneGroup)
			{
				m_TrailBufferDesc.pBoneGroup = pBoneGroup;
			} // �ε� �� ���׷��� ����ߴ� �������̸� �� �׷��� ä���ش�.
			// Ʈ���� ����Ʈ ��ü���� �� �׷��� ä���ִ� ���� BeginPlay���� ���ֱ� ����.
		}

		ImGui::SeparatorText("Survival");
		if (m_pTrailBufferEffect->Get_IsEffectDead())
			ImGui::Text("Effect Dead");
		else
			ImGui::Text("Effect Alive");
		HelpMarker(u8"��������");

		ImGui::SeparatorText("ObjPoolNum");
		_int iObjPoolNum = m_pTrailBufferEffect->Get_ObjPool_MaxNum();
		ImGui::InputInt("PoolNum", &iObjPoolNum);
		m_pTrailBufferEffect->Set_ObjPool_MaxNum(iObjPoolNum);
		HelpMarker(u8"������Ʈ Ǯ Ŭ�� ���� ���� ����");

		ImGui::Separator();
		if (!m_bModify && m_bLoad)
		{
			if (ImGui::Button("AddGroup"))
			{
				CEffect_Manager::GetInstance()->Get_EffectGroup()->Add_Effect(EFFECT_TYPE::EFFECT_TRAIL, m_strLoadFilePath, m_pTrailBufferEffect);
				m_pTrailBufferEffect = nullptr;
				return S_OK;
			}
		}

		ImGui::Separator();
		
		if (ImGui::Button("PlayStop") || m_pGameInstance->Key_Down(DIK_N))
		{
			if(m_TrailBufferDesc.bUpdate)
				m_TrailBufferDesc.bUpdate = false;
			else
				m_TrailBufferDesc.bUpdate = true;

			if (m_pModel)
			{
				if (m_TrailBufferDesc.bUpdate)
				{
					if (m_TrailBufferDesc.eUseType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
					{
						m_pModel->TurnOn_State(OBJSTATE::Tick);
						m_pModel->Get_Owner().lock()->TurnOn_State(OBJSTATE::Tick);
						// �����̸� ���ʵ� ���� tick�� Ȱ��ȭ
					}
					else {
						m_pModel->TurnOn_State(OBJSTATE::Tick);
						// �ƴϸ� �� �ϳ��� Ȱ��ȭ
					}
				}
				else {
					if (m_TrailBufferDesc.eUseType == CEffect::USE_TYPE::USE_FOLLOW_PARTS)
					{
						m_pModel->TurnOff_State(OBJSTATE::Tick);
						m_pModel->Get_Owner().lock()->TurnOff_State(OBJSTATE::Tick);
						// �����̸� ���ʵ� ���� tick�� ����
					}
					else {
						m_pModel->TurnOff_State(OBJSTATE::Tick);
						// �ƴϸ� �� �ϳ��� ����
					}
				}// ���� ƽ�� ����
			}
		} // ��� �÷��� �Ǵ� ����

		ImGui::Separator();
		if (ImGui::BeginCombo("ShaderPass_Type", strTrailTypes[m_TrailBufferDesc.iShaderPassType]))
		{
			for (int n = 0; n < IM_ARRAYSIZE(strTrailTypes); n++)
			{
				_bool bSelected = (m_TrailBufferDesc.iShaderPassType == n);
				if (ImGui::Selectable(strTrailTypes[n], bSelected))
				{
					m_TrailBufferDesc.iShaderPassType = n;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("Discard_Alpha", &m_TrailBufferDesc.fDiscard_Alpha);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat2("DiffuseUV", &m_TrailBufferDesc.vDiffuseUV);
		m_pImgui_Manger->InputFloat2("MaskUV", &m_TrailBufferDesc.vMaskUV);
		m_pImgui_Manger->InputFloat2("NoiseUV", &m_TrailBufferDesc.vNoiseUV);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("DiffuseWeigh", &m_TrailBufferDesc.vWeight.x);
		m_pImgui_Manger->InputFloat("MaskWeight", &m_TrailBufferDesc.vWeight.y);
		m_pImgui_Manger->InputFloat("NoiseWeight", &m_TrailBufferDesc.vWeight.z);

		ImGui::Separator();
		ImGui::Checkbox("MaskAlphaInverse", &m_TrailBufferDesc.bMaskInverse);

		ImGui::Separator();
		ImGui::Checkbox("m_bDiffuseUse", &m_TrailBufferDesc.bDiffuseUse);

		ImGui::Separator();
		ImGui::Checkbox("isDiffuseClamp", &m_TrailBufferDesc.bDiffuseClamp);
		ImGui::Checkbox("isMaskClamp", &m_TrailBufferDesc.bMaskClamp);
		ImGui::Checkbox("isNoiseClamp", &m_TrailBufferDesc.bNoiseClamp);

		if (m_TrailBufferDesc.iShaderPassType == 2)
		{
			/*ImGui::Separator();
			m_pImgui_Manger->InputFloat3("DistortionValue", &m_TrailBufferDesc.vDistortionValue);
			m_pImgui_Manger->InputFloat("DistortinScale", &m_TrailBufferDesc.fDistortionScale);
			m_pImgui_Manger->InputFloat("DistortionBias", &m_TrailBufferDesc.fDistortionBias);
			m_pImgui_Manger->InputFloat("DisNoiseWeight", &m_TrailBufferDesc.fNoiseWeight);*/
		}

		ImGui::Separator();
		ImGui::Checkbox("Gizmo", &m_bGizmo);
		if (m_bGizmo)
		{
			Use_ImGuizmo(m_pTrailBufferEffect, true);
		}

		if (m_pGameInstance->Key_Down(DIK_Y))
		{
			if (m_bGizmo)
				m_bGizmo = false;
			else
				m_bGizmo = true;
		}

		ImGui::SameLine();
		if (m_pModel)
		{
			ImGui::Checkbox("ModelGizmo", &m_bModelGizmo);
			if (m_bModelGizmo)
			{
				Use_ImGuizmo(m_pModel, true);
			}
		}

		if (m_pGameInstance->Key_Down(DIK_U))
		{
			if (m_bModelGizmo)
				m_bModelGizmo = false;
			else
				m_bModelGizmo = true;
		}
		
		ImGui::Separator();
		HelpMarker(u8"����� ���� �÷� ���� ������ ��������.");
		ImGuiColorEditFlags flags;

		flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar |
			ImGuiColorEditFlags_NoInputs;

		ImGui::ColorEdit4("Max_InitColor", (float*)&m_TrailBufferDesc.vSolid_Color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);

		_uint iColorBlendType = m_TrailBufferDesc.eColorBlendType;
		ComboBox("ColorBlendType", CImgui_Tab_EffectTabBase::g_strColorBlendType, COLORBLENDTYPENUM,
			&iColorBlendType);
		m_TrailBufferDesc.eColorBlendType = (CEffect::COLORBLEND)iColorBlendType;

		m_pTrailBufferEffect->Set_TrailBuffer_Desc(m_TrailBufferDesc);
		m_pBuffer->Set_Buffer_Desc(m_BufferDesc);
	}

	return S_OK;
}


CImgui_Tab_TrailBufferEdit* CImgui_Tab_TrailBufferEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_TrailBufferEdit* pInstance = new CImgui_Tab_TrailBufferEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_TrailBufferEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_TrailBufferEdit::Free()
{
	__super::Free();
}

void CImgui_Tab_TrailBufferEdit::Set_Effect(shared_ptr<CTrail_Buffer> pEffect)
{
	if (m_pTrailBufferEffect != nullptr)
	{
		m_pTrailBufferEffect->TurnOn_State(OBJSTATE::WillRemoved);
		m_pTrailBufferEffect = nullptr;

		m_pBuffer = nullptr;
	}

	m_pTrailBufferEffect = pEffect;
	m_pBuffer = dynamic_pointer_cast<CVIBuffer_Trail>(m_pTrailBufferEffect->Get_VIBufferCom().lock());

	m_pBuffer = dynamic_pointer_cast<CVIBuffer_Trail>(m_pTrailBufferEffect->Get_VIBufferCom().lock());

	m_TrailBufferDesc = (m_pTrailBufferEffect->Get_TrailBuffer_Desc());

	m_BufferDesc = m_pBuffer->Get_Buffer_Desc();

	m_iMaxCntTemp = m_BufferDesc.iMaxCnt;
	m_iLerPointNum = m_BufferDesc.iLerpPointNum;

	m_bModify = true;
}

void CImgui_Tab_TrailBufferEdit::Remove_Effect()
{
	m_pTrailBufferEffect = nullptr;
	m_pBuffer = nullptr;

	m_bModify = false;
}

#endif // DEBUG
