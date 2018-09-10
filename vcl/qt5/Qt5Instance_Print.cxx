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

#include <Qt5Instance.hxx>
// #include <Qt5Printer.hxx>

#include <vcl/svapp.hxx>
#include <vcl/timer.hxx>
#include <printerinfomanager.hxx>

#include <jobset.h>
#include <print.h>
#include <salptype.hxx>
#include <saldatabasic.hxx>

#ifndef _WIN32
#include <unx/genpspgraphics.h>
#endif

using namespace psp;

/*
 *  static helpers
 */

#ifndef _WIN32
static OUString getPdfDir(const PrinterInfo& rInfo)
{
    OUString aDir;
    sal_Int32 nIndex = 0;
    while (nIndex != -1)
    {
        OUString aToken(rInfo.m_aFeatures.getToken(0, ',', nIndex));
        if (aToken.startsWith("pdf="))
        {
            sal_Int32 nPos = 0;
            aDir = aToken.getToken(1, '=', nPos);
            if (aDir.isEmpty())
                aDir = OStringToOUString(OString(getenv("HOME")), osl_getThreadTextEncoding());
            break;
        }
    }
    return aDir;
}
#endif

SalInfoPrinter* Qt5Instance::CreateInfoPrinter(SalPrinterQueueInfo* /* pQueueInfo */,
                                               ImplJobSetup* /* pJobSetup */)
{
#ifndef _WIN32
    // create and initialize SalInfoPrinter
    PspSalInfoPrinter* pPrinter = new PspSalInfoPrinter;
    configurePspInfoPrinter(pPrinter, pQueueInfo, pJobSetup);

    return pPrinter;
#else
    return nullptr;
#endif
}

void Qt5Instance::DestroyInfoPrinter(SalInfoPrinter* pPrinter) { delete pPrinter; }

std::unique_ptr<SalPrinter> Qt5Instance::CreatePrinter(SalInfoPrinter* /* pInfoPrinter */)
{
#ifndef _WIN32
    // create and initialize SalPrinter
    Qt5Printer* pPrinter = new Qt5Printer(pInfoPrinter);
    pPrinter->m_aJobData = static_cast<PspSalInfoPrinter*>(pInfoPrinter)->m_aJobData;

    return std::unique_ptr<SalPrinter>(pPrinter);
#else
    return std::unique_ptr<SalPrinter>();
#endif
}

void Qt5Instance::GetPrinterQueueInfo(ImplPrnQueueList* /* pList */)
{
#ifndef _WIN32
    PrinterInfoManager& rManager(PrinterInfoManager::get());
    static const char* pNoSyncDetection = getenv("SAL_DISABLE_SYNCHRONOUS_PRINTER_DETECTION");
    if (!pNoSyncDetection || !*pNoSyncDetection)
    {
        // #i62663# synchronize possible asynchronouse printer detection now
        rManager.checkPrintersChanged(true);
    }
    ::std::vector<OUString> aPrinters;
    rManager.listPrinters(aPrinters);

    for (::std::vector<OUString>::iterator it = aPrinters.begin(); it != aPrinters.end(); ++it)
    {
        const PrinterInfo& rInfo(rManager.getPrinterInfo(*it));
        // create new entry
        SalPrinterQueueInfo* pInfo = new SalPrinterQueueInfo;
        pInfo->maPrinterName = *it;
        pInfo->maDriver = rInfo.m_aDriverName;
        pInfo->maLocation = rInfo.m_aLocation;
        pInfo->maComment = rInfo.m_aComment;

        sal_Int32 nIndex = 0;
        while (nIndex != -1)
        {
            OUString aToken(rInfo.m_aFeatures.getToken(0, ',', nIndex));
            if (aToken.startsWith("pdf="))
            {
                pInfo->maLocation = getPdfDir(rInfo);
                break;
            }
        }

        pList->Add(pInfo);
    }
#endif
}

void Qt5Instance::DeletePrinterQueueInfo(SalPrinterQueueInfo* pInfo) { delete pInfo; }

void Qt5Instance::GetPrinterQueueState(SalPrinterQueueInfo*) {}

OUString Qt5Instance::GetDefaultPrinter()
{
#ifndef _WIN32
    PrinterInfoManager& rManager(PrinterInfoManager::get());
    return rManager.getDefaultPrinter();
#else
    return OUString();
#endif
}

void Qt5Instance::PostPrintersChanged() {}

#ifndef _WIN32
GenPspGraphics* Qt5Instance::CreatePrintGraphics() { return new GenPspGraphics(); }
#else
GenPspGraphics* Qt5Instance::CreatePrintGraphics() { return nullptr; }
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
