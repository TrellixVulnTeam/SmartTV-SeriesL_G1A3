// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_DATETIMEPICKER_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_DATETIMEPICKER_H_

#include "xfa/fwl/basewidget/ifwl_datetimepicker.h"
#include "xfa/fwl/lightwidget/cfwl_widget.h"

class CFWL_DateTimePicker : public CFWL_Widget {
 public:
  static CFWL_DateTimePicker* Create();
  FWL_ERR Initialize(const CFWL_WidgetProperties* pProperties = NULL);
  FWL_ERR SetToday(int32_t iYear, int32_t iMonth, int32_t iDay);
  FWL_ERR GetEditText(CFX_WideString& wsText);
  FWL_ERR SetEditText(const CFX_WideStringC& wsText);
  int32_t CountSelRanges();
  int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  FWL_ERR GetCurSel(int32_t& iYear, int32_t& iMonth, int32_t& iDay);
  FWL_ERR SetCurSel(int32_t iYear, int32_t iMonth, int32_t iDay);
  FX_BOOL CanUndo();
  FX_BOOL CanRedo();
  FX_BOOL Undo();
  FX_BOOL Redo();
  FX_BOOL CanCopy();
  FX_BOOL CanCut();
  FX_BOOL CanSelectAll();
  FX_BOOL Copy(CFX_WideString& wsCopy);
  FX_BOOL Cut(CFX_WideString& wsCut);
  FX_BOOL Paste(const CFX_WideString& wsPaste);
  FX_BOOL SelectAll();
  FX_BOOL Delete();
  FX_BOOL DeSelect();
  FWL_ERR GetBBox(CFX_RectF& rect);
  FWL_ERR SetEditLimit(int32_t nLimit);
  FWL_ERR ModifyEditStylesEx(uint32_t dwStylesExAdded,
                             uint32_t dwStylesExRemoved);

 protected:
  CFWL_DateTimePicker();
  virtual ~CFWL_DateTimePicker();
  class CFWL_DateTimePickerDP : public IFWL_DateTimePickerDP {
   public:
    CFWL_DateTimePickerDP();
    virtual FWL_ERR GetCaption(IFWL_Widget* pWidget, CFX_WideString& wsCaption);
    virtual FWL_ERR GetToday(IFWL_Widget* pWidget,
                             int32_t& iYear,
                             int32_t& iMonth,
                             int32_t& iDay);
    int32_t m_iYear;
    int32_t m_iMonth;
    int32_t m_iDay;
    CFX_WideString m_wsData;
  };
  CFWL_DateTimePickerDP m_DateTimePickerDP;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_DATETIMEPICKER_H_
