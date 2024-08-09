#pragma once
#include "Base.h"
#include "GameInstance.h"

BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CPath_Mgr final : public CBase
{
	DECLARE_SINGLETON(CPath_Mgr)

public:
	enum FILE_TYPE {MODEL_FILE,ANIM_FILE,TEXTURE_FILE,DATA_FILE,SOUND_FILE,SHADER_FILE,FILE_TYPE_END};

private:
	CPath_Mgr();
	virtual	~CPath_Mgr() = default;

public:
	HRESULT	Initialize();
	void	Add_FilePath(FILE_TYPE eType, const wstring& strFileName,const wstring& strFilePath);
	wstring	Get_FilePath(FILE_TYPE eType, const wstring& strFileName);

public:
	template<class T>
	HRESULT	Create_Prototype_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,const wstring& strFileName);

private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	const map<wstring, wstring>& CRef_DataPathes() const { return m_mapDataPath; }

	void	Test_MAP_SIZE();
	
private:
	map<wstring, wstring> m_mapModelPath;
	map<wstring, wstring> m_mapAnimPath;
	map<wstring, wstring> m_mapTexturePath;
	map<wstring, wstring> m_mapDataPath;
	map<wstring, wstring> m_mapSoundPath;

private:
	wstring	Find_FilePath(FILE_TYPE eType, const wstring& strFileName);

private:
	virtual void	Free() override;
};

END

template<class T>
HRESULT	Create_Prototype_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,const wstring& strFileName)
{
	if (FAILED(m_pGameInstance->Add_Prototype(strFileName,
		T::Create(pDevice, pContext, filePathName))))
		RETURN_EFAIL;
}