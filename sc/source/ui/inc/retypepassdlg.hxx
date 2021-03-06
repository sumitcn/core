/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_SC_SOURCE_UI_INC_RETYPEPASSDLG_HXX
#define INCLUDED_SC_SOURCE_UI_INC_RETYPEPASSDLG_HXX

#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/edit.hxx>
#include <vcl/fixed.hxx>
#include <vcl/layout.hxx>
#include <vcl/scrbar.hxx>
#include <vcl/weld.hxx>
#include <svx/checklbx.hxx>

#include <tabprotection.hxx>

#include <memory>

namespace vcl { class Window; }
class ScDocProtection;
class ScTableProtection;
class ScDocument;

class ScRetypePassDlg : public ModalDialog
{
public:
    typedef std::shared_ptr<ScDocProtection>    DocProtectionPtr;
    typedef std::shared_ptr<ScTableProtection>  TabProtectionPtr;

    ScRetypePassDlg() = delete;
    explicit ScRetypePassDlg(vcl::Window* pParent);
    virtual ~ScRetypePassDlg() override;
    virtual void dispose() override;

    virtual short Execute() override;

    void SetDataFromDocument(const ScDocument& rDoc);
    void SetDesiredHash(ScPasswordHash eHash);

    /** Write the new set of password data to the document instance to
        overwrite the current ones. */
    void WriteNewDataToDocument(ScDocument& rDoc) const;

private:
    void Init();
    void PopulateDialog();
    void SetDocData();
    void SetTableData(size_t nRowPos, SCTAB nTab);

    /** Check the status of all hash values to see if it's okay to enable
        the OK button. */
    void CheckHashStatus();

    void DeleteSheets();

private:
    VclPtr<OKButton>       mpBtnOk;
    VclPtr<FixedText>      mpTextDocStatus;
    VclPtr<PushButton>     mpBtnRetypeDoc;
    VclPtr<VclVBox>        mpSheetsBox;

    std::vector<VclPtr<VclHBox>> maSheets;

    OUString        maTextNotProtected;
    OUString        maTextNotPassProtected;
    OUString        maTextHashBad;
    OUString        maTextHashGood;

    DECL_LINK( OKHdl, Button*, void );
    DECL_LINK( RetypeBtnHdl, Button*, void );

    struct TableItem
    {
        OUString       maName;
        TabProtectionPtr    mpProtect;
    };
    ::std::vector<TableItem> maTableItems;

    DocProtectionPtr    mpDocItem;
    ScPasswordHash      meDesiredHash;
};

class ScRetypePassInputDlg : public weld::GenericDialogController
{
public:
    ScRetypePassInputDlg() = delete;
    explicit ScRetypePassInputDlg(weld::Window* pParent, ScPassHashProtectable* pProtected);
    virtual ~ScRetypePassInputDlg() override;

    bool IsRemovePassword() const;
    OUString GetNewPassword() const;

private:
    void Init();
    void CheckPasswordInput();

private:
    ScPassHashProtectable* m_pProtected;

    std::unique_ptr<weld::Button> m_xBtnOk;

    std::unique_ptr<weld::RadioButton> m_xBtnRetypePassword;

    std::unique_ptr<weld::Widget> m_xPasswordGrid;
    std::unique_ptr<weld::Entry> m_xPassword1Edit;
    std::unique_ptr<weld::Entry> m_xPassword2Edit;

    std::unique_ptr<weld::CheckButton> m_xBtnMatchOldPass;

    std::unique_ptr<weld::RadioButton> m_xBtnRemovePassword;

    DECL_LINK( OKHdl, weld::Button&, void );
    DECL_LINK( RadioBtnHdl, weld::ToggleButton&, void );
    DECL_LINK( CheckBoxHdl, weld::ToggleButton&, void );
    DECL_LINK( PasswordModifyHdl, weld::Entry&, void );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
