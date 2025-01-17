// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_CPDF_STANDARD_SECURITY_HANDLER_H_
#define CORE_FPDFAPI_FPDF_PARSER_CPDF_STANDARD_SECURITY_HANDLER_H_

#include "core/fpdfapi/fpdf_parser/ipdf_security_handler.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Array;

#define PDF_ENCRYPT_CONTENT 0

class CPDF_StandardSecurityHandler : public IPDF_SecurityHandler {
 public:
  CPDF_StandardSecurityHandler();
  ~CPDF_StandardSecurityHandler() override;

  // IPDF_SecurityHandler:
  FX_BOOL OnInit(CPDF_Parser* pParser, CPDF_Dictionary* pEncryptDict) override;
  uint32_t GetPermissions() override;
  FX_BOOL GetCryptInfo(int& cipher,
                       const uint8_t*& buffer,
                       int& keylen) override;
  FX_BOOL IsMetadataEncrypted() override;
  IPDF_CryptoHandler* CreateCryptoHandler() override;

  void OnCreate(CPDF_Dictionary* pEncryptDict,
                CPDF_Array* pIdArray,
                const uint8_t* user_pass,
                uint32_t user_size,
                const uint8_t* owner_pass,
                uint32_t owner_size,
                uint32_t type = PDF_ENCRYPT_CONTENT);

  void OnCreate(CPDF_Dictionary* pEncryptDict,
                CPDF_Array* pIdArray,
                const uint8_t* user_pass,
                uint32_t user_size,
                uint32_t type = PDF_ENCRYPT_CONTENT);

  CFX_ByteString GetUserPassword(const uint8_t* owner_pass,
                                 uint32_t pass_size,
                                 int32_t key_len);
  int CheckPassword(const uint8_t* password,
                    uint32_t pass_size,
                    FX_BOOL bOwner,
                    uint8_t* key,
                    int key_len);

 private:
  FX_BOOL LoadDict(CPDF_Dictionary* pEncryptDict);
  FX_BOOL LoadDict(CPDF_Dictionary* pEncryptDict,
                   uint32_t type,
                   int& cipher,
                   int& key_len);

  FX_BOOL CheckUserPassword(const uint8_t* password,
                            uint32_t pass_size,
                            FX_BOOL bIgnoreEncryptMeta,
                            uint8_t* key,
                            int32_t key_len);

  FX_BOOL CheckOwnerPassword(const uint8_t* password,
                             uint32_t pass_size,
                             uint8_t* key,
                             int32_t key_len);
  FX_BOOL AES256_CheckPassword(const uint8_t* password,
                               uint32_t size,
                               FX_BOOL bOwner,
                               uint8_t* key);
  void AES256_SetPassword(CPDF_Dictionary* pEncryptDict,
                          const uint8_t* password,
                          uint32_t size,
                          FX_BOOL bOwner,
                          const uint8_t* key);
  void AES256_SetPerms(CPDF_Dictionary* pEncryptDict,
                       uint32_t permission,
                       FX_BOOL bEncryptMetadata,
                       const uint8_t* key);
  void OnCreate(CPDF_Dictionary* pEncryptDict,
                CPDF_Array* pIdArray,
                const uint8_t* user_pass,
                uint32_t user_size,
                const uint8_t* owner_pass,
                uint32_t owner_size,
                FX_BOOL bDefault,
                uint32_t type);
  FX_BOOL CheckSecurity(int32_t key_len);

  int m_Version;
  int m_Revision;
  CPDF_Parser* m_pParser;
  CPDF_Dictionary* m_pEncryptDict;
  uint32_t m_Permissions;
  int m_Cipher;
  uint8_t m_EncryptKey[32];
  int m_KeyLen;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_CPDF_STANDARD_SECURITY_HANDLER_H_
