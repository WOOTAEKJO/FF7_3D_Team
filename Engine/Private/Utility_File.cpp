#include "Utility_File.h"
#include <filesystem>

wstring CUtility_File::PathFinder(const wstring& wstrMainPath, const wstring& wstrFileName, _bool bMultiple, _bool bSavePath)
{
	if (!filesystem::exists(wstrMainPath))
		return wstring();

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(wstrMainPath))
	{
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;
		_bool	bNumCheck = false;
		CPath_Mgr::FILE_TYPE eType = CPath_Mgr::FILE_TYPE::FILE_TYPE_END;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if ((!strcmp(strTmp.c_str(), "")) || (!strcmp(strTmp.c_str(), ".afbx")) || (!strcmp(strTmp.c_str(), ".fbx"))
			|| (!strcmp(strTmp.c_str(), ".ini")) || (!strcmp(strTmp.c_str(), ".pskx")) || (!strcmp(strTmp.c_str(), ".txt")))
		{
			continue;
		}
		else if ((!strcmp(strTmp.c_str(), ".amodel")) || (!strcmp(strTmp.c_str(), ".groupmodel")))
		{
			eType = CPath_Mgr::FILE_TYPE::MODEL_FILE;
		}
		else if (!strcmp(strTmp.c_str(), ".aanim"))
		{
			eType = CPath_Mgr::FILE_TYPE::ANIM_FILE;
		}
		else if ((!strcmp(strTmp.c_str(), ".dds")) || (!strcmp(strTmp.c_str(), ".png")) || (!strcmp(strTmp.c_str(), ".hdr")))
		{
			eType = CPath_Mgr::FILE_TYPE::TEXTURE_FILE;
		}
		else if (!strcmp(strTmp.c_str(), ".json"))
		{
			eType = CPath_Mgr::FILE_TYPE::DATA_FILE;
		}
		else if ((!strcmp(strTmp.c_str(), ".hlsli")) || (!strcmp(strTmp.c_str(), ".hlsl")))
		{
			eType = CPath_Mgr::FILE_TYPE::SHADER_FILE;
		}
		else if ((!strcmp(strTmp.c_str(), ".mp3")) || (!strcmp(strTmp.c_str(), ".wav")) ||
			(!strcmp(strTmp.c_str(), ".ogg")))
		{
			eType = CPath_Mgr::FILE_TYPE::SOUND_FILE;
		} // 파일 확장자에 따른 분류
		else {
			string strName = szFileName;
			cout << strName + " 파일의 확장자가 등록되지 않았습니다. " + strTmp << endl;
			continue;
		}

		strTmp = szFileName;

		string strTmpFileName;
		auto vecWstring = SplitWstr(wstrFileName, L'.');
		if (vecWstring.empty())
			return wstring();
		else
			strTmpFileName = WstrToStr(vecWstring.front());
		// 텍스쳐가 여러개이면
		if (bMultiple)
		{
			// 파일 이름 뒤에 숫자가 있으면
			if (Is_Number(strTmp))
			{
				// 숫자를 제거한 파일이름
				strTmp = Remove_Number(strTmp);
				strTmpFileName = Remove_Number(strTmpFileName);
				bNumCheck = true;
			}
		}

		if (!strcmp(strTmpFileName.c_str(), strTmp.c_str()))
		{
			_char szFile[MAX_PATH] = "";
			string strResultFileName = (strTmp + (bNumCheck == true ? "%d" : ""));

			strcpy_s(szFile, szDdrive);
			strcat_s(szFile, szDirectory);
			strcat_s(szFile, strResultFileName.c_str());
			strcat_s(szFile, szExc);
			// 텍스쳐를 여러개 저장하는 경우 파일이름 뒤에 %d를 붙여서 주소를 리턴시킨다.

			wstring strResultPath = StrToWstr(szFile);

			if (bSavePath)
			{
				Add_FilePath(eType, wstrFileName, strResultPath);

			}
				
			return strResultPath;
		}
	}

    return TEXT("Failed");
}

wstring CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName)
{
	return CPath_Mgr::GetInstance()->Get_FilePath(eType,wstrFileName);
}

void CUtility_File::Add_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName, const wstring& wstrFilePath)
{
	CPath_Mgr::GetInstance()->Add_FilePath(eType,wstrFileName, wstrFilePath);
}

void CUtility_File::Path_Mgr_Destroy()
{
	CPath_Mgr::GetInstance()->DestroyInstance();
}

wstring CUtility_File::ConvertToFileName(const wstring& wstrFileName)
{
	_tchar	szDdrive[MAX_PATH] = L"";
	_tchar	szDirectory[MAX_PATH] = L"";
	_tchar	szFileName[MAX_PATH] = L"";
	_tchar	szExc[MAX_PATH] = L"";

	_wsplitpath_s(wstrFileName.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
		szFileName, MAX_PATH, szExc, MAX_PATH);

	wstring strResult = szFileName;

	return strResult;
}

HRESULT CUtility_File::All_FilePath_Save(const wstring& wstrMainPath)
{
	if (!filesystem::exists(wstrMainPath))
		RETURN_EFAIL;

	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(wstrMainPath))
	{
		string strFIlePath = WstrToStr(entry.path());
		string strTmp;
		_bool	bNumCheck = false;
		CPath_Mgr::FILE_TYPE eType = CPath_Mgr::FILE_TYPE::FILE_TYPE_END;

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		strTmp = szExc;

		if ((!strcmp(strTmp.c_str(), ".amodel")) || (!strcmp(strTmp.c_str(), ".groupmodel")))
		{
			eType = CPath_Mgr::FILE_TYPE::MODEL_FILE;
		}
		else if (!strcmp(strTmp.c_str(), ".aanim"))
		{
			eType = CPath_Mgr::FILE_TYPE::ANIM_FILE;
		}
		else if ((!strcmp(strTmp.c_str(), ".dds")) || (!strcmp(strTmp.c_str(), ".png")))
		{
			eType = CPath_Mgr::FILE_TYPE::TEXTURE_FILE;
		}
		else if (!strcmp(strTmp.c_str(), ".json"))
		{
			eType = CPath_Mgr::FILE_TYPE::DATA_FILE;

		}
		else if ((!strcmp(strTmp.c_str(), ".hlsli")) || (!strcmp(strTmp.c_str(), ".hlsl")))
		{
			eType = CPath_Mgr::FILE_TYPE::SHADER_FILE;
		}
		else if ((!strcmp(strTmp.c_str(), ".mp3")) || (!strcmp(strTmp.c_str(), ".wav")) ||
			(!strcmp(strTmp.c_str(), ".ogg")))
		{
			eType = CPath_Mgr::FILE_TYPE::SOUND_FILE;
		}
		else
			continue;
		// 파일 확장자에 따른 분류

		strTmp = szFileName;

		Add_FilePath(eType, StrToWstr(strTmp), StrToWstr(strFIlePath));
	}

	return S_OK;
}

string CUtility_File::Get_FileName(string strFilePath)
{
	vector<string> vecPath = SplitStr(strFilePath, L'/');
	vector<string> strTemp;
	string strFileName;

	for (auto& iter : vecPath)
	{
		strTemp = SplitStr(iter, L'\\');
	} // 한 번 더 짤라준다.

	strFileName = SplitStr(strTemp[strTemp.size()-1], L'.').front();

	return strFileName;
}

_bool CUtility_File::Is_Number(const string& strFileName)
{
	_uint iSize = Cast<_uint>(strFileName.size());

	if (strFileName[iSize - 1] > 47 && strFileName[iSize - 1] < 58)
		return true;

	return false;
}

string CUtility_File::Remove_Number(const string& strFileName)
{
	string strTmp = "";
	_uint iSize = Cast<_uint>(strFileName.size());

	if (strFileName[iSize - 1] > 47 && strFileName[iSize - 1] < 58)
	{
		for (_uint i = 0; i < iSize - 1; i++)
			strTmp.push_back(strFileName[i]);

		strTmp = Remove_Number(strTmp);
	}
	else {
		strTmp = strFileName;
	}

	return strTmp;
}
