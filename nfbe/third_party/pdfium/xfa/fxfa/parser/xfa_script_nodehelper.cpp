// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_nodehelper.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fxfa/fm2js/xfa_fm2jsapi.h"
#include "xfa/fxfa/parser/xfa_docdata.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_parser.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_NodeHelper::CXFA_NodeHelper(void)
    : m_eLastCreateType(XFA_ELEMENT_DataValue),
      m_pCreateParent(NULL),
      m_iCreateCount(0),
      m_iCreateFlag(XFA_RESOLVENODE_RSTYPE_CreateNodeOne),
      m_iCurAllStart(-1),
      m_pAllStartParent(NULL) {}
CXFA_NodeHelper::~CXFA_NodeHelper(void) {}
CXFA_Node* CXFA_NodeHelper::XFA_ResolveNodes_GetOneChild(
    CXFA_Node* parent,
    const FX_WCHAR* pwsName,
    FX_BOOL bIsClassName) {
  if (parent == NULL) {
    return NULL;
  }
  CXFA_NodeArray siblings;
  uint32_t uNameHash = FX_HashCode_String_GetW(pwsName, FXSYS_wcslen(pwsName));
  XFA_NodeAcc_TraverseAnySiblings(parent, uNameHash, &siblings, bIsClassName);
  if (siblings.GetSize() == 0) {
    return NULL;
  }
  return siblings[0];
}
int32_t CXFA_NodeHelper::XFA_CountSiblings(CXFA_Node* pNode,
                                           XFA_LOGIC_TYPE eLogicType,
                                           CXFA_NodeArray* pSiblings,
                                           FX_BOOL bIsClassName) {
  if (!pNode)
    return 0;
  CXFA_Node* parent =
      XFA_ResolveNodes_GetParent(pNode, XFA_LOGIC_NoTransparent);
  if (!parent)
    return 0;
  const XFA_PROPERTY* pProperty = XFA_GetPropertyOfElement(
      parent->GetClassID(), pNode->GetClassID(), XFA_XDPPACKET_UNKNOWN);
  if (!pProperty && eLogicType == XFA_LOGIC_Transparent) {
    parent = XFA_ResolveNodes_GetParent(pNode, XFA_LOGIC_Transparent);
    if (parent == NULL) {
      return 0;
    }
  }
  if (bIsClassName) {
    return XFA_NodeAcc_TraverseSiblings(parent, pNode->GetClassHashCode(),
                                        pSiblings, eLogicType, bIsClassName);
  } else {
    return XFA_NodeAcc_TraverseSiblings(parent, pNode->GetNameHash(), pSiblings,
                                        eLogicType, bIsClassName);
  }
}
int32_t CXFA_NodeHelper::XFA_NodeAcc_TraverseAnySiblings(
    CXFA_Node* parent,
    uint32_t dNameHash,
    CXFA_NodeArray* pSiblings,
    FX_BOOL bIsClassName) {
  if (parent == NULL || pSiblings == NULL) {
    return 0;
  }
  int32_t nCount = 0;
  int32_t i = 0;
  CXFA_NodeArray properties;
  parent->GetNodeList(properties, XFA_NODEFILTER_Properties);
  int32_t nProperties = properties.GetSize();
  for (i = 0; i < nProperties; ++i) {
    CXFA_Node* child = properties[i];
    if (bIsClassName) {
      if (child->GetClassHashCode() == dNameHash) {
        pSiblings->Add(child);
        nCount++;
      }
    } else {
      if (child->GetNameHash() == dNameHash) {
        pSiblings->Add(child);
        nCount++;
      }
    }
    if (nCount > 0) {
      return nCount;
    }
    nCount += XFA_NodeAcc_TraverseAnySiblings(child, dNameHash, pSiblings,
                                              bIsClassName);
  }
  CXFA_NodeArray children;
  parent->GetNodeList(children, XFA_NODEFILTER_Children);
  int32_t nChildren = children.GetSize();
  for (i = 0; i < nChildren; i++) {
    CXFA_Node* child = children[i];
    if (bIsClassName) {
      if (child->GetClassHashCode() == dNameHash) {
        if (pSiblings) {
          pSiblings->Add(child);
        }
        nCount++;
      }
    } else {
      if (child->GetNameHash() == dNameHash) {
        if (pSiblings) {
          pSiblings->Add(child);
        }
        nCount++;
      }
    }
    if (nCount > 0) {
      return nCount;
    }
    nCount += XFA_NodeAcc_TraverseAnySiblings(child, dNameHash, pSiblings,
                                              bIsClassName);
  }
  return nCount;
}
int32_t CXFA_NodeHelper::XFA_NodeAcc_TraverseSiblings(CXFA_Node* parent,
                                                      uint32_t dNameHash,
                                                      CXFA_NodeArray* pSiblings,
                                                      XFA_LOGIC_TYPE eLogicType,
                                                      FX_BOOL bIsClassName,
                                                      FX_BOOL bIsFindProperty) {
  if (parent == NULL || pSiblings == NULL) {
    return 0;
  }
  int32_t nCount = 0;
  int32_t i = 0;
  if (bIsFindProperty) {
    CXFA_NodeArray properties;
    parent->GetNodeList(properties, XFA_NODEFILTER_Properties);
    int32_t nProperties = properties.GetSize();
    for (i = 0; i < nProperties; ++i) {
      CXFA_Node* child = properties[i];
      if (bIsClassName) {
        if (child->GetClassHashCode() == dNameHash) {
          pSiblings->Add(child);
          nCount++;
        }
      } else {
        if (child->GetNameHash() == dNameHash) {
          if (child->GetClassID() != XFA_ELEMENT_PageSet &&
              child->GetClassID() != XFA_ELEMENT_Extras &&
              child->GetClassID() != XFA_ELEMENT_Items) {
            pSiblings->Add(child);
            nCount++;
          }
        }
      }
      if (child->IsUnnamed() && child->GetClassID() == XFA_ELEMENT_PageSet) {
        nCount += XFA_NodeAcc_TraverseSiblings(child, dNameHash, pSiblings,
                                               eLogicType, bIsClassName, FALSE);
      }
    }
    if (nCount > 0) {
      return nCount;
    }
  }
  CXFA_NodeArray children;
  parent->GetNodeList(children, XFA_NODEFILTER_Children);
  int32_t nChildren = children.GetSize();
  for (i = 0; i < nChildren; i++) {
    CXFA_Node* child = children[i];
    if (child->GetClassID() == XFA_ELEMENT_Variables) {
      continue;
    }
    if (bIsClassName) {
      if (child->GetClassHashCode() == dNameHash) {
        if (pSiblings) {
          pSiblings->Add(child);
        }
        nCount++;
      }
    } else {
      if (child->GetNameHash() == dNameHash) {
        if (pSiblings) {
          pSiblings->Add(child);
        }
        nCount++;
      }
    }
    if (eLogicType == XFA_LOGIC_NoTransparent) {
      continue;
    }
    if (XFA_NodeIsTransparent(child) &&
        child->GetClassID() != XFA_ELEMENT_PageSet) {
      nCount += XFA_NodeAcc_TraverseSiblings(child, dNameHash, pSiblings,
                                             eLogicType, bIsClassName, FALSE);
    }
  }
  return nCount;
}
CXFA_Node* CXFA_NodeHelper::XFA_ResolveNodes_GetParent(
    CXFA_Node* pNode,
    XFA_LOGIC_TYPE eLogicType) {
  if (!pNode) {
    return NULL;
  }
  if (eLogicType == XFA_LOGIC_NoTransparent) {
    return pNode->GetNodeItem(XFA_NODEITEM_Parent);
  }
  CXFA_Node* parent;
  CXFA_Node* node = pNode;
  while (TRUE) {
    parent = XFA_ResolveNodes_GetParent(node);
    if (parent == NULL) {
      break;
    }
    XFA_ELEMENT parentElement = parent->GetClassID();
    if ((!parent->IsUnnamed() && parentElement != XFA_ELEMENT_SubformSet) ||
        parentElement == XFA_ELEMENT_Variables) {
      break;
    }
    node = parent;
  }
  return parent;
}
int32_t CXFA_NodeHelper::XFA_GetIndex(CXFA_Node* pNode,
                                      XFA_LOGIC_TYPE eLogicType,
                                      FX_BOOL bIsProperty,
                                      FX_BOOL bIsClassIndex) {
  CXFA_Node* parent =
      XFA_ResolveNodes_GetParent(pNode, XFA_LOGIC_NoTransparent);
  if (parent == NULL) {
    return 0;
  }
  if (!bIsProperty && eLogicType == XFA_LOGIC_Transparent) {
    parent = XFA_ResolveNodes_GetParent(pNode, XFA_LOGIC_Transparent);
    if (parent == NULL) {
      return 0;
    }
  }
  uint32_t dwHashName = pNode->GetNameHash();
  if (bIsClassIndex) {
    dwHashName = pNode->GetClassHashCode();
  }
  CXFA_NodeArray siblings;
  int32_t iSize = XFA_NodeAcc_TraverseSiblings(parent, dwHashName, &siblings,
                                               eLogicType, bIsClassIndex);
  for (int32_t i = 0; i < iSize; ++i) {
    CXFA_Node* child = siblings[i];
    if (child == pNode) {
      return i;
    }
  }
  return 0;
}
void CXFA_NodeHelper::XFA_GetNameExpression(CXFA_Node* refNode,
                                            CFX_WideString& wsName,
                                            FX_BOOL bIsAllPath,
                                            XFA_LOGIC_TYPE eLogicType) {
  wsName.Empty();
  if (bIsAllPath) {
    XFA_GetNameExpression(refNode, wsName, FALSE, eLogicType);
    CFX_WideString wsParent;
    CXFA_Node* parent =
        XFA_ResolveNodes_GetParent(refNode, XFA_LOGIC_NoTransparent);
    while (parent) {
      XFA_GetNameExpression(parent, wsParent, FALSE, eLogicType);
      wsParent += L".";
      wsParent += wsName;
      wsName = wsParent;
      parent = XFA_ResolveNodes_GetParent(parent, XFA_LOGIC_NoTransparent);
    }
    return;
  }

  CFX_WideStringC wsTagName;
  CFX_WideString ws;
  FX_BOOL bIsProperty = XFA_NodeIsProperty(refNode);
  if (refNode->IsUnnamed() ||
      (bIsProperty && refNode->GetClassID() != XFA_ELEMENT_PageSet)) {
    refNode->GetClassName(wsTagName);
    ws = wsTagName;
    wsName.Format(L"#%s[%d]", (const FX_WCHAR*)ws,
                  XFA_GetIndex(refNode, eLogicType, bIsProperty, TRUE));
    return;
  }
  ws = refNode->GetCData(XFA_ATTRIBUTE_Name);
  ws.Replace(L".", L"\\.");
  wsName.Format(L"%s[%d]", (const FX_WCHAR*)ws,
                XFA_GetIndex(refNode, eLogicType, bIsProperty, FALSE));
}

FX_BOOL CXFA_NodeHelper::XFA_NodeIsTransparent(CXFA_Node* refNode) {
  if (refNode == NULL) {
    return FALSE;
  }
  XFA_ELEMENT eRefNode = refNode->GetClassID();
  if ((refNode->IsUnnamed() && refNode->IsContainerNode()) ||
      eRefNode == XFA_ELEMENT_SubformSet || eRefNode == XFA_ELEMENT_Area ||
      eRefNode == XFA_ELEMENT_Proto) {
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_NodeHelper::XFA_CreateNode_ForCondition(
    CFX_WideString& wsCondition) {
  int32_t iLen = wsCondition.GetLength();
  CFX_WideString wsIndex = FX_WSTRC(L"0");
  FX_BOOL bAll = FALSE;

  if (iLen == 0) {
    m_iCreateFlag = XFA_RESOLVENODE_RSTYPE_CreateNodeOne;
    return FALSE;
  }
  if (wsCondition.GetAt(0) == '[') {
    int32_t i = 1;
    for (; i < iLen; ++i) {
      FX_WCHAR ch = wsCondition[i];
      if (ch == ' ') {
        continue;
      }
      if (ch == '+' || ch == '-') {
        break;
      } else if (ch == '*') {
        bAll = TRUE;
        break;
      } else {
        break;
      }
    }
    if (bAll) {
      wsIndex = FX_WSTRC(L"1");
      m_iCreateFlag = XFA_RESOLVENODE_RSTYPE_CreateNodeAll;
    } else {
      m_iCreateFlag = XFA_RESOLVENODE_RSTYPE_CreateNodeOne;
      wsIndex = wsCondition.Mid(i, iLen - 1 - i);
    }
    int32_t iIndex = wsIndex.GetInteger();
    m_iCreateCount = iIndex;
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_NodeHelper::XFA_ResolveNodes_CreateNode(
    CFX_WideString wsName,
    CFX_WideString wsCondition,
    FX_BOOL bLastNode,
    CXFA_ScriptContext* pScriptContext) {
  if (m_pCreateParent == NULL) {
    return FALSE;
  }
  FX_BOOL bIsClassName = FALSE;
  FX_BOOL bResult = FALSE;
  if (wsName.GetAt(0) == '!') {
    wsName = wsName.Right(wsName.GetLength() - 1);
    m_pCreateParent = ToNode(
        pScriptContext->GetDocument()->GetXFAObject(XFA_HASHCODE_Datasets));
  }
  if (wsName.GetAt(0) == '#') {
    bIsClassName = TRUE;
    wsName = wsName.Right(wsName.GetLength() - 1);
  }
  if (m_iCreateCount == 0) {
    XFA_CreateNode_ForCondition(wsCondition);
  }
  if (bIsClassName) {
    const XFA_ELEMENTINFO* lpElement =
        XFA_GetElementByName(wsName.AsWideStringC());
    if (lpElement == NULL) {
      return FALSE;
    }
    for (int32_t iIndex = 0; iIndex < m_iCreateCount; iIndex++) {
      CXFA_Node* pNewNode =
          m_pCreateParent->CreateSamePacketNode(lpElement->eName);
      if (pNewNode) {
        m_pCreateParent->InsertChild(pNewNode);
        if (iIndex == m_iCreateCount - 1) {
          m_pCreateParent = pNewNode;
        }
        bResult = TRUE;
      }
    }
  } else {
    XFA_ELEMENT eClassType = XFA_ELEMENT_DataGroup;
    if (bLastNode) {
      eClassType = m_eLastCreateType;
    }
    for (int32_t iIndex = 0; iIndex < m_iCreateCount; iIndex++) {
      CXFA_Node* pNewNode = m_pCreateParent->CreateSamePacketNode(eClassType);
      if (pNewNode) {
        pNewNode->SetAttribute(XFA_ATTRIBUTE_Name, wsName.AsWideStringC());
        pNewNode->CreateXMLMappingNode();
        m_pCreateParent->InsertChild(pNewNode);
        if (iIndex == m_iCreateCount - 1) {
          m_pCreateParent = pNewNode;
        }
        bResult = TRUE;
      }
    }
  }
  if (!bResult) {
    m_pCreateParent = NULL;
  }
  return bResult;
}
void CXFA_NodeHelper::XFA_SetCreateNodeType(CXFA_Node* refNode) {
  if (refNode == NULL) {
    return;
  }
  if (refNode->GetClassID() == XFA_ELEMENT_Subform) {
    m_eLastCreateType = XFA_ELEMENT_DataGroup;
  } else if (refNode->GetClassID() == XFA_ELEMENT_Field) {
    m_eLastCreateType = XFA_FieldIsMultiListBox(refNode)
                            ? XFA_ELEMENT_DataGroup
                            : XFA_ELEMENT_DataValue;
  } else if (refNode->GetClassID() == XFA_ELEMENT_ExclGroup) {
    m_eLastCreateType = XFA_ELEMENT_DataValue;
  }
}
FX_BOOL CXFA_NodeHelper::XFA_NodeIsProperty(CXFA_Node* refNode) {
  CXFA_Node* parent =
      XFA_ResolveNodes_GetParent(refNode, XFA_LOGIC_NoTransparent);
  return parent && refNode &&
         XFA_GetPropertyOfElement(parent->GetClassID(), refNode->GetClassID(),
                                  XFA_XDPPACKET_UNKNOWN);
}
