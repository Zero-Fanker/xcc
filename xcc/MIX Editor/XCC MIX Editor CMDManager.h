#pragma once
#include "resource.h"
#include "XCC MIX EditorDlg.h"
class CXCCMIXEditorDlg;

class XCCMIXEditorCMDManager
{
public:

	static XCCMIXEditorCMDManager Instance;

	struct Action
	{
		bool Extract{ false };
		bool Packup{ false };
		bool Encrypt{ false };
		bool Compact{ false };
	};

	enum Strategy : unsigned
	{
		Recursive,
		Single,
		Multiple,
		Folder
	};

	struct FileInfo
	{

	};


	void DoAction(CXCCMIXEditorDlg* pDlg);

	void ParseArgs() 
	{
		//MessageBoxA(NULL, __FUNCTION__, __FILE__, MB_OK);
		for (int idx = 0; idx < __argc; idx++) {
			this->ParseArg(__argv[idx], idx);
		}

		this->ParseFiles();
	}

	bool IsHandled() {
		return this->ActionHandled;
	}

	bool SetAsHandled(bool handled) {
		return (this->ActionHandled = handled);
	}

	XCCMIXEditorCMDManager() = default;

private:
	void ParseArg(const char* arg, int& idx);
	void ParseFiles();

	Action ActionType{};
	Strategy ImportStrategy{};
	std::string ImportFiles{};
	std::string ExportFile{};
	std::vector<std::string> ImportFileNames{};
	bool ActionHandled{ false };
};