#pragma once
#include "resource.h"
#include "XCC MIX EditorDlg.h"
class CXCCMIXEditorDlg;

class XCCMIXEditorCMDManager
{
public:

	struct Action
	{
		bool Extract : 1;
		bool Packup : 1;
		bool Encrypt : 1;
		bool Compact : 1;
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



	XCCMIXEditorCMDManager()
	{
		for (int idx = 0; idx < __argc; idx++) {
			this->ParseArg(__argv[idx], idx);
		}

		this->ParseFiles();
	}

private:
	void ParseArg(const char* arg, int& idx);
	void ParseFiles();

	Action ActionType;
	Strategy ImportStrategy;
	std::string ImportFiles;
	std::string ExportFile;
	std::vector<std::string> ImportFileNames;

};