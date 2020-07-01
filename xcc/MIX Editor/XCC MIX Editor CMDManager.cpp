#include "stdafx.h"
#include "XCC MIX EditorDlg.h"
#include "XCC MIX Editor.h"
#include "MIXOptions.h"
#include "XCC MIX Editor CMDManager.h"
#include <boost/algorithm/string.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/filesystem.hpp>

//namespace fs = boost::filesystem;

XCCMIXEditorCMDManager XCCMIXEditorCMDManager::Instance;

const char* getNextArg(int& idx)
{
	if (idx >= __argc) {
		return "";
	}
	return __argv[++idx];
}

void XCCMIXEditorCMDManager::ParseArg(const char * arg, int& idx)
{

	if(!_strcmpi(arg, "-i"))
	{
		this->ImportFiles = getNextArg(idx);
	}
	else if (!_strcmpi(arg, "-o"))
	{
		this->ExportFile = getNextArg(idx);
	}
	else if (!_strcmpi(arg, "--import-strategy"))
	{
		auto const strategy = getNextArg(idx);
		if (!_strcmpi(strategy, "recursive"))
		{
			this->ImportStrategy = Strategy::Recursive;
		}
		else if (!_strcmpi(strategy, "multiple"))
		{
			this->ImportStrategy = Strategy::Multiple;
		}
	}
	else if (!_strcmpi(arg, "--action"))
	{
		auto const action = getNextArg(idx);
		if (!_strcmpi(action, "encrypt"))
		{
			this->ActionType.Encrypt = true;
		}
		else if (!_strcmpi(action, "compact"))
		{
			this->ActionType.Compact = true;
		}
		else if (!_strcmpi(action, "packup"))
		{
			this->ActionType.Packup = true;
		}
		else if (!_strcmpi(action, "extract"))
		{
			this->ActionType.Extract = true;
		}
	}

}

#if 0
void GatherAllFiles(std::string& name, std::vector<std::string>& fileNames)
{
	fs::path full_path(name, fs::native);

	if (fs::exists(full_path)) {
		for (auto const& item : full_path) {
			if (!fs::is_directory(full_path)) {
				fileNames.emplace_back(name);
			}
			else {
				string sub_dir_str(full_path.string());
				sub_dir_str.append(item.string());
				sub_dir_str.append("//");
				GatherAllFiles(sub_dir_str, fileNames);
			}
		}
	}
}
#endif

void XCCMIXEditorCMDManager::ParseFiles()
{
	if (this->ImportFiles.empty()) {
		return;
	}

	switch (this->ImportStrategy) {
		case Strategy::Recursive:
			{
				std::vector<std::string> names;
				boost::split(names, this->ImportFiles, boost::is_any_of(_T(",;")));

				for (auto const& name : names) {

					//check whether is directory:
					this->ImportFileNames.push_back(name);

				}

			}
			break;
		case Strategy::Single:
			break;
		case Strategy::Multiple:
			{
				boost::split(this->ImportFileNames, this->ImportFiles, boost::is_any_of(_T(",;")));
			}
			break;
		case Strategy::Folder:
			break;
		default:
			break;
	}
}


void XCCMIXEditorCMDManager::DoAction(CXCCMIXEditorDlg* pDlg)
{
	if (this->ExportFile.empty()) {
		return;
	}

	int error;

	if (!(error = pDlg->read_mix(this->ExportFile))) {
		pDlg->set_status("open mix", error);
	}
	else {
		pDlg->create_file(this->ExportFile);
	}

	if (this->ActionType.Extract) {
		::MessageBoxA(NULL, __FUNCTION__" processing extract ", __FILE__, MB_OK);
	}
	else {

		if (this->ActionType.Packup)
		{
			for (auto const& fileName : this->ImportFileNames) {
				pDlg->add_file(fileName);
			}
		}

		if (this->ActionType.Encrypt)
		{
			CMIXOptions dlg;
			dlg.set(game_ra2_yr, TRUE, FALSE);
			//pDlg->set_changed(true);
		}

		if (this->ActionType.Compact) {
			pDlg->compact_mix();
		}

		pDlg->save_mix();
	}

}