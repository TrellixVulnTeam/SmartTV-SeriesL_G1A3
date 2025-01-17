// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PDFWINDOW_PWL_EDITCTRL_H_
#define FPDFSDK_PDFWINDOW_PWL_EDITCTRL_H_

#include "core/fxcrt/include/fx_string.h"
#include "fpdfsdk/include/fxedit/fx_edit.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"

class CPWL_Caret;
class CPWL_Edit;
class CPWL_EditCtrl;
class IFX_Edit;
class IPWL_Edit_Notify;
struct CPVT_WordPlace;

enum PWL_EDIT_ALIGNFORMAT_H { PEAH_LEFT = 0, PEAH_MIDDLE, PEAH_RIGHT };

enum PWL_EDIT_ALIGNFORMAT_V { PEAV_TOP = 0, PEAV_CENTER, PEAV_BOTTOM };

class IPWL_Edit_Notify {
 public:
  virtual ~IPWL_Edit_Notify() {}
  // when the position of caret is changed in edit
  virtual void OnCaretMove(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {}
  virtual void OnContentChange(const CFX_FloatRect& rcContent) {}
  // OprType: 0 InsertWord
  // 1 InsertReturn
  // 2 BackSpace
  // 3 Delete
  // 4 Clear
  // 5 InsertText
  // 6 SetText
  virtual void OnInsertWord(const CPVT_WordPlace& place,
                            const CPVT_WordPlace& oldplace) {}
  virtual void OnInsertReturn(const CPVT_WordPlace& place,
                              const CPVT_WordPlace& oldplace) {}
  virtual void OnBackSpace(const CPVT_WordPlace& place,
                           const CPVT_WordPlace& oldplace) {}
  virtual void OnDelete(const CPVT_WordPlace& place,
                        const CPVT_WordPlace& oldplace) {}
  virtual void OnClear(const CPVT_WordPlace& place,
                       const CPVT_WordPlace& oldplace) {}
  virtual void OnInsertText(const CPVT_WordPlace& place,
                            const CPVT_WordPlace& oldplace) {}
  virtual void OnSetText(const CPVT_WordPlace& place,
                         const CPVT_WordPlace& oldplace) {}
  virtual void OnAddUndo(CPWL_Edit* pEdit) {}
};

class CPWL_EditCtrl : public CPWL_Wnd, public IFX_Edit_Notify {
  friend class CPWL_Edit_Notify;

 public:
  CPWL_EditCtrl();
  ~CPWL_EditCtrl() override;

  CFX_FloatRect GetContentRect() const;
  void GetCaretPos(int32_t& x, int32_t& y) const;

  CFX_WideString GetText() const;
  void SetSel(int32_t nStartChar, int32_t nEndChar);
  void GetSel(int32_t& nStartChar, int32_t& nEndChar) const;
  void GetTextRange(const CFX_FloatRect& rect,
                    int32_t& nStartChar,
                    int32_t& nEndChar) const;
  CFX_WideString GetText(int32_t& nStartChar, int32_t& nEndChar) const;
  void Clear();
  void SelectAll();

  int32_t GetCaret() const;
  void SetCaret(int32_t nPos);
  int32_t GetTotalWords() const;

  void Paint();

  void EnableRefresh(FX_BOOL bRefresh);
  CFX_FloatPoint GetScrollPos() const;
  void SetScrollPos(const CFX_FloatPoint& point);

  void SetEditNotify(IPWL_Edit_Notify* pNotify) { m_pEditNotify = pNotify; }

  void SetCharSet(uint8_t nCharSet) { m_nCharSet = nCharSet; }
  int32_t GetCharSet() const;

  void SetCodePage(int32_t nCodePage) { m_nCodePage = nCodePage; }
  int32_t GetCodePage() const { return m_nCodePage; }

  CPDF_Font* GetCaretFont() const;
  FX_FLOAT GetCaretFontSize() const;

  FX_BOOL CanUndo() const;
  FX_BOOL CanRedo() const;
  void Redo();
  void Undo();

  void SetReadyToInput();

  // CPWL_Wnd
  void OnCreate(PWL_CREATEPARAM& cp) override;
  void OnCreated() override;
  FX_BOOL OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  FX_BOOL OnChar(uint16_t nChar, uint32_t nFlag) override;
  FX_BOOL OnLButtonDown(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnLButtonUp(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnMouseMove(const CFX_FloatPoint& point, uint32_t nFlag) override;
  void OnNotify(CPWL_Wnd* pWnd,
                uint32_t msg,
                intptr_t wParam = 0,
                intptr_t lParam = 0) override;
  void CreateChildWnd(const PWL_CREATEPARAM& cp) override;
  void RePosChildWnd() override;
  void SetFontSize(FX_FLOAT fFontSize) override;
  FX_FLOAT GetFontSize() const override;
  void SetCursor() override;

 protected:
  // IFX_Edit_Notify
  void IOnSetScrollInfoX(FX_FLOAT fPlateMin,
                         FX_FLOAT fPlateMax,
                         FX_FLOAT fContentMin,
                         FX_FLOAT fContentMax,
                         FX_FLOAT fSmallStep,
                         FX_FLOAT fBigStep) override {}
  void IOnSetScrollInfoY(FX_FLOAT fPlateMin,
                         FX_FLOAT fPlateMax,
                         FX_FLOAT fContentMin,
                         FX_FLOAT fContentMax,
                         FX_FLOAT fSmallStep,
                         FX_FLOAT fBigStep) override;
  void IOnSetScrollPosX(FX_FLOAT fx) override {}
  void IOnSetScrollPosY(FX_FLOAT fy) override;
  void IOnSetCaret(FX_BOOL bVisible,
                   const CFX_FloatPoint& ptHead,
                   const CFX_FloatPoint& ptFoot,
                   const CPVT_WordPlace& place) override;
  void IOnCaretChange(const CPVT_SecProps& secProps,
                      const CPVT_WordProps& wordProps) override;
  void IOnContentChange(const CFX_FloatRect& rcContent) override;
  void IOnInvalidateRect(CFX_FloatRect* pRect) override;

  void InsertText(const FX_WCHAR* csText);
  void SetText(const FX_WCHAR* csText);
  void CopyText();
  void PasteText();
  void CutText();
  void ShowVScrollBar(FX_BOOL bShow);
  void InsertWord(uint16_t word, int32_t nCharset);
  void InsertReturn();

  FX_BOOL IsWndHorV();

  void Delete();
  void Backspace();

  void GetCaretInfo(CFX_FloatPoint& ptHead, CFX_FloatPoint& ptFoot) const;
  void SetCaret(FX_BOOL bVisible,
                const CFX_FloatPoint& ptHead,
                const CFX_FloatPoint& ptFoot);

  void SetEditCaret(FX_BOOL bVisible);

 private:
  void CreateEditCaret(const PWL_CREATEPARAM& cp);

 protected:
  IFX_Edit* m_pEdit;
  CPWL_Caret* m_pEditCaret;
  FX_BOOL m_bMouseDown;
  IPWL_Edit_Notify* m_pEditNotify;

 private:
  int32_t m_nCharSet;
  int32_t m_nCodePage;
};

#endif  // FPDFSDK_PDFWINDOW_PWL_EDITCTRL_H_
