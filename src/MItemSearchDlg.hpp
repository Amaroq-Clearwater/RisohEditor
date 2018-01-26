// MItemSearchDlg --- RisohEditor "Item Search" dialog
//////////////////////////////////////////////////////////////////////////////
// RisohEditor --- Win32API resource editor
// Copyright (C) 2017-2018 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////////

#ifndef MZC4_MITEMSEARCHDLG_HPP_
#define MZC4_MITEMSEARCHDLG_HPP_

#include "RisohEditor.hpp"
#include "ResToText.hpp"
#include <set>

struct ITEM_SEARCH;
class MItemSearchDlg;

//////////////////////////////////////////////////////////////////////////////

struct ITEM_SEARCH
{
    ResToText   res2text;
    BOOL        bIgnoreCases;
    BOOL        bDownward;
    BOOL        bInternalText;
    BOOL        bRunning;
    BOOL        bCancelled;
    MString     strText;
    BOOL        bFindFirst;
    BOOL        bValid;
    HTREEITEM   hCurrent;
    HTREEITEM   hFound;
    TV_ITEM     item;
    TCHAR       szText[80];
    ITEM_SEARCH(const RisohSettings& settings, const ConstantsDB& db,
                const ResEntries& entries) : res2text(settings, db, entries)
    {
        bIgnoreCases = TRUE;
        bDownward = TRUE;
        bInternalText = FALSE;
        bRunning = FALSE;
        bCancelled = FALSE;
    }
};

class MItemSearchDlg : public MDialogBase
{
public:
    HICON m_hIcon;
    HICON m_hIconSm;
    ITEM_SEARCH& m_search;

    MItemSearchDlg(ITEM_SEARCH& search) 
        : MDialogBase(IDD_ITEMSEARCH), m_search(search)
    {
        m_hIcon = LoadIconDx(4);
        m_hIconSm = LoadSmallIconDx(4);
    }

    virtual ~MItemSearchDlg()
    {
        DestroyIcon(m_hIcon);
        DestroyIcon(m_hIconSm);
    }

    typedef std::set<MItemSearchDlg *> dialogs_type;

    static dialogs_type& Dialogs()
    {
        static dialogs_type s_dialogs;
        return s_dialogs;
    }

    virtual void PostNcDestroy()
    {
        Dialogs().erase(this);
        MDialogBase::PostNcDestroy();
        delete this;
    }

    void Done()
    {
        m_search.bRunning = FALSE;
        EnableWindow(GetDlgItem(m_hwnd, IDOK), TRUE);
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        Dialogs().insert(this);

        CheckRadioButton(hwnd, rad1, rad2, rad2);

        SendMessageDx(WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
        SendMessageDx(WM_SETICON, ICON_SMALL, (LPARAM)m_hIconSm);

        CenterWindowDx();
        return TRUE;
    }

    void OnOK(HWND hwnd)
    {
        if (m_search.bRunning)
            return;
        m_search.strText = GetDlgItemText(edt1);
        m_search.bIgnoreCases = IsDlgButtonChecked(hwnd, chx1) == BST_UNCHECKED;
        m_search.bDownward = IsDlgButtonChecked(hwnd, rad2) == BST_CHECKED;
        m_search.bInternalText = IsDlgButtonChecked(hwnd, chx2) == BST_CHECKED;
        m_search.bRunning = TRUE;
        EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
        SendMessage(GetParent(hwnd), WM_COMMAND, CMDID_ITEMSEARCHBANG, (WPARAM)this);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
            OnOK(hwnd);
            break;
        case IDCANCEL:
            if (!m_search.bRunning)
            {
                PostMessageDx(WM_COMMAND, 999);
            }
            else if (!m_search.bCancelled)
            {
                m_search.bRunning = FALSE;
                m_search.bCancelled = TRUE;
            }
            break;
        case 999:
            DestroyWindow(hwnd);
            break;
        }
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        }
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef MZC4_MITEMSEARCHDLG_HPP_