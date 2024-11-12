#include "stdafx.h"
#include "XCC MIX EditorDlg.h"

#include "blowfish.h"
#include "cc_file.h"
#include "file32.h"
#include "fname.h"
#include "id_log.h"
#include "mix_decode.h"
#include "MIXOptions.h"
#include "string_conversion.h"
#include "xcc_dirs.h"
#include "xcc_lmd_file_write.h"

constexpr const char* database_file_name = "local mix database.dat";

CXCCMIXEditorDlg::CXCCMIXEditorDlg(CWnd* pParent /*=NULL*/):
	ETSLayoutDialog(CXCCMIXEditorDlg::IDD, pParent, "XCCMIXEditorDlg")
{
	//{{AFX_DATA_INIT(CXCCMIXEditorDlg)
	m_edit_status = _T("Author: Olaf van der Spek <OlafvdSpek@GMail.Com>");
	//}}AFX_DATA_INIT
	m_current_id = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_open = false;
	m_key_loaded = false;
	m_xcc_id_enable = true;
}

void CXCCMIXEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXCCMIXEditorDlg)
	DDX_Control(pDX, IDC_BUTTON_INSERT, m_button_insert);
	DDX_Control(pDX, IDC_BUTTON_OPTIONS, m_button_options);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_button_delete);
	DDX_Control(pDX, IDC_BUTTON_LOAD_KEY, m_button_load_key);
	DDX_Control(pDX, IDC_BUTTON_COMPACT, m_button_compact);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_button_save);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_button_close);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_button_open);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_button_new);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_edit_status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXCCMIXEditorDlg, ETSLayoutDialog)
	//{{AFX_MSG_MAP(CXCCMIXEditorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_COMPACT, OnButtonCompact)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_KEY, OnButtonLoadKey)
	ON_BN_CLICKED(IDC_BUTTON_XCC_HOME_PAGE, OnButtonXccHomePage)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_OPTIONS, OnButtonOptions)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, OnDeleteitemList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnButtonInsert)
	ON_MESSAGE(WM_CMD_HANDLE, OnCommandHandle)
	ON_WM_TIMER()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int c_colums = 7;
char* column_label[] = {"Name", "Type", "Description", "ID", "Offset", "Size", ""};
int column_alignment[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_LEFT};

BOOL CXCCMIXEditorDlg::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();
	CreateRoot(VERTICAL)
		<< (pane(HORIZONTAL, GREEDY)
			<< item(IDC_LIST, GREEDY)
			<< (pane(VERTICAL, ABSOLUTE_HORZ)
				<< item(IDC_BUTTON_NEW, NORESIZE)
				<< item(IDC_BUTTON_OPEN, NORESIZE)
				<< item(IDC_BUTTON_SAVE, NORESIZE)
				<< item(IDC_BUTTON_COMPACT, NORESIZE)
				<< item(IDC_BUTTON_CLOSE, NORESIZE)
				<< item(IDC_BUTTON_LOAD_KEY, NORESIZE)
				<< item(IDC_BUTTON_INSERT, NORESIZE)
				<< item(IDC_BUTTON_DELETE, NORESIZE)
				<< item(IDC_BUTTON_OPTIONS, NORESIZE)
				<< item(IDC_BUTTON_XCC_HOME_PAGE, NORESIZE)
				)
			)
		<< item(IDC_EDIT_STATUS, ABSOLUTE_VERT);
	UpdateLayout();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	for (int i = 0; i < c_colums; i++)
		m_list.InsertColumn(i, column_label[i], column_alignment[i]);
	if (read_key(xcc_dirs::get_main_mix(game_ts)))
		read_key(xcc_dirs::get_main_mix(game_ra2));
	m_list.auto_size();
	update_buttons();

	PostMessage(WM_CMD_HANDLE);
	//SetTimer(1, 1000, NULL);
	return true;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXCCMIXEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		ETSLayoutDialog::OnPaint();
	}
}

HCURSOR CXCCMIXEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

const char* mix_filter = "MIX files (*.mix)|*.mix|";

void CXCCMIXEditorDlg::create_file(string& fileName)
{
	m_game = game_ts;
	m_encrypted = false;
	m_checksum = false;
	m_fname = fileName;
	m_open = true;
	set_changed(false);
}

void CXCCMIXEditorDlg::OnButtonNew()
{
	CFileDialog dlg(false, "mix", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, mix_filter, this);
	if (IDOK == dlg.DoModal())
	{
		this->create_file(static_cast<string>(dlg.GetPathName()));
	}
}

void CXCCMIXEditorDlg::OnButtonOpen()
{
	CFileDialog dlg(true, "mix", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, mix_filter, this);
	if (IDOK == dlg.DoModal())
		set_status("open mix", read_mix(static_cast<string>(dlg.GetPathName())));
}

void CXCCMIXEditorDlg::OnButtonSave()
{
	set_status("save mix", save_mix());
}

void CXCCMIXEditorDlg::OnButtonCompact()
{
	set_status("compact mix", compact_mix());
}

void CXCCMIXEditorDlg::OnButtonClose()
{
	m_open = false;
	m_list.DeleteAllItems();
	m_index.clear();
	update_buttons();
}

void CXCCMIXEditorDlg::OnButtonLoadKey()
{
	CFileDialog dlg(true, "mix", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, mix_filter, this);
	if (IDOK == dlg.DoModal())
		set_status("read key", read_key(static_cast<string>(dlg.GetPathName())));
}

void CXCCMIXEditorDlg::OnButtonDelete()
{
	int index = m_list.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	while (index != -1)
	{
		int id = m_list.GetItemData(index);
		m_list.DeleteItem(index);
		m_index.erase(id);
		index = m_list.GetNextItem(index - 1, LVNI_ALL | LVNI_SELECTED);
	}
	set_changed(true);
}

void CXCCMIXEditorDlg::OnButtonOptions()
{
	CMIXOptions dlg;
	dlg.set(m_game, m_encrypted, m_checksum);
	if (dlg.DoModal() == IDOK)
	{
		dlg.get(m_game, m_encrypted, m_checksum);
		set_changed(true);
	}
}

void CXCCMIXEditorDlg::set_game_option(t_game game, bool encrypted)
{
	m_game = game;
	m_encrypted = encrypted;
	//m_checksum = checksum;
}

void CXCCMIXEditorDlg::set_xcc_id_enable(bool enable)
{
	m_xcc_id_enable = enable;
}

void CXCCMIXEditorDlg::OnButtonXccHomePage()
{
	ShellExecute(m_hWnd, "open", "http://xccu.sourceforge.net/", NULL, NULL, SW_SHOW);
}

void CXCCMIXEditorDlg::update_buttons()
{
	m_button_new.EnableWindow(m_key_loaded && !m_open);
	m_button_open.EnableWindow(!m_open);
	m_button_save.EnableWindow(m_open && m_changed);
	m_button_compact.EnableWindow(m_open);
	m_button_close.EnableWindow(m_open);
	m_button_insert.EnableWindow(m_open);
	m_button_delete.EnableWindow(false);
	m_button_options.EnableWindow(m_open);
}

int CXCCMIXEditorDlg::read_key(const string& name)
{
	int error = 0;
	Cmix_file f;
	if (f.open(name))
		error = 1;
	else
	{
		if (f.is_encrypted())
		{
			f.seek(4);
			f.read(m_key, cb_mix_key_source);
			m_key_loaded = true;
			update_buttons();
		}
		else
			error = 1;
		f.close();

	}
	return error;
}

void CXCCMIXEditorDlg::add_file(const string& name)
{
	if (!m_open) {
		return;
	}
	const string fname = static_cast<Cfname>(name).get_fname();

	Ccc_file inputFile(false);
	if (inputFile.open(name)) {
		return;
	}
	auto const new_file_size = static_cast<unsigned>(inputFile.get_size());
	auto const id = Cmix_file::get_id(m_game, fname);

	{
		auto const it = m_index.find(id);
		// found, and can still be fit in
		if (it != m_index.end()) {
			if (new_file_size <= it->second.size) {
				it->second.fname = name;
				it->second.size = new_file_size;
				it->second.is_override = true;
				return;// we don't need to reconstruct
			} else {
				// sorry you have to get lost, see you later
				m_index.erase(id);
			}
		}
	}

	auto const result = m_index.insert(
		{ id, t_index_entry {
				 inputFile.get_file_type(),
					0,
					new_file_size,
					name,
			}
		});

	ASSERT(result.second);
	inputFile.close();

	set_changed(true);
	add_entry(id);

	//char buffer[256];
	//sprintf_s(buffer, __FUNCTION__" id : %d", id);
	//::MessageBoxA(NULL, buffer, __FILE__, MB_OK);
}

void CXCCMIXEditorDlg::add_entry(int id)
{
	auto& i = find_ref(m_index, id);
	int j = m_list.GetItemCount();
	m_list.InsertItem(j, static_cast<Cfname>(i.fname).get_fname().c_str());
	m_list.SetItemData(j, id);
	m_list.SetItemText(j, 1, ft_name[i.ft]);
	m_list.SetItemText(j, 2, mix_database::get_description(m_game, id).c_str());
	m_list.SetItemText(j, 3, nh(8, id).c_str());
	m_list.SetItemText(j, 4, n(i.offset).c_str());
	m_list.SetItemText(j, 5, n(i.size).c_str());
}

int CXCCMIXEditorDlg::read_mix(const string& name)
{
	Cmix_file f;
	m_fname = name;
	if (f.open(name)) {
		return 1;
	}
	if (f.is_encrypted()) {
		f.seek(4);
		f.read(m_key, cb_mix_key_source);
		m_key_loaded = true;
	}
	m_encrypted = f.is_encrypted();
	m_game = f.get_game();
	m_checksum = f.has_checksum();

	for (int i = 0; i < f.get_c_files(); i++) {
		auto const id = f.get_id(i);
		t_index_entry entry;
		entry.ft = f.get_type(id);
		entry.fname = mix_database::get_name(m_game, id);
		entry.offset = f.get_offset(id);
		entry.size = f.get_size(id);
		m_index[id] = entry;
	}

	f.close();
	m_open = true;
	set_changed(false);
	update_list();
	return 0;
}

static int copy_block(Cfile32& src, int src_pos, Cfile32& dst, int dst_pos, int size)
{
	int error = 0;
	int buffer_size = 4 << 20;
	byte* buffer = new byte[buffer_size];
	while (!error && size)
	{
		int cb_buffer = min(buffer_size, size);
		src.seek(src_pos);
		if (src.read(buffer, cb_buffer))
			error = 1;
		else
		{
			dst.seek(dst_pos);
			if (dst.write(buffer, cb_buffer))
				error = 1;
			else
			{
				src_pos += cb_buffer;
				dst_pos += cb_buffer;
				size -= cb_buffer;
			}
		}
	}
	delete[] buffer;
	return error;
}

static int get_header_size(t_game game, int c_files, bool checksum, bool encrypted)
{
	const int cb_checksum = checksum ? cb_mix_checksum : 0;
	const int cb_index = c_files * sizeof(t_mix_index_entry);
	if (game == game_td)
		return sizeof(t_mix_header) + cb_index;
	else if (encrypted)
		return 4 + cb_mix_key_source + (sizeof(t_mix_header) + cb_index + 7 & ~7) + cb_checksum;
	else
		return 4 + sizeof(t_mix_header) + cb_index + cb_checksum;
}

int CXCCMIXEditorDlg::get_header_size(bool including_db_file) const
{
	return ::get_header_size(m_game, m_index.size() + static_cast<int>(including_db_file == true), m_checksum, m_encrypted);
}

int CXCCMIXEditorDlg::get_max_offset(bool including_db_file) const
{
	unsigned r = get_header_size(including_db_file);
	for (auto& i : m_index)
		r = max(r, i.second.offset + i.second.size);
	return r;
}

// this function always save newly inserted file to the end of file, but not clean up old same file name content
int CXCCMIXEditorDlg::save_mix()
{
	// TODO: save to a temp file

	Cfile32 mixFile;
	if (mixFile.open(xcc_dirs::find_file(m_fname), GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS, 0)) {
		return 1;
	} 
	int error = 0;
	m_checksum = false;
	if (m_game == game_td) {
		m_encrypted = false;
	}

	const unsigned lmd_id = Cmix_file::get_id(m_game, database_file_name);
	// always remove it first
	m_index.erase(lmd_id);

	const unsigned header_size = get_header_size(m_xcc_id_enable);
	unsigned max_offset = header_size;
	// try to copy the first file block to last
	for (;;) {
		unsigned id;
		unsigned min_offset = INT_MAX;
		for (auto& idx : m_index) {
			// this one is not in mix file yet
			if (!idx.second.offset) {
				continue;
			}
			// get the minimum
			if (min_offset > idx.second.offset) {
				id = idx.first;
				min_offset = idx.second.offset;
			}
			// get the maximum offset
			if (max_offset < idx.second.offset + idx.second.size) {
				max_offset = idx.second.offset + idx.second.size;
			}
		}
		// means not found one
		if (min_offset >= header_size) {
			break;
		}
		// now it is the first item
		auto& first_item = find_ref(m_index, id);
		// copy the first one to last
		error = copy_block(mixFile, min_offset, mixFile, max_offset, first_item.size);
		if (error) {
			return error;
		}
		first_item.offset = max_offset;
	}
	// insert new files
	for (auto& indexed_block : m_index) {
		auto& block_entry = indexed_block.second;
		// already exists in mix file
		if (block_entry.offset && !block_entry.is_override) {
			continue;
		}
		if (indexed_block.first == lmd_id) {
			// let's handle database file lastly
			continue;
		}
		Cfile32 indexed_src_file;
		if (indexed_src_file.open(block_entry.fname, GENERIC_READ)) {
			return 1;
		}
		if (block_entry.is_override) {
			ASSERT(indexed_src_file.size() <= block_entry.size);
			error = copy_block(indexed_src_file, 0, mixFile, block_entry.offset, indexed_src_file.size());
			if (error) {
				return error;
			}
			// we only do it one time;
			block_entry.is_override = false;
			continue;
		}
		error = copy_block(indexed_src_file, 0, mixFile, max_offset, indexed_src_file.size());
		if (error) {
			return error;
		}
		block_entry.offset = max_offset;
		max_offset += indexed_src_file.size();
		indexed_src_file.close();
	}

	// write database file
	if (m_xcc_id_enable) {
		Cxcc_lmd_file_write db_file_writer;
		for (auto& i : m_index) {
			if (!i.second.fname.empty()) {
				db_file_writer.add_fname(static_cast<Cfname>(i.second.fname).get_fname());
			}
		}

		Cvirtual_binary lmd_data = db_file_writer.write(m_game);
		auto const result = m_index.emplace(lmd_id, t_index_entry{
			ft_xcc_lmd,
			0,
			lmd_data.size(),
			database_file_name,
			});
		ASSERT(result.second);
		// end of the file, but align with 0xf (16 in decimal)
		max_offset = (max_offset - header_size + 0xf & ~0xf) + header_size;
		mixFile.seek(max_offset);
		mixFile.write(lmd_data.data(), lmd_data.size());
		result.first->second.offset = max_offset;
		max_offset += lmd_data.size();
	}

	// go to the end
	mixFile.seek(max_offset);
	if (mixFile.set_eof() != 0) {
		return 1;
	}

	byte* header_data = new byte[header_size];
	t_mix_header* header;
	if (m_game == game_td) {
		header = reinterpret_cast<t_mix_header*>(header_data);
	}
	else {
		*reinterpret_cast<int*>(header_data) = (m_checksum ? mix_checksum : 0) | (m_encrypted ? mix_encrypted : 0);
		if (m_encrypted) {
			memcpy(header_data + 4, m_key, cb_mix_key_source);
			header = reinterpret_cast<t_mix_header*>(header_data + 4 + cb_mix_key_source);
		}
		else {
			header = reinterpret_cast<t_mix_header*>(header_data + 4);
		}
	}
	header->c_files = m_index.size();
	header->size = max_offset - header_size;
	t_mix_index_entry* index = reinterpret_cast<t_mix_index_entry*>(reinterpret_cast<byte*>(header) + sizeof(t_mix_header));
	for (auto const& indexed_block : m_index) {
		index->id = indexed_block.first;
		index->offset = indexed_block.second.offset - header_size;
		index->size = indexed_block.second.size;
		index++;
	}
	if (m_encrypted) {
		Cblowfish bf;
		std::array<byte, cb_mix_key> key;
		get_blowfish_key(m_key, key);
		bf.set_key(key);
		bf.encipher(header, header, header_size - (4 + cb_mix_key_source));
	}
	mixFile.seek(0);
	if (mixFile.write(header_data, header_size) && !error) {
		error = 1;
	}
	delete[] header_data;
	mixFile.close();
	set_changed(false);
	update_list();
	return error;
}

int CXCCMIXEditorDlg::compact_mix()
{
	int error = 0;
	error = save_mix();
	if (error) {
		return error;
	}
	Cfile32 f;
	if (f.open(xcc_dirs::find_file(m_fname), GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS, 0)) {
		return 1;
	}

	bool should_save = false;

	// remove database file first, since it will be reconstructed
	auto const erase_ret = m_index.erase(Cmix_file::get_id(m_game, database_file_name));
	ASSERT(erase_ret == 1);

	// this dead loop checks indexes multiple rounds,
	// each round it checks whether there are gaps between blocks, and try to move items to fill gaps
	// if there is not enough gap space, the block will be copied to the end of file first (make gaps), 
	// and then goes to next round.
	unsigned round = 0;
	for (;; ++round) {
		bool changed = false;
		unsigned debug_last_offset = 0;
		// use file offsets as key, and hash id (file name representative) as value
		map<unsigned, unsigned> offset_ordered_crc_table;

		unsigned max_offset = get_max_offset(m_xcc_id_enable);

		for (auto& idx : m_index) {
			auto const result = offset_ordered_crc_table.emplace(idx.second.offset, idx.first);
			ASSERT(result.second);
		}
		const unsigned payload_offset = get_header_size(m_xcc_id_enable);
		unsigned cur_offset = payload_offset;

		for (auto it = offset_ordered_crc_table.begin();
			it != offset_ordered_crc_table.end();
			++it) {
			auto& ordered_block = find_ref(m_index, it->second);
			ASSERT(ordered_block.size > 0);
			if (m_game == game_ts) {
				cur_offset = (cur_offset - payload_offset + 0xf & ~0xf) + payload_offset;
			}

			ASSERT(debug_last_offset != cur_offset);
			debug_last_offset = cur_offset;

			unsigned current_gap = 0;

			bool copy_to_cur_offset = false;
			bool copy_to_end = false;
			// this block is not close enough to current offset
			// so copy it here
			if (ordered_block.offset > cur_offset) {
				current_gap = ordered_block.offset - cur_offset;
				// safe check, we have enough gap:
				if (current_gap >= ordered_block.size) {
					copy_to_cur_offset = true;
				} else {// we don't have enough gap, other wise next block item will be overriden
					copy_to_end = true;
				}
			} else if (ordered_block.offset < cur_offset) {
				// this block is before current offset
				// copy it to the end of file
				copy_to_end = true;
			} else {

#if defined(DEBUG)
				auto next_it = [&it, &offset_ordered_crc_table]() {
					auto it_copy = it;
					if (it != offset_ordered_crc_table.end()) {
						++it_copy;
					}
					return it_copy;
				};
				auto const next = next_it();
				if (next != offset_ordered_crc_table.end()) {
					auto const& next_block = find_ref(m_index, next->second);
					ASSERT(next_block.offset != cur_offset);
					if (next_block.offset > cur_offset) {
						current_gap = next_block.offset - cur_offset;
					}
				} else {
					// I am the last one
					current_gap = ordered_block.size;
				}
#endif
				// if same, no need to move or copy
				ASSERT(ordered_block.size <= current_gap);
				cur_offset += ordered_block.size;
				continue;
			}

			ASSERT((copy_to_cur_offset + copy_to_end) <= 1);
			if (copy_to_cur_offset) {
				// this block copy handles the same file
				error = copy_block(f, ordered_block.offset, f, cur_offset, ordered_block.size);
				if (error) {
					return error;
				}
				ordered_block.offset = cur_offset;
				cur_offset += ordered_block.size;
				changed = true;
			}
			
			if (copy_to_end) {
				error = copy_block(f, ordered_block.offset, f, max_offset, ordered_block.size);
				if (error) {
					break;
				}
				//// now my old offset is available for data override
				//cur_offset = ordered_block.offset;
				// mine new offset is at the end
				ordered_block.offset = max_offset;
				max_offset += ordered_block.size;
				changed = true;
				continue;
			}
		}
		//if any block have ever changed
		should_save |= changed;
		if (!changed || error) {
			break;
		}
	}
	f.close();

	if (error) {
		return error;
	}

	if (should_save) {
		error = save_mix();
	}
	else {
		this->MessageBoxA("nothing to compact", "Compact Error", MB_OK);
	}
	return error;
}

void CXCCMIXEditorDlg::set_changed(bool changed)
{
	m_changed = changed;
	update_buttons();
}

void CXCCMIXEditorDlg::update_list()
{
	m_list.SetRedraw(false);
	m_list.DeleteAllItems();
	for (auto& i : m_index)
		add_entry(i.first);
	m_list.auto_size();
	sort_list(0, false);
	m_list.SetRedraw(true);
}

void CXCCMIXEditorDlg::set_status(const string& msg, bool error)
{
	m_edit_status = (msg + (error ? ": failed" : ": done")).c_str();
	UpdateData(false);
}

void CXCCMIXEditorDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_current_id = pNMListView->uNewState & LVIS_FOCUSED ? m_list.GetItemData(pNMListView->iItem) : 0;
	m_button_delete.EnableWindow(m_current_id);
	*pResult = 0;
}

void CXCCMIXEditorDlg::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_current_id = pNMListView->uNewState & LVIS_FOCUSED ? m_list.GetItemData(pNMListView->iItem) : 0;
	m_button_delete.EnableWindow(m_current_id);
	*pResult = 0;
}

void CXCCMIXEditorDlg::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int column = reinterpret_cast<NM_LISTVIEW*>(pNMHDR)->iSubItem;
	sort_list(column, column == m_sort_column ? !m_sort_reverse : false);
	*pResult = 0;
}

int compare_int(unsigned int a, unsigned int b)
{
	return a < b ? -1 : a != b;
}

int compare_string(string a, string b)
{
	return a < b ? -1 : a != b;
}

int CXCCMIXEditorDlg::compare(int id_a, int id_b) const
{
	if (m_sort_reverse)
		swap(id_a, id_b);
	const t_index_entry& a = find_ref(m_index, id_a);
	const t_index_entry& b = find_ref(m_index, id_b);
	switch (m_sort_column)
	{
	case 0:
		return compare_string(a.fname, b.fname);
	case 1:
		return compare_int(a.ft, b.ft);
	case 3:
		return compare_int(id_a, id_b);
	case 4:
		return compare_int(a.offset, b.offset);
	case 5:
		return compare_int(a.size, b.size);
	}
	return 0;
}

static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return reinterpret_cast<CXCCMIXEditorDlg*>(lParamSort)->compare(lParam1, lParam2);
}

void CXCCMIXEditorDlg::sort_list(int i, bool reverse)
{
	m_sort_column = i;
	m_sort_reverse = reverse;
	m_list.SortItems(Compare, reinterpret_cast<dword>(this));
}

const char* all_filter = "All files (*.*)|*.*|";

void CXCCMIXEditorDlg::OnButtonInsert()
{
	CFileDialog dlg(true, NULL, 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, all_filter, this);
	dlg.m_ofn.lpstrInitialDir = xcc_dirs::get_dir(game_td).c_str();
	if (IDOK == dlg.DoModal())
		add_file(static_cast<string>(dlg.GetPathName()));
}

void CXCCMIXEditorDlg::OnDropFiles(HDROP hDropInfo)
{
	int c_files = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	char fname[MAX_PATH];
	for (int i = 0; i < c_files; i++)
	{
		DragQueryFile(hDropInfo, i, fname, MAX_PATH);
		add_file(fname);
	}
	DragFinish(hDropInfo);
}

LRESULT CXCCMIXEditorDlg::OnCommandHandle(WPARAM wParam, LPARAM lParam)
{
	//command line handling
	auto& manager = XCCMIXEditorCMDManager::Instance;
	if (!manager.IsHandled()) {
		manager.SetAsHandled(true);
		manager.ParseArgs();
		manager.DoAction(this);
	}

	return S_OK;
}

void CXCCMIXEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case 1://90ms
		{
			//auto& manager = XCCMIXEditorCMDManager::Instance;
			//if (!manager.IsHandled()) {
			//	manager.SetAsHandled(true);
			//	manager.ParseArgs();
			//	manager.DoAction(this);
			//}
			break;
		}
		default:
			break;
	}

	CDialog::OnTimer(nIDEvent);

}