#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_ParticleEdit.h"
#include "ImGuiFileDialog.h"

#include "Data_Manager.h"

#include "Utility_File.h"
#include "Texture_Data.h"

#include "Imgui_Tab_EffectEdit.h"

#include "ModelContainer.h"

#include "Effect_Manager.h"

const char* strParticleTypes[] = { "BILLBOARD","NONBILLBOARD","DIRECTION","FIRE",
									"FIRE_DIR","DISTORTION","DISTORTION_DIR"};
const char* strParticleSpriteTypes[] = { "Diffuse","Mask" };
const char* strParticleSpriteTypesDistortion[] = { "Noise","Mask" };

CImgui_Tab_ParticleEdit::CImgui_Tab_ParticleEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Tab_EffectTabBase(pGameObjectList, pDevice, pContext)
{

}

HRESULT CImgui_Tab_ParticleEdit::Initialize()
{

	return S_OK;
}

void CImgui_Tab_ParticleEdit::Tick(_cref_time fTimeDelta)
{

}

HRESULT CImgui_Tab_ParticleEdit::Render()
{
	if (m_pParticle)
	{
		if (m_pParticle->IsState(OBJSTATE::WillRemoved))
			m_pParticle = nullptr;
		// 삭제 예정이면 비워두기
	}

	if (ImGui::Button("Save_Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveEffect", "Save Effect File", ".json", "../Bin/Data/Effect_Data/Particle/");
	}

	if (ImGuiFileDialog::Instance()->Display("SaveEffect"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_pParticle);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (!m_bModify)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load_Effect"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("LoadEffect", "Load Effect File", ".json", "../Bin/Data/Effect_Data/Particle/");
		}

		if (ImGuiFileDialog::Instance()->Display("LoadEffect"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string filePathName = m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

				if (m_pParticle != nullptr)
					m_pParticle->TurnOn_State(OBJSTATE::WillRemoved);

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(),
					L_EFFECT, StrToWstr(CUtility_File::Get_FileName(filePathName)), nullptr, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pParticle))))
					RETURN_EFAIL;

				m_pVIBufferCom = dynamic_pointer_cast<CVIBuffer_Instancing>(m_pParticle->Get_VIBufferCom().lock());
				m_InstancingDesc = *m_pVIBufferCom->Get_InstancingDesc();

				m_tParticle_Desc = m_pParticle->Get_ParticleDesc();

				if (m_pMask_Window)
				{
					Safe_Release(m_pMask_Window);
					m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
				}
				else {
					m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
				}

				for (_uint i = 0; i < m_tParticle_Desc.iMaskArrayNum; i++)
				{
					m_pMask_Window->Set_MaskArrayTag(i, m_tParticle_Desc.strMaskArray[i]);
				}

				m_iMoveControlType = m_InstancingDesc.eMoveControl;			//움직임
				m_iMeshStartPointType = m_InstancingDesc.eMeshStartType;	// 스타트 지점
				m_iDeathType = m_InstancingDesc.eDeathType;					// 죽음조건
				m_iUseTypeRadio = m_tParticle_Desc.eUseType;				// 오너타입	

				m_bLoad = true;
			}
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::SeparatorText("Texture_List");
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
							m_tParticle_Desc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum] = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_tParticle_Desc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						}
					}
					if (m_pGameInstance->Key_Down(DIK_UP) && m_iPickIndex_PrtTag[m_iTexTypeRadio] > 0 && m_iPickIndex_PrtTag[m_iTexTypeRadio] <= m_PrototypeList[m_iTexTypeRadio].size() - 1)
					{
						m_iPickIndex_PrtTag[m_iTexTypeRadio]--;

						switch (m_iTexTypeRadio)
						{
						case 0:
							m_tParticle_Desc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 1:
							m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum] = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
							break;
						case 2:
							m_tParticle_Desc.strNoise = m_PrototypeList[m_iTexTypeRadio][m_iPickIndex_PrtTag[m_iTexTypeRadio]];
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
							m_tParticle_Desc.strDiffuse = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 1:
							m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum] = m_PrototypeList[m_iTexTypeRadio][i];
							break;
						case 2:
							m_tParticle_Desc.strNoise = m_PrototypeList[m_iTexTypeRadio][i];
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
							m_tParticle_Desc.strDiffuse = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 1:
							m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum] = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						case 2:
							m_tParticle_Desc.strNoise = m_vecFilter[m_iPickIndex_Filter[m_iTexTypeRadio]];
							break;
						}
					}
				}

				ImGui::EndListBox();
			}
		}

		if (m_iTexTypeRadio == 1)
		{
			ImGui::SeparatorText("Select_Texture");
			if ((m_pMask_Window == nullptr))
				m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
			else
			{
				if (ImGui::Button("Select"))
				{
					if (m_tParticle_Desc.iMaskArrayNum < 18)
					{
						m_pMask_Window->Set_MaskArrayTag(m_tParticle_Desc.iMaskArrayNum,
							m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum]);
						++m_tParticle_Desc.iMaskArrayNum;
						m_InstancingDesc.iMaskArrayMaxIndex = m_tParticle_Desc.iMaskArrayNum;
					}
				}
			}
		}

		ImGui::SeparatorText(" Image ");
		switch (m_iTexTypeRadio)
		{
		case 0:
			m_pImgui_Manger->Load_Image(m_tParticle_Desc.strDiffuse, _float2(200.f, 200.f));
			break;
		case 1:
			m_pImgui_Manger->Load_Image(m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum],
				_float2(200.f, 200.f));
			break;
		case 2:
			m_pImgui_Manger->Load_Image(m_tParticle_Desc.strNoise, _float2(200.f, 200.f));
			break;
		}

		if (m_pParticle == nullptr)
		{
			ImGui::Checkbox("MeshVTXParticle", &m_InstancingDesc.bMeshVTXParticle);
			HelpMarker(u8" 메쉬로 만드는 이펙트");
			if (m_InstancingDesc.bMeshVTXParticle)
			{
				if (ImGui::BeginListBox("Model", vSize))
				{
					for (int i = 0; i < m_PrototypeList[CImgui_Window_EffectEdit::ET_MODEL].size(); i++)
					{
						wstring Temp = m_PrototypeList[CImgui_Window_EffectEdit::ET_MODEL][i];
						vector<wstring> PrtTag = SplitWstr(Temp, L'/');
						wstring wstrTag = {};
						wstrTag = SplitWstr(PrtTag[PrtTag.size() - 1], L'.').front();
						wstrTag = L"[" + wstrTag + L"] ";

						string  str;
						str = WstrToStr(wstrTag);
						if (ImGui::Selectable(str.c_str(), m_iPickIndex_ModelTag == i))
						{
							m_iPickIndex_ModelTag = i;
						}
					}
					ImGui::EndListBox();

				}

				m_pImgui_Manger->InputFloat("MeshSize", &m_InstancingDesc.fMeshSize);
				ImGui::Separator();
				ImGui::RadioButton("PZero", &m_iMeshStartPointType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("PMesh", &m_iMeshStartPointType, 1);
				ImGui::SameLine();
				ImGui::RadioButton("PRandom", &m_iMeshStartPointType, 2);
				m_InstancingDesc.eMeshStartType = (CVIBuffer_Instancing::MESH_START_TYPE)m_iMeshStartPointType;
				m_pImgui_Manger->InputFloat3("MeshOffsetMin", &m_InstancingDesc.vMeshPointOffset[0]);
				m_pImgui_Manger->InputFloat3("MeshOffsetMax", &m_InstancingDesc.vMeshPointOffset[1]);

				if (ImGui::Button("MeshApply"))
				{
					FModelData* ModelData = m_pGameInstance->Get_ModelDatas()->find(m_PrototypeList[CImgui_Window_EffectEdit::ET_MODEL][m_iPickIndex_ModelTag])->second;
					m_InstancingDesc.vecMeshVTX = ModelData->Find_Mesh(0)->vecVertices;
				}

			}
		}
		
		ImGui::SeparatorText("Particle_Count");
		m_pImgui_Manger->Arrow_Button("NumInstance", 10, &m_InstancingDesc.iNumInstance);
		m_tParticle_Desc.iNumInstnace = m_InstancingDesc.iNumInstance;

		ImGui::Separator();
		if (ImGui::Button("Create_Effect"))
		{
			if (m_pParticle)
				return S_OK;

			if (m_InstancingDesc.bMeshVTXParticle)
			{
				if (m_InstancingDesc.vecMeshVTX.empty())
					return S_OK;
			}

			if (Compare_Wstr(m_tParticle_Desc.strDiffuse, L""))
				m_tParticle_Desc.strDiffuse = TEXT("0Effect_Diffuse_None");

			if (Compare_Wstr(m_tParticle_Desc.strNoise, L""))
				m_tParticle_Desc.strNoise = TEXT("0Effect_Noise_None");

			if (Compare_Wstr(m_tParticle_Desc.strMaskArray[0], L""))
			{
				m_tParticle_Desc.strMaskArray[0] = TEXT("0Effect_Mask_None");
				if (m_pMask_Window == nullptr)
					m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
				else
					m_pMask_Window->Set_MaskArrayTag(0, m_tParticle_Desc.strMaskArray[0]);
			}

			m_tParticle_Desc.Instancing_Desc = m_InstancingDesc;

			if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT,
				TEXT("Prototype_GameObject_Particle"), &m_tParticle_Desc, reinterpret_cast<shared_ptr<CGameObject>*>(&m_pParticle))))
				RETURN_EFAIL;
			m_pVIBufferCom = dynamic_pointer_cast<CVIBuffer_Instancing>(m_pParticle->Get_VIBufferCom().lock());

			m_bLoad = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove_Effect"))
		{
			if (m_pParticle != nullptr)
			{
				m_pParticle->TurnOn_State(OBJSTATE::WillRemoved);
				m_pParticle = nullptr;

			}

			if (m_pMask_Window)
			{
				Safe_Release(m_pMask_Window);
				m_pMask_Window = nullptr;
				m_tParticle_Desc.iMaskArrayNum = 0;
			}

			m_bLoad = false;
		}
	}

	ImGui::SeparatorText("Current_Texture_Text");
	if (m_tParticle_Desc.strDiffuse.size() > 0)
		ImGui::Text(("Diffuse : " + WstrToStr(SplitWstr(m_tParticle_Desc.strDiffuse, L'/').back())).c_str());
	if (m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum].size() > 0)
		ImGui::Text(("Mask : " + WstrToStr(SplitWstr(m_tParticle_Desc.strMaskArray[m_tParticle_Desc.iMaskArrayNum]
			, L'/').back())).c_str());
	if (m_tParticle_Desc.strNoise.size() > 0)
		ImGui::Text(("Noise : " + WstrToStr(SplitWstr(m_tParticle_Desc.strNoise, L'/').back())).c_str());

	ImGui::Separator();
	if (m_pParticle != nullptr)
	{
		ImGui::SeparatorText("Survival");
		if (m_pParticle->Get_IsEffectDead())
			ImGui::Text("Effect Dead");
		else
			ImGui::Text("Effect Alive");
		HelpMarker(u8"생존여부");

		ImGui::SeparatorText("ObjPoolNum");
		_int iObjPoolNum = m_pParticle->Get_ObjPool_MaxNum();
		ImGui::InputInt("PoolNum",&iObjPoolNum);
		m_pParticle->Set_ObjPool_MaxNum(iObjPoolNum);
		HelpMarker(u8"오브젝트 풀 클론 생성 갯수 설정");

		ImGui::Separator();
		if (!m_bModify && m_bLoad)
		{
			if (ImGui::Button("AddGroup"))
			{
				CEffect_Manager::GetInstance()->Get_EffectGroup()->Add_Effect(EFFECT_TYPE::EFFECT_PARTICLE, m_strLoadFilePath, m_pParticle);
				m_pParticle = nullptr;
				return S_OK;
			}// 저장된 파일만 그룹에 추가할 수 있다.
		}

		ImGui::SeparatorText("OnwerType");
		ImGui::RadioButton("UNone", &m_iUseTypeRadio, 0);
		HelpMarker(u8"오너X | 단발성 이펙트");
		ImGui::SameLine();
		ImGui::RadioButton("UNor", &m_iUseTypeRadio, 1);
		HelpMarker(u8"일반적인 객체 | 트랜스폼의 월드행렬을 사용");

		ImGui::RadioButton("UPart", &m_iUseTypeRadio, 2);
		HelpMarker(u8"파츠오브젝트 | 파츠오브젝트의 월드행렬을 사용");
		ImGui::SameLine();
		ImGui::RadioButton("UEffect", &m_iUseTypeRadio, 3);
		HelpMarker(u8"이펙트 객체 | 블랜드 오브젝트의 월드행렬 사용");
		m_tParticle_Desc.eUseType = (CEffect::USE_TYPE)m_iUseTypeRadio;
		// 오너의 타입을 정한다.

		ImGui::SeparatorText("StartLine");
		if (ImGui::Button("Play"))
		{
			m_pParticle->Set_Play(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_pParticle->Set_Play(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			m_pParticle->Reset_Effect(true);
		}
		if (m_pGameInstance->Key_Down(DIK_N))
		{
			m_pParticle->Reset_Effect(true);
		}

		ImGui::Separator();
		ComboBox("ParticleType", strParticleTypes, 7, &m_tParticle_Desc.iShaderPassType);
		HelpMarker(u8"BillBoard : \nNone : BustMode 사용시 그 해당 방향으로 회전하면서 진행\nDir : 빌보드 + 자기방향회전, 카메라 가까이가면 조금 틀어짐\nFire : 디스토션 효과");

		ImGui::SeparatorText("Soft");
		ImGui::Checkbox("Soft", &m_tParticle_Desc.bParticleSoft);

		ImGui::SeparatorText("SolidColor");
		ImGui::Checkbox("SolidColor", &m_tParticle_Desc.bSolidColor);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat("Discard_Alpha", &m_tParticle_Desc.fDiscard_Alpha);

		ImGui::Separator();
		m_pImgui_Manger->InputFloat2("DiffuseUV", &m_tParticle_Desc.vDiffuseUV);
		m_pImgui_Manger->InputFloat2("MaskUV", &m_tParticle_Desc.vMaskUV);
		m_pImgui_Manger->InputFloat2("NoiseUV", &m_tParticle_Desc.vNoiseUV);

		if ((m_tParticle_Desc.iShaderPassType == 3) || (m_tParticle_Desc.iShaderPassType == 4))
		{
			ImGui::Separator();
			m_pImgui_Manger->InputFloat3("DistortionValue", &m_tParticle_Desc.vDistortionValue);
			HelpMarker(u8"1,2,3번째 마스크 텍스쳐라고 생각하면 된다.");
			m_pImgui_Manger->InputFloat("DistortinScale", &m_tParticle_Desc.fDistortionScale);
			m_pImgui_Manger->InputFloat("distortionBias", &m_tParticle_Desc.fDistortionBias);
			m_pImgui_Manger->InputFloat("NoiseWeight", &m_tParticle_Desc.fNoiseWeight);
			HelpMarker(u8"외곡 속도");
		}

		/* For. ETC*/

		ImGui::Separator();
		ImGui::Checkbox("Loop", &m_InstancingDesc.bLoop);
		HelpMarker(u8"반복재생");
		ImGui::Checkbox("Cube", &m_InstancingDesc.bCube);
		HelpMarker(u8"자기값을 큐브처럼 똑같이 유지, 길게 늘리고 싶으면 체크X");
		ImGui::Checkbox("BurstMode", &m_InstancingDesc.bBurst);
		HelpMarker(u8"NoneBillBoard모드에서만 사용 | Look방향으로 방향성을 줌");
		m_pImgui_Manger->Arrow_Button("SpawnTime", 0.1f, &m_InstancingDesc.fSpawnTime);
		HelpMarker(u8"0~X까지 그 사이에 랜덤하게 생성");
		m_pImgui_Manger->Arrow_Button("Min_LifeTime", 0.1f, &m_InstancingDesc.vLifeTime.x);
		m_pImgui_Manger->Arrow_Button("Max_LifeTime", 0.1f, &m_InstancingDesc.vLifeTime.y);
		HelpMarker(u8"Min~Max까지만 생존한다");

		if (m_InstancingDesc.vLifeTime.x > m_InstancingDesc.vLifeTime.y)
			m_InstancingDesc.vLifeTime.y = m_InstancingDesc.vLifeTime.x;
		m_pImgui_Manger->InputFloat3("Center", &m_InstancingDesc.vCenter);
		HelpMarker(u8"생성되는 위치 | 보통 안 만짐");

		ImGui::Separator();
		ImGui::Checkbox("Gizmo", &m_bGizmo);
		if (m_bGizmo)
		{
			Use_ImGuizmo(m_pParticle, true);
		}

		if (m_pGameInstance->Key_Down(DIK_Y))
		{
			if(m_bGizmo)
				m_bGizmo = false;
			else
				m_bGizmo = true;
		}

		ImGui::Separator();
		m_pImgui_Manger->InputFloat3("Min_StartRange", &m_InstancingDesc.vStartRange[0]);
		m_pImgui_Manger->InputFloat3("Max_StartRange", &m_InstancingDesc.vStartRange[1]);
		HelpMarker(u8"원점을 기준으로 X,Z만 설정하면 2D, Y까지추가하면 3D가 된다");
		m_InstancingDesc.vStartRange[1].x = Max(m_InstancingDesc.vStartRange[0].x, m_InstancingDesc.vStartRange[1].x);
		m_InstancingDesc.vStartRange[1].y = Max(m_InstancingDesc.vStartRange[0].y, m_InstancingDesc.vStartRange[1].y);
		m_InstancingDesc.vStartRange[1].z = Max(m_InstancingDesc.vStartRange[0].z, m_InstancingDesc.vStartRange[1].z);
		m_pImgui_Manger->InputFloat("CutRange", &m_InstancingDesc.fCutRange);
		HelpMarker(u8"위 StartRange 내에서 Range범위를 잘라낸다 (도넛모양)");
		m_pImgui_Manger->InputFloat3("Min_OffsetRange", &m_InstancingDesc.vOffsetRange[0]);
		m_pImgui_Manger->InputFloat3("Max_OffsetRange", &m_InstancingDesc.vOffsetRange[1]);
		HelpMarker(u8"DRange조건일떄만 동작한다\nMin~Max범위를 벗어나면 Dead처리");
		m_InstancingDesc.vOffsetRange[1].x = Max(m_InstancingDesc.vOffsetRange[0].x, m_InstancingDesc.vOffsetRange[1].x);
		m_InstancingDesc.vOffsetRange[1].y = Max(m_InstancingDesc.vOffsetRange[0].y, m_InstancingDesc.vOffsetRange[1].y);
		m_InstancingDesc.vOffsetRange[1].z = Max(m_InstancingDesc.vOffsetRange[0].z, m_InstancingDesc.vOffsetRange[1].z);

		ImGui::Separator();
		ImGui::RadioButton("PMove", &m_iControlType, 0);
		HelpMarker(u8"이동 옵션 설정");
		ImGui::SameLine();
		ImGui::RadioButton("PScale", &m_iControlType, 1);
		HelpMarker(u8"크기 옵션 설정");
		ImGui::RadioButton("PRotation", &m_iControlType, 2);
		HelpMarker(u8"빌보드 사용 시 적용이 안되서 잘 사용 안함");
		ImGui::SameLine();
		ImGui::RadioButton("PColor", &m_iControlType, 3);
		HelpMarker(u8"Min~Max Init Color 범위 내 랜덤, Min은 000,Max는 최대111까지 설정가능");

		/* For. Move*/
		if (m_iControlType == 0)
		{
			ImGui::Separator();
			ImGui::RadioButton("Nor", &m_iMoveControlType, 0);
			HelpMarker(u8"노말 : 수치대로 움직인다");
			ImGui::SameLine();
			ImGui::RadioButton("Center", &m_iMoveControlType, 1);
			HelpMarker(u8"정점으로 모인다");

			ImGui::RadioButton("Easing", &m_iMoveControlType, 2);
			HelpMarker(u8"이징함수 | 포스값 영향을 반으로 받으니 무조건 포스 줘야함");
			ImGui::SameLine();
			ImGui::RadioButton("Mesh", &m_iMoveControlType, 3);
			HelpMarker(u8"메쉬인덱스 순서대로 순회한다.");
			ImGui::SameLine();
			ImGui::RadioButton("ZeroDir", &m_iMoveControlType, 4);
			HelpMarker(u8"원점에서 위치로 향하는 방향");
			m_InstancingDesc.eMoveControl = (CVIBuffer_Instancing::MOVE_CONTROL_TYPE)m_iMoveControlType;

			if (m_InstancingDesc.eMoveControl == CVIBuffer_Instancing::MOVE_CONTROL_TYPE::EASING)
			{
				ComboBox("EasingType", CEasing::pEases, 34, &m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::MOVE]);
				m_pImgui_Manger->Load_EaseImage(m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::MOVE], _float2(150.f, 150.f));
			}

			ImGui::Separator();
			m_pImgui_Manger->Arrow_Button("Adjust_MoveSpeed", 0.1f, &m_InstancingDesc.fMoveAdjustSpeed);
			HelpMarker(u8"아래의 Min,Max값에 영향을 준다. | 전체에 곱해준다고 생각하면 된다.");
			m_pImgui_Manger->InputFloat3("Min_Move_InitSpeed", &m_InstancingDesc.vMove_Speed[0]);
			m_pImgui_Manger->InputFloat3("Max_Move_InitSpeed", &m_InstancingDesc.vMove_Speed[1]);
			HelpMarker(u8"생성되었을때의 스피드 Min,Max값");
			m_InstancingDesc.vMove_Speed[1].x = Max(m_InstancingDesc.vMove_Speed[0].x, m_InstancingDesc.vMove_Speed[1].x);
			m_InstancingDesc.vMove_Speed[1].y = Max(m_InstancingDesc.vMove_Speed[0].y, m_InstancingDesc.vMove_Speed[1].y);
			m_InstancingDesc.vMove_Speed[1].z = Max(m_InstancingDesc.vMove_Speed[0].z, m_InstancingDesc.vMove_Speed[1].z);
			m_pImgui_Manger->InputFloat3("Move_Force", &m_InstancingDesc.vMove_Force);
			HelpMarker(u8"순간적인 가속도 | 시간이 흘러갈수록 좀더 빨라지거나 느려지게 설정 가능");
			m_pImgui_Manger->InputFloat3("Move_Speed_Min", &m_InstancingDesc.vMove_MinSpeed);
			m_pImgui_Manger->InputFloat3("Move_Speed_Max", &m_InstancingDesc.vMove_MaxSpeed);
			HelpMarker(u8"스피드 Min,Max값");
			m_InstancingDesc.vMove_MaxSpeed.x = Max(m_InstancingDesc.vMove_MinSpeed.x, m_InstancingDesc.vMove_MaxSpeed.x);
			m_InstancingDesc.vMove_MaxSpeed.y = Max(m_InstancingDesc.vMove_MinSpeed.y, m_InstancingDesc.vMove_MaxSpeed.y);
			m_InstancingDesc.vMove_MaxSpeed.z = Max(m_InstancingDesc.vMove_MinSpeed.z, m_InstancingDesc.vMove_MaxSpeed.z);

			ImGui::Separator();
			ImGui::Checkbox("DirTurn", &m_InstancingDesc.bDirTurn);
			HelpMarker(u8"꺾이는걸 나타냄");
			if (m_InstancingDesc.bDirTurn)
			{
				m_pImgui_Manger->InputFloat("DirTurnLerp", &m_InstancingDesc.fDirTurnLerp);
				HelpMarker(u8"Lerp는 0에 가까울수록 거의X 1에 가까울수록 빠르게 회전");
				m_pImgui_Manger->InputFloat("DirTurnAngle", &m_InstancingDesc.fDirTurnAngle);
				m_pImgui_Manger->InputFloat("WaitTime", &m_InstancingDesc.fDirTurnWaitTime);
				HelpMarker(u8"대기시간이 지나면 회전");
				m_pImgui_Manger->InputFloat2("DirTurnTime", &m_InstancingDesc.vDirTurnTime);
				HelpMarker(u8"Min~Max까지 시간이 될때마다 회전 (듀레이션 개념)");
				m_InstancingDesc.vDirTurnTime.x = Min(m_InstancingDesc.vDirTurnTime.x, m_InstancingDesc.vDirTurnTime.y);
				m_InstancingDesc.vDirTurnTime.y = Max(m_InstancingDesc.vDirTurnTime.x, m_InstancingDesc.vDirTurnTime.y);
			}

			ImGui::Separator();
			ImGui::Checkbox("Force", &m_InstancingDesc.bForce);
			HelpMarker(u8"일종의 중력느낌, 힘의 방향을 벡터로 줄 수 있음\n기본적으로 중력처럼 0,-1,0처럼 사용, ForecValue에 Speed값이 곱해진다, Speed값이 클수록 영향이 크다 | 가속도 개념");
			if (m_InstancingDesc.bForce)
			{
				m_pImgui_Manger->InputFloat3("ForceDir", &m_InstancingDesc.vForceDir);
				m_pImgui_Manger->InputFloat("ForceValue", &m_InstancingDesc.fForce);
				m_pImgui_Manger->InputFloat("ForceSpeed", &m_InstancingDesc.fForceSpeed);
			}
		}
		else if (m_iControlType == 1) /* For. Scale*/
		{
			ImGui::Separator();
			ImGui::Checkbox("ScaleEasing", &m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::SCALE]);
			if (m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::SCALE])
			{
				ComboBox("EasingType", CEasing::pEases, 34, &m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::SCALE]);
				m_pImgui_Manger->Load_EaseImage(m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::SCALE], _float2(150.f, 150.f));
			}

			ImGui::Separator();
			m_pImgui_Manger->InputFloat3("Min_InitScale", &m_InstancingDesc.vScale[0]);
			m_pImgui_Manger->InputFloat3("Max_InitScale", &m_InstancingDesc.vScale[1]);
			m_InstancingDesc.vScale[1].x = Max(m_InstancingDesc.vScale[0].x, m_InstancingDesc.vScale[1].x);
			m_InstancingDesc.vScale[1].y = Max(m_InstancingDesc.vScale[0].y, m_InstancingDesc.vScale[1].y);
			m_InstancingDesc.vScale[1].z = Max(m_InstancingDesc.vScale[0].z, m_InstancingDesc.vScale[1].z);
			m_pImgui_Manger->InputFloat3("Scale_Speed", &m_InstancingDesc.vScale_Speed);
			m_pImgui_Manger->InputFloat3("Scale_Force", &m_InstancingDesc.vScale_Force);
			m_pImgui_Manger->InputFloat3("Scale_Min", &m_InstancingDesc.vScale_Min);
			m_pImgui_Manger->InputFloat3("Scale_Max", &m_InstancingDesc.vScale_Max);
			m_InstancingDesc.vScale_Max.x = Max(m_InstancingDesc.vScale_Min.x, m_InstancingDesc.vScale_Max.x);
			m_InstancingDesc.vScale_Max.y = Max(m_InstancingDesc.vScale_Min.y, m_InstancingDesc.vScale_Max.y);
			m_InstancingDesc.vScale_Max.z = Max(m_InstancingDesc.vScale_Min.z, m_InstancingDesc.vScale_Max.z);
		}
		else if (m_iControlType == 2) /* For. Rotation*/
		{
			ImGui::Separator();
			ImGui::Checkbox("RotationEasing", &m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::ROTATION]);
			if (m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::ROTATION])
			{
				ComboBox("EasingType", CEasing::pEases, 34, &m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::ROTATION]);
				m_pImgui_Manger->Load_EaseImage(m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::ROTATION], _float2(150.f, 150.f));
			}

			ImGui::Separator();
			m_pImgui_Manger->InputFloat3("Min_InitRotation", &m_InstancingDesc.vRotation[0]);
			m_pImgui_Manger->InputFloat3("Max_InitRotation", &m_InstancingDesc.vRotation[1]);
			m_InstancingDesc.vRotation[1].x = Max(m_InstancingDesc.vRotation[0].x, m_InstancingDesc.vRotation[1].x);
			m_InstancingDesc.vRotation[1].y = Max(m_InstancingDesc.vRotation[0].y, m_InstancingDesc.vRotation[1].y);
			m_InstancingDesc.vRotation[1].z = Max(m_InstancingDesc.vRotation[0].z, m_InstancingDesc.vRotation[1].z);
			m_pImgui_Manger->InputFloat3("Rotation_Speed", &m_InstancingDesc.vRotation_Speed);
			m_pImgui_Manger->InputFloat3("Rotation_Force", &m_InstancingDesc.vRotation_Force);
			m_pImgui_Manger->InputFloat3("Rotation_Min", &m_InstancingDesc.vRotation_Min);
			m_pImgui_Manger->InputFloat3("Rotation_Max", &m_InstancingDesc.vRotation_Max);
			m_InstancingDesc.vRotation_Max.x = Max(m_InstancingDesc.vRotation_Min.x, m_InstancingDesc.vRotation_Max.x);
			m_InstancingDesc.vRotation_Max.y = Max(m_InstancingDesc.vRotation_Min.y, m_InstancingDesc.vRotation_Max.y);
			m_InstancingDesc.vRotation_Max.z = Max(m_InstancingDesc.vRotation_Min.z, m_InstancingDesc.vRotation_Max.z);
		}
		else if (m_iControlType == 3) /* For. Color*/
		{
			ImGui::Separator();
			HelpMarker(u8"디스토션 사용시 컬러 알파 값으로 조절가능.");
			ImGui::Checkbox("ColorEasing", &m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::COLOR]);
			if (m_InstancingDesc.bEasing[CVIBuffer_Instancing::VALUE_TYPE::COLOR])
			{
				ComboBox("EasingType", CEasing::pEases, 34, &m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::COLOR]);
				m_pImgui_Manger->Load_EaseImage(m_InstancingDesc.iEasingType[CVIBuffer_Instancing::VALUE_TYPE::COLOR], _float2(150.f, 150.f));
			}


			ImGui::Separator();
			ImGui::ColorEdit4("Min_InitColor", (float*)&m_InstancingDesc.vColor[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine();
			ImGui::ColorEdit4("Max_InitColor", (float*)&m_InstancingDesc.vColor[1], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);
			
			m_InstancingDesc.vColor[1].x = Max(m_InstancingDesc.vColor[0].x, m_InstancingDesc.vColor[1].x);
			m_InstancingDesc.vColor[1].y = Max(m_InstancingDesc.vColor[0].y, m_InstancingDesc.vColor[1].y);
			m_InstancingDesc.vColor[1].z = Max(m_InstancingDesc.vColor[0].z, m_InstancingDesc.vColor[1].z);
			m_InstancingDesc.vColor[1].w = Max(m_InstancingDesc.vColor[0].w, m_InstancingDesc.vColor[1].w);
			m_pImgui_Manger->InputFloat4("Color_Speed", &m_InstancingDesc.vColor_Speed);
			m_pImgui_Manger->InputFloat4("Color_Force", &m_InstancingDesc.vColor_Force);
			m_pImgui_Manger->InputFloat4("Color_Max", &m_InstancingDesc.vColor_Max);
			m_InstancingDesc.vColor_Max.x = Max(0.f, m_InstancingDesc.vColor_Max.x);
			m_InstancingDesc.vColor_Max.y = Max(0.f, m_InstancingDesc.vColor_Max.y);
			m_InstancingDesc.vColor_Max.z = Max(0.f, m_InstancingDesc.vColor_Max.z);
			m_InstancingDesc.vColor_Max.w = Max(0.f, m_InstancingDesc.vColor_Max.w);

			_uint iColorBlendType = m_tParticle_Desc.eColorBlendType;
			ComboBox("ColorBlendType", CImgui_Tab_EffectTabBase::g_strColorBlendType, COLORBLENDTYPENUM,
				&iColorBlendType);
			m_tParticle_Desc.eColorBlendType = (CEffect::COLORBLEND)iColorBlendType;

		}

		ImGui::Separator();
		ImGui::Checkbox("Sprite", &m_tParticle_Desc.bSpriteAnim);
		HelpMarker(u8"스프라이트 애니메이션 사용 여부");
		m_InstancingDesc.bSprite = m_tParticle_Desc.bSpriteAnim;

		if (m_tParticle_Desc.bSpriteAnim)
		{

			ImGui::Separator();
			
			if ((m_tParticle_Desc.iShaderPassType == 5) || (m_tParticle_Desc.iShaderPassType == 6))
				ComboBox("SpriteType", strParticleSpriteTypesDistortion, 2, &m_tParticle_Desc.iSpriteType);
			else
				ComboBox("SpriteType", strParticleSpriteTypes, 2, &m_tParticle_Desc.iSpriteType);

			ImGui::Checkbox("SpriteLoop", &m_tParticle_Desc.bLoop);
			HelpMarker(u8"스프라이트 애니메이션을 계속 돌릴지 선택");
			m_InstancingDesc.bSprtieLoop = m_tParticle_Desc.bLoop;

			ImGui::Separator();
			m_pImgui_Manger->InputFloat2("ColRow", &m_tParticle_Desc.vSpriteColRow);
			HelpMarker(u8"스트라이트 자르는 가로 세로줄 설정");
			m_InstancingDesc.vSpriteColRow = m_tParticle_Desc.vSpriteColRow;

			ImGui::Separator();
			m_pImgui_Manger->InputFloat("Duration", &m_tParticle_Desc.fDuration);
			HelpMarker(u8"스프라이트 애니메이션이 도는 총 시간을 의미한다.");
			m_InstancingDesc.fSpriteDuration = m_tParticle_Desc.fDuration;
			m_pImgui_Manger->InputFloat2("Sprite_Speed", &m_InstancingDesc.vSprite_Speed);
			HelpMarker(u8"Min~Max까지의 시간 랜덤값");
		}

		/* For. DeathCondition*/
		ImGui::SeparatorText("Dead_Type");
		ImGui::RadioButton("DTime", &m_iDeathType, 0);
		HelpMarker(u8"시간이 지나면 Dead처리");
		ImGui::SameLine();
		ImGui::RadioButton("DScale", &m_iDeathType, 1);
		HelpMarker(u8"일정 크기에 도달시 Dead처리");
		ImGui::SameLine();
		ImGui::RadioButton("DColor", &m_iDeathType, 2);
		HelpMarker(u8"알파값이 0이되면 Dead처리");
	
		ImGui::RadioButton("DRange", &m_iDeathType, 3);
		HelpMarker(u8"설정한 범위를 벗어나면 Dead처리");
		ImGui::SameLine();
		ImGui::RadioButton("DOwner", &m_iDeathType, 4);
		HelpMarker(u8"오너가 Nullptr이면 Dead처리");
		m_InstancingDesc.eDeathType = (CVIBuffer_Instancing::DEATH_TYPE)m_iDeathType;
		m_tParticle_Desc.eDeadType = (CEffect::DEADTYPE)m_iDeathType;
		// 죽음 타입을 정한다.
		// 파티클도 인스턴싱도 같이 죽음 조건을 잡아줘야 한다.

		m_pVIBufferCom->Set_InstancingDesc(m_InstancingDesc);
		m_pParticle->Set_ParticleDesc(m_tParticle_Desc);
	}

	if (m_pMask_Window)
	{
		m_pMask_Window->Render();
	}

	return S_OK;
}

void CImgui_Tab_ParticleEdit::Set_Effect(shared_ptr<CParticle> pEffect)
{
	if (m_pParticle != nullptr)
	{
		m_pParticle->TurnOn_State(OBJSTATE::WillRemoved);
		m_pParticle = nullptr;

		m_pVIBufferCom = nullptr;
	}

	m_pParticle = pEffect;
	m_pVIBufferCom = dynamic_pointer_cast<CVIBuffer_Instancing>(m_pParticle->Get_VIBufferCom().lock());

	m_InstancingDesc = *m_pVIBufferCom->Get_InstancingDesc();

	m_tParticle_Desc = m_pParticle->Get_ParticleDesc();

	if (m_pMask_Window)
	{
		Safe_Release(m_pMask_Window);
		m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	}
	else {
		m_pMask_Window = CImgui_Window_Mask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	}

	for (_uint i = 0; i < m_tParticle_Desc.iMaskArrayNum; i++)
	{
		m_pMask_Window->Set_MaskArrayTag(i, m_tParticle_Desc.strMaskArray[i]);
	}

	m_iMoveControlType = m_InstancingDesc.eMoveControl;
	m_iMeshStartPointType = m_InstancingDesc.eMeshStartType;
	m_iDeathType = m_InstancingDesc.eDeathType;

	m_bModify = true;
}

void CImgui_Tab_ParticleEdit::Remove_Effect()
{
	m_pParticle = nullptr;

	if (m_pMask_Window)
	{
		Safe_Release(m_pMask_Window);
		m_pMask_Window = nullptr;
		m_tParticle_Desc.iMaskArrayNum = 0;
	}

	m_bModify = false;
}

CImgui_Tab_ParticleEdit* CImgui_Tab_ParticleEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_ParticleEdit* pInstance = new CImgui_Tab_ParticleEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_ParticleEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_ParticleEdit::Free()
{

	Safe_Release(m_pMask_Window);

	__super::Free();
}

#endif // DEBUG
