#include "Path_Mgr.h"
#include <filesystem>

IMPLEMENT_SINGLETON(CPath_Mgr)

CPath_Mgr::CPath_Mgr()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPath_Mgr::Initialize()
{
    return S_OK;
}

void CPath_Mgr::Add_FilePath(FILE_TYPE eType, const wstring& strFileName,const wstring& strFilePath)
{
    wstring strTemp = Find_FilePath(eType,strFileName);
    if (Compare_Wstr(strTemp, L"Not_Find"))
    {
        switch (eType)
        {
        case Engine::CPath_Mgr::MODEL_FILE:
            m_mapModelPath.emplace(strFileName, strFilePath);
            break;
        case Engine::CPath_Mgr::ANIM_FILE:
            m_mapAnimPath.emplace(strFileName, strFilePath);
            break;
        case Engine::CPath_Mgr::TEXTURE_FILE:
            m_mapTexturePath.emplace(strFileName, strFilePath);
            break;
        case Engine::CPath_Mgr::DATA_FILE:
            m_mapDataPath.emplace(strFileName, strFilePath);
            break;
        case Engine::CPath_Mgr::SOUND_FILE:
            m_mapSoundPath.emplace(strFileName, strFilePath);
            break;
        case Engine::CPath_Mgr::SHADER_FILE:
            break;
        case Engine::CPath_Mgr::FILE_TYPE_END:

            cout << "파일 주소 저장 실패" << endl;

            break;
        }
    }
}

wstring CPath_Mgr::Get_FilePath(FILE_TYPE eType, const wstring& strFileName)
{
    wstring strTemp = Find_FilePath(eType,strFileName);
    if (Compare_Wstr(strTemp, L"Not_Find"))
    {
#ifdef _DEBUG
        cout << WstrToStr(strFileName) + "를 찾지못했습니다." << endl;
#endif
    }

    return strTemp;
}

void CPath_Mgr::Test_MAP_SIZE()
{
	_uint_64 iSize = 0;
	_uint_64 iKeySize = 0;
	for (auto iter = m_mapTexturePath.begin(); iter != m_mapTexturePath.end(); ++iter)
	{
		iKeySize += ((*iter).first.size() + 1) * 2;
		iSize += ((*iter).second.size() + 1) * 2;
	}
    
	for (auto iter = m_mapAnimPath.begin(); iter != m_mapAnimPath.end(); ++iter)
	{
		iKeySize += ((*iter).first.size() + 1) * 2;
		iSize += ((*iter).second.size() + 1) * 2;
	}

	for (auto iter = m_mapDataPath.begin(); iter != m_mapDataPath.end(); ++iter)
	{
		iKeySize += ((*iter).first.size() + 1) * 2;
		iSize += ((*iter).second.size() + 1) * 2;
	}

	for (auto iter = m_mapModelPath.begin(); iter != m_mapModelPath.end(); ++iter)
	{
		iKeySize += ((*iter).first.size() + 1) * 2;
		iSize += ((*iter).second.size() + 1) * 2;
	}

	for (auto iter = m_mapSoundPath.begin(); iter != m_mapSoundPath.end(); ++iter)
	{
		iKeySize += ((*iter).first.size() + 1) * 2;
		iSize += ((*iter).second.size() + 1) * 2;
	}
    
    cout << "키 사이즈 : " + to_string(iKeySize) << endl;
    cout << "문자열 사이즈 : " + to_string(iSize) << endl;
}

wstring CPath_Mgr::Find_FilePath(FILE_TYPE eType, const wstring& strFileName)
{
    switch (eType)
    {
    case Engine::CPath_Mgr::MODEL_FILE:
    {
        auto& iter = m_mapModelPath.find(strFileName);
        if (iter == m_mapModelPath.end())
            return L"Not_Find";

        return iter->second;
    }
    case Engine::CPath_Mgr::ANIM_FILE:
    {
        auto& iter = m_mapAnimPath.find(strFileName);
        if (iter == m_mapAnimPath.end())
            return L"Not_Find";

        return iter->second;
    }
    case Engine::CPath_Mgr::TEXTURE_FILE:
    {
        auto& iter = m_mapTexturePath.find(strFileName);
        if (iter == m_mapTexturePath.end())
            return L"Not_Find";

        return iter->second;
    }
    case Engine::CPath_Mgr::DATA_FILE:
    {
        auto& iter = m_mapDataPath.find(strFileName);
        if (iter == m_mapDataPath.end())
            return L"Not_Find";

        return iter->second;
    } 
    case Engine::CPath_Mgr::SOUND_FILE:
    {
        auto& iter = m_mapSoundPath.find(strFileName);
        if (iter == m_mapSoundPath.end())
            return L"Not_Find";

        return iter->second;
    }
    }

    return L"Not_Find";
}

void CPath_Mgr::Free()
{
    m_mapModelPath.clear();
    m_mapAnimPath.clear();
    m_mapTexturePath.clear();
    m_mapDataPath.clear();
    m_mapSoundPath.clear();

    Safe_Release(m_pGameInstance);
}
