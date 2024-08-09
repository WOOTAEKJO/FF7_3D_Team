#pragma once
#include "Base.h"
#include "Path_Mgr.h"

BEGIN(Engine)

class ENGINE_DLL CUtility_File final : public CBase
{
public:
	enum DATA_TYPE {DATA,MODEL,TEXTURE,SHADER,TYPE_END};

public:
	static wstring PathFinder(const wstring& wstrMainPath, const wstring& wstrFileName,_bool bMultiple = false,
		_bool bSavePath = false);
	static wstring Get_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName);
	static void		Add_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName, const wstring& wstrFilePath);
	static void		Path_Mgr_Destroy();
	// 확장자 뺀 값 얻는 함수
	static wstring ConvertToFileName(const wstring& wstrFileName);

	static HRESULT	  All_FilePath_Save(const wstring& wstrMainPath);

	static string Get_FileName(string strFilePath); // 파일 경로에서 파일이름만 리턴

public:
	static _bool		Is_Number(const string& strFileName);
	static string		Remove_Number(const string& strFileName);

public:
	static map<wstring, wstring>	CRef_DataPathes() { return CPath_Mgr::GetInstance()->CRef_DataPathes(); }

};

END