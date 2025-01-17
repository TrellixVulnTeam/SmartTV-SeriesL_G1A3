// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/cpdf_standard_crypto_handler.h"

#include <time.h>

#include "core/fdrm/crypto/include/fx_crypt.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfapi/fpdf_parser/ipdf_security_handler.h"

IPDF_CryptoHandler::~IPDF_CryptoHandler() {}

void IPDF_CryptoHandler::Decrypt(uint32_t objnum,
                                 uint32_t gennum,
                                 CFX_ByteString& str) {
  CFX_BinaryBuf dest_buf;
  void* context = DecryptStart(objnum, gennum);
  DecryptStream(context, (const uint8_t*)str, str.GetLength(), dest_buf);
  DecryptFinish(context, dest_buf);
  str = dest_buf;
}

void CPDF_StandardCryptoHandler::CryptBlock(FX_BOOL bEncrypt,
                                            uint32_t objnum,
                                            uint32_t gennum,
                                            const uint8_t* src_buf,
                                            uint32_t src_size,
                                            uint8_t* dest_buf,
                                            uint32_t& dest_size) {
  if (m_Cipher == FXCIPHER_NONE) {
    FXSYS_memcpy(dest_buf, src_buf, src_size);
    return;
  }
  uint8_t realkey[16];
  int realkeylen = 16;
  if (m_Cipher != FXCIPHER_AES || m_KeyLen != 32) {
    uint8_t key1[32];
    FXSYS_memcpy(key1, m_EncryptKey, m_KeyLen);
    key1[m_KeyLen + 0] = (uint8_t)objnum;
    key1[m_KeyLen + 1] = (uint8_t)(objnum >> 8);
    key1[m_KeyLen + 2] = (uint8_t)(objnum >> 16);
    key1[m_KeyLen + 3] = (uint8_t)gennum;
    key1[m_KeyLen + 4] = (uint8_t)(gennum >> 8);
    FXSYS_memcpy(key1 + m_KeyLen, &objnum, 3);
    FXSYS_memcpy(key1 + m_KeyLen + 3, &gennum, 2);
    if (m_Cipher == FXCIPHER_AES) {
      FXSYS_memcpy(key1 + m_KeyLen + 5, "sAlT", 4);
    }
    CRYPT_MD5Generate(
        key1, m_Cipher == FXCIPHER_AES ? m_KeyLen + 9 : m_KeyLen + 5, realkey);
    realkeylen = m_KeyLen + 5;
    if (realkeylen > 16) {
      realkeylen = 16;
    }
  }
  if (m_Cipher == FXCIPHER_AES) {
    CRYPT_AESSetKey(m_pAESContext, 16, m_KeyLen == 32 ? m_EncryptKey : realkey,
                    m_KeyLen, bEncrypt);
    if (bEncrypt) {
      uint8_t iv[16];
      for (int i = 0; i < 16; i++) {
        iv[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(m_pAESContext, iv);
      FXSYS_memcpy(dest_buf, iv, 16);
      int nblocks = src_size / 16;
      CRYPT_AESEncrypt(m_pAESContext, dest_buf + 16, src_buf, nblocks * 16);
      uint8_t padding[16];
      FXSYS_memcpy(padding, src_buf + nblocks * 16, src_size % 16);
      FXSYS_memset(padding + src_size % 16, 16 - src_size % 16,
                   16 - src_size % 16);
      CRYPT_AESEncrypt(m_pAESContext, dest_buf + nblocks * 16 + 16, padding,
                       16);
      dest_size = 32 + nblocks * 16;
    } else {
      CRYPT_AESSetIV(m_pAESContext, src_buf);
      CRYPT_AESDecrypt(m_pAESContext, dest_buf, src_buf + 16, src_size - 16);
      dest_size = src_size - 16;
      dest_size -= dest_buf[dest_size - 1];
    }
  } else {
    ASSERT(dest_size == src_size);
    if (dest_buf != src_buf) {
      FXSYS_memcpy(dest_buf, src_buf, src_size);
    }
    CRYPT_ArcFourCryptBlock(dest_buf, dest_size, realkey, realkeylen);
  }
}

struct AESCryptContext {
  uint8_t m_Context[2048];
  FX_BOOL m_bIV;
  uint8_t m_Block[16];
  uint32_t m_BlockOffset;
};

void* CPDF_StandardCryptoHandler::CryptStart(uint32_t objnum,
                                             uint32_t gennum,
                                             FX_BOOL bEncrypt) {
  if (m_Cipher == FXCIPHER_NONE) {
    return this;
  }
  if (m_Cipher == FXCIPHER_AES && m_KeyLen == 32) {
    AESCryptContext* pContext = FX_Alloc(AESCryptContext, 1);
    pContext->m_bIV = TRUE;
    pContext->m_BlockOffset = 0;
    CRYPT_AESSetKey(pContext->m_Context, 16, m_EncryptKey, 32, bEncrypt);
    if (bEncrypt) {
      for (int i = 0; i < 16; i++) {
        pContext->m_Block[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(pContext->m_Context, pContext->m_Block);
    }
    return pContext;
  }
  uint8_t key1[48];
  FXSYS_memcpy(key1, m_EncryptKey, m_KeyLen);
  FXSYS_memcpy(key1 + m_KeyLen, &objnum, 3);
  FXSYS_memcpy(key1 + m_KeyLen + 3, &gennum, 2);
  if (m_Cipher == FXCIPHER_AES) {
    FXSYS_memcpy(key1 + m_KeyLen + 5, "sAlT", 4);
  }
  uint8_t realkey[16];
  CRYPT_MD5Generate(
      key1, m_Cipher == FXCIPHER_AES ? m_KeyLen + 9 : m_KeyLen + 5, realkey);
  int realkeylen = m_KeyLen + 5;
  if (realkeylen > 16) {
    realkeylen = 16;
  }
  if (m_Cipher == FXCIPHER_AES) {
    AESCryptContext* pContext = FX_Alloc(AESCryptContext, 1);
    pContext->m_bIV = TRUE;
    pContext->m_BlockOffset = 0;
    CRYPT_AESSetKey(pContext->m_Context, 16, realkey, 16, bEncrypt);
    if (bEncrypt) {
      for (int i = 0; i < 16; i++) {
        pContext->m_Block[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(pContext->m_Context, pContext->m_Block);
    }
    return pContext;
  }
  void* pContext = FX_Alloc(uint8_t, 1040);
  CRYPT_ArcFourSetup(pContext, realkey, realkeylen);
  return pContext;
}
FX_BOOL CPDF_StandardCryptoHandler::CryptStream(void* context,
                                                const uint8_t* src_buf,
                                                uint32_t src_size,
                                                CFX_BinaryBuf& dest_buf,
                                                FX_BOOL bEncrypt) {
  if (!context) {
    return FALSE;
  }
  if (m_Cipher == FXCIPHER_NONE) {
    dest_buf.AppendBlock(src_buf, src_size);
    return TRUE;
  }
  if (m_Cipher == FXCIPHER_RC4) {
    int old_size = dest_buf.GetSize();
    dest_buf.AppendBlock(src_buf, src_size);
    CRYPT_ArcFourCrypt(context, dest_buf.GetBuffer() + old_size, src_size);
    return TRUE;
  }
  AESCryptContext* pContext = (AESCryptContext*)context;
  if (pContext->m_bIV && bEncrypt) {
    dest_buf.AppendBlock(pContext->m_Block, 16);
    pContext->m_bIV = FALSE;
  }
  uint32_t src_off = 0;
  uint32_t src_left = src_size;
  while (1) {
    uint32_t copy_size = 16 - pContext->m_BlockOffset;
    if (copy_size > src_left) {
      copy_size = src_left;
    }
    FXSYS_memcpy(pContext->m_Block + pContext->m_BlockOffset, src_buf + src_off,
                 copy_size);
    src_off += copy_size;
    src_left -= copy_size;
    pContext->m_BlockOffset += copy_size;
    if (pContext->m_BlockOffset == 16) {
      if (!bEncrypt && pContext->m_bIV) {
        CRYPT_AESSetIV(pContext->m_Context, pContext->m_Block);
        pContext->m_bIV = FALSE;
        pContext->m_BlockOffset = 0;
      } else if (src_off < src_size) {
        uint8_t block_buf[16];
        if (bEncrypt) {
          CRYPT_AESEncrypt(pContext->m_Context, block_buf, pContext->m_Block,
                           16);
        } else {
          CRYPT_AESDecrypt(pContext->m_Context, block_buf, pContext->m_Block,
                           16);
        }
        dest_buf.AppendBlock(block_buf, 16);
        pContext->m_BlockOffset = 0;
      }
    }
    if (!src_left) {
      break;
    }
  }
  return TRUE;
}
FX_BOOL CPDF_StandardCryptoHandler::CryptFinish(void* context,
                                                CFX_BinaryBuf& dest_buf,
                                                FX_BOOL bEncrypt) {
  if (!context) {
    return FALSE;
  }
  if (m_Cipher == FXCIPHER_NONE) {
    return TRUE;
  }
  if (m_Cipher == FXCIPHER_RC4) {
    FX_Free(context);
    return TRUE;
  }
  AESCryptContext* pContext = (AESCryptContext*)context;
  if (bEncrypt) {
    uint8_t block_buf[16];
    if (pContext->m_BlockOffset == 16) {
      CRYPT_AESEncrypt(pContext->m_Context, block_buf, pContext->m_Block, 16);
      dest_buf.AppendBlock(block_buf, 16);
      pContext->m_BlockOffset = 0;
    }
    FXSYS_memset(pContext->m_Block + pContext->m_BlockOffset,
                 (uint8_t)(16 - pContext->m_BlockOffset),
                 16 - pContext->m_BlockOffset);
    CRYPT_AESEncrypt(pContext->m_Context, block_buf, pContext->m_Block, 16);
    dest_buf.AppendBlock(block_buf, 16);
  } else if (pContext->m_BlockOffset == 16) {
    uint8_t block_buf[16];
    CRYPT_AESDecrypt(pContext->m_Context, block_buf, pContext->m_Block, 16);
    if (block_buf[15] <= 16) {
      dest_buf.AppendBlock(block_buf, 16 - block_buf[15]);
    }
  }
  FX_Free(pContext);
  return TRUE;
}
void* CPDF_StandardCryptoHandler::DecryptStart(uint32_t objnum,
                                               uint32_t gennum) {
  return CryptStart(objnum, gennum, FALSE);
}
uint32_t CPDF_StandardCryptoHandler::DecryptGetSize(uint32_t src_size) {
  return m_Cipher == FXCIPHER_AES ? src_size - 16 : src_size;
}

FX_BOOL CPDF_StandardCryptoHandler::Init(
    CPDF_Dictionary* pEncryptDict,
    IPDF_SecurityHandler* pSecurityHandler) {
  const uint8_t* key;
  if (!pSecurityHandler->GetCryptInfo(m_Cipher, key, m_KeyLen)) {
    return FALSE;
  }
  if (m_KeyLen > 32 || m_KeyLen < 0) {
    return FALSE;
  }
  if (m_Cipher != FXCIPHER_NONE) {
    FXSYS_memcpy(m_EncryptKey, key, m_KeyLen);
  }
  if (m_Cipher == FXCIPHER_AES) {
    m_pAESContext = FX_Alloc(uint8_t, 2048);
  }
  return TRUE;
}

FX_BOOL CPDF_StandardCryptoHandler::Init(int cipher,
                                         const uint8_t* key,
                                         int keylen) {
  if (cipher == FXCIPHER_AES) {
    switch (keylen) {
      case 16:
      case 24:
      case 32:
        break;
      default:
        return FALSE;
    }
  } else if (cipher == FXCIPHER_AES2) {
    if (keylen != 32) {
      return FALSE;
    }
  } else if (cipher == FXCIPHER_RC4) {
    if (keylen < 5 || keylen > 16) {
      return FALSE;
    }
  } else {
    if (keylen > 32) {
      keylen = 32;
    }
  }
  m_Cipher = cipher;
  m_KeyLen = keylen;
  FXSYS_memcpy(m_EncryptKey, key, keylen);
  if (m_Cipher == FXCIPHER_AES) {
    m_pAESContext = FX_Alloc(uint8_t, 2048);
  }
  return TRUE;
}
FX_BOOL CPDF_StandardCryptoHandler::DecryptStream(void* context,
                                                  const uint8_t* src_buf,
                                                  uint32_t src_size,
                                                  CFX_BinaryBuf& dest_buf) {
  return CryptStream(context, src_buf, src_size, dest_buf, FALSE);
}
FX_BOOL CPDF_StandardCryptoHandler::DecryptFinish(void* context,
                                                  CFX_BinaryBuf& dest_buf) {
  return CryptFinish(context, dest_buf, FALSE);
}
uint32_t CPDF_StandardCryptoHandler::EncryptGetSize(uint32_t objnum,
                                                    uint32_t version,
                                                    const uint8_t* src_buf,
                                                    uint32_t src_size) {
  if (m_Cipher == FXCIPHER_AES) {
    return src_size + 32;
  }
  return src_size;
}
FX_BOOL CPDF_StandardCryptoHandler::EncryptContent(uint32_t objnum,
                                                   uint32_t gennum,
                                                   const uint8_t* src_buf,
                                                   uint32_t src_size,
                                                   uint8_t* dest_buf,
                                                   uint32_t& dest_size) {
  CryptBlock(TRUE, objnum, gennum, src_buf, src_size, dest_buf, dest_size);
  return TRUE;
}
CPDF_StandardCryptoHandler::CPDF_StandardCryptoHandler() {
  m_pAESContext = NULL;
  m_Cipher = FXCIPHER_NONE;
  m_KeyLen = 0;
}
CPDF_StandardCryptoHandler::~CPDF_StandardCryptoHandler() {
  FX_Free(m_pAESContext);
}
