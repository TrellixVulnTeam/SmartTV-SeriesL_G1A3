// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/include/fxfa/xfa_fontmgr.h"

#include <algorithm>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "xfa/include/fxfa/xfa_ffapp.h"
#include "xfa/include/fxfa/xfa_ffdoc.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
static const XFA_FONTINFO g_XFAFontsMap[] = {
    {0x01d5d33e, L"SimSun", L"Arial", 0, 936},
    {0x01e4f102, L"YouYuan", L"Arial", 1, 936},
    {0x030549dc, L"LiSu", L"Arial", 1, 936},
    {0x032edd44, L"Simhei", L"Arial", 1, 936},
    {0x03eac6fc, L"PoorRichard-Regular", L"Arial", 2, 1252},
    {0x03ed90e6, L"Nina", L"Arial", 0, 1252},
    {0x077b56b3, L"KingsoftPhoneticPlain", L"Arial", 0, 1252},
    {0x078ed524, L"MicrosoftSansSerif", L"Arial", 0, 1252},
    {0x089b18a9, L"Arial", L"Arial", 0, 1252},
    {0x0b2cad72, L"MonotypeCorsiva", L"Arial", 8, 1252},
    {0x0bb003e7, L"Kartika", L"Arial", 2, 1252},
    {0x0bb469df, L"VinerHandITC", L"Arial", 8, 1252},
    {0x0bc1a851, L"SegoeUI", L"Arial", 0, 1252},
    {0x0c112ebd, L"KozukaGothicPro-VIM", L"Arial", 0, 1252},
    {0x0cfcb9c1, L"AdobeThai", L"Kokila,Arial Narrow", 0, 847},
    {0x0e7de0f9, L"Playbill", L"Arial", 0, 1252},
    {0x0eff47c3, L"STHupo", L"Arial", 0, 936},
    {0x107ad374, L"Constantia", L"Arial", 2, 1252},
    {0x12194c2d, L"KunstlerScript", L"Arial", 8, 1252},
    {0x135ef6a1, L"MinionProSmBd",
     L"Bell MT,Corbel,Times New Roman,Cambria,Berlin Sans FB", 0, 1252},
    {0x158c4049, L"Garamond", L"Arial", 2, 1252},
    {0x160ecb24, L"STZhongsong", L"Arial", 0, 936},
    {0x161ed07e, L"MSGothic", L"Arial", 1, 1252},
    {0x171d1ed1, L"SnapITC-Regular", L"Arial", 0, 1252},
    {0x18d1188f, L"Cambria", L"Arial", 2, 1252},
    {0x18eaf350, L"ArialUnicodeMS", L"Arial", 0, 936},
    {0x1a92d115, L"MingLiU", L"Arial", 1, 1252},
    {0x1cc217c6, L"TrebuchetMS", L"Arial", 0, 1252},
    {0x1d649596, L"BasemicTimes", L"Arial", 0, 1252},
    {0x1e34ee60, L"BellMT", L"Arial", 2, 1252},
    {0x1eb36945, L"CooperBlack", L"Arial", 2, 1252},
    {0x1ef7787d, L"BatangChe", L"Arial", 1, 1252},
    {0x20b3bd3a, L"BrushScriptMT", L"Arial", 8, 1252},
    {0x220877aa, L"Candara", L"Arial", 0, 1252},
    {0x22135007, L"FreestyleScript-Regular", L"Arial", 8, 1252},
    {0x251059c3, L"Chiller", L"Arial", 0, 1252},
    {0x25bed6dd, L"MSReferenceSansSerif", L"Arial", 0, 1252},
    {0x28154c81, L"Parchment-Regular", L"Arial", 8, 1252},
    {0x29711eb9, L"STLiti", L"Arial", 0, 936},
    {0x2b1993b4, L"Basemic", L"Arial", 0, 1252},
    {0x2b316339, L"NiagaraSolid-Reg", L"Arial", 0, 1252},
    {0x2c147529, L"FootlightMTLight", L"Arial", 0, 1252},
    {0x2c198928, L"HarlowSolid", L"Arial", 0, 1252},
    {0x2c6ac6b2, L"LucidaBright", L"Arial", 2, 1252},
    {0x2c9f38e2, L"KozukaMinchoPro-VIR", L"Arial", 0, 1252},
    {0x2d5a47b0, L"STCaiyun", L"Arial", 0, 936},
    {0x2def26bf, L"BernardMT-Condensed", L"Arial", 0, 1252},
    {0x2fd8930b, L"KozukaMinchoPr6NR", L"Arial", 0, 1252},
    {0x3115525a, L"FangSong_GB2312", L"Arial", 0, 1252},
    {0x31327817, L"MyriadPro",
     L"Calibri,Corbel,Candara,Cambria Math,Franklin Gothic Medium,Arial "
     L"Narrow,Times New Roman",
     0, 1252},
    {0x32244975, L"Helvetica", L"Arial", 0, 1252},
    {0x32ac995c, L"Terminal", L"Arial", 0, 1252},
    {0x338d648a, L"NiagaraEngraved-Reg", L"Arial", 0, 1252},
    {0x33bb65f2, L"Sylfaen", L"Arial", 2, 1252},
    {0x3402c30e, L"MSPMincho", L"Arial", 2, 1252},
    {0x3412bf31, L"SimSun-PUA", L"Arial", 0, 936},
    {0x36eb39b9, L"BerlinSansFB", L"Arial", 0, 1252},
    {0x36f42055, L"UniversATT", L"Microsoft Sans Serif", 0, 1252},
    {0x3864c4f6, L"HighTowerText", L"Arial", 2, 1252},
    {0x3a257d03, L"FangSong_GB2312", L"Arial", 0, 1252},
    {0x3cdae668, L"FreestyleScript", L"Arial", 8, 1252},
    {0x3d55aed7, L"Jokerman", L"Arial", 0, 1252},
    {0x3d5b4385, L"PMingLiU", L"Arial", 2, 1252},
    {0x3d9b7669, L"EstrangeloEdessa", L"Arial", 0, 1252},
    {0x3e532d74, L"FranklinGothicMedium", L"Arial", 0, 1252},
    {0x3e6aa32d, L"NSimSun", L"Arial", 1, 936},
    {0x3f6c36a8, L"Gautami", L"Arial", 0, 1252},
    {0x3ff32662, L"Chiller-Regular", L"Arial", 0, 1252},
    {0x409de312, L"ModernNo.20", L"Arial", 2, 1252},
    {0x41443c5e, L"Georgia", L"Arial", 2, 1252},
    {0x4160ade5, L"BellGothicStdBlack",
     L"Arial,Arial Unicode MS,Book Antiqua,Dotum,Georgia", 0, 1252},
    {0x421976c4, L"Modern-Regular", L"Arial", 2, 1252},
    {0x422a7252, L"Stencil", L"Arial", 0, 1252},
    {0x42c8554f, L"Fixedsys", L"Arial", 0, 1252},
    {0x435cb41d, L"Roman", L"Arial", 0, 1252},
    {0x47882383, L"CourierNew", L"Arial", 1, 1252},
    {0x480a2338, L"BerlinSansFBDemi", L"Arial", 0, 1252},
    {0x480bf7a4, L"CourierStd", L"Courier New,Verdana", 0, 1252},
    {0x481ad6ed, L"VladimirScript", L"Arial", 8, 1252},
    {0x4911577a, L"YouYuan", L"Arial", 1, 936},
    {0x4a788d72, L"STXingkai", L"Arial", 0, 936},
    {0x4bf88566, L"SegoeCondensed", L"Arial", 0, 1252},
    {0x4ccf51a4, L"BerlinSansFB-Reg", L"Arial", 0, 1252},
    {0x4ea967ce, L"GulimChe", L"Arial", 1, 1252},
    {0x4f68bd79, L"LetterGothicStd", L"Courier New,Verdana", 0, 1252},
    {0x51a0d0e6, L"KozukaGothicPr6NM", L"Arial", 0, 1252},
    {0x531b3dea, L"BasemicSymbol", L"Arial", 0, 1252},
    {0x5333fd39, L"CalifornianFB-Reg", L"Arial", 2, 1252},
    {0x53561a54, L"FZYTK--GBK1-0", L"Arial", 0, 936},
    {0x55e0dde6, L"LucidaSansTypewriter", L"Arial", 0, 1252},
    {0x574d4d3d, L"AdobeArabic", L"Arial Narrow", 0, 1252},
    {0x5792e759, L"STKaiti", L"Arial", 0, 936},
    {0x5921978e, L"LucidaSansUnicode", L"Arial", 0, 1252},
    {0x594e2da4, L"Vrinda", L"Arial", 0, 1252},
    {0x59baa9a2, L"KaiTi_GB2312", L"Arial", 0, 1252},
    {0x5cfedf4f, L"BaskOldFace", L"Arial", 0, 1252},
    {0x5f97921c, L"AdobeMyungjoStdM",
     L"Batang,Bookman Old Style,Consolas,STZhongsong", 0, 936},
    {0x5fefbfad, L"Batang", L"Arial", 2, 1252},
    {0x605342b9, L"DotumChe", L"Arial", 1, 1252},
    {0x608c5f9a, L"KaiTi_GB2312", L"Arial", 0, 936},
    {0x61efd0d1, L"MaturaMTScriptCapitals", L"Arial", 0, 1252},
    {0x626608a9, L"MVBoli", L"Arial", 0, 1252},
    {0x630501a3, L"SmallFonts", L"Arial", 0, 1252},
    {0x65d0e2a9, L"FZYTK--GBK1-0", L"Arial", 0, 936},
    {0x669f29e1, L"FZSTK--GBK1-0", L"Arial", 0, 936},
    {0x673a9e5f, L"Tunga", L"Arial", 0, 1252},
    {0x691aa4ce, L"NiagaraSolid", L"Arial", 0, 1252},
    {0x696259b7, L"Corbel", L"Arial", 0, 1252},
    {0x696ee9be, L"STXihei", L"Arial", 0, 936},
    {0x6c59cf69, L"Dotum", L"Arial", 0, 1252},
    {0x707fa561, L"Gungsuh", L"Arial", 2, 1252},
    {0x71416bb2, L"ZWAdobeF", L"Arial", 0, 1252},
    {0x71b41801, L"Verdana", L"Arial", 0, 1252},
    {0x73f25e4c, L"PalatinoLinotype", L"Arial", 0, 1252},
    {0x73f4d19f, L"NiagaraEngraved", L"Arial", 0, 1252},
    {0x74001694, L"MyriadProBlack", L"Book Antiqua,Constantia,Dotum,Georgia", 0,
     1252},
    {0x74b14d8f, L"Haettenschweiler", L"Arial", 0, 1252},
    {0x74cb44ee, L"NSimSun", L"Arial", 1, 936},
    {0x76b4d7ff, L"Shruti", L"Arial", 0, 1252},
    {0x788b3533, L"Webdings", L"Arial", 6, 42},
    {0x797dde99, L"MSSerif", L"Arial", 0, 1252},
    {0x7a0f9e9e, L"MSMincho", L"Arial", 1, 1252},
    {0x7b439caf, L"OldEnglishTextMT", L"Arial", 0, 1252},
    {0x8213a433, L"LucidaSans-Typewriter", L"Arial", 0, 1252},
    {0x82fec929, L"AdobeSongStdL",
     L"Centaur,Calibri,STSong,Bell MT,Garamond,Times New Roman", 0, 936},
    {0x83581825, L"Modern", L"Arial", 0, 1252},
    {0x835a2823, L"Algerian", L"Arial", 0, 1252},
    {0x83dab9f5, L"Script", L"Arial", 0, 1252},
    {0x847b56da, L"Tahoma", L"Arial", 0, 1252},
    {0x8a783cb2, L"SimSun-PUA", L"Arial", 0, 1252},
    {0x8b5cac0e, L"Onyx", L"Arial", 0, 1252},
    {0x8c6a499e, L"Gulim", L"Arial", 0, 1252},
    {0x8e0af790, L"JuiceITC", L"Arial", 0, 1252},
    {0x8e8d43b2, L"Centaur", L"Arial", 2, 1252},
    {0x8ee4dcca, L"BookshelfSymbol7", L"Arial", 0, 1252},
    {0x90794800, L"BellGothicStdLight", L"Bell MT,Calibri,Times New Roman", 0,
     1252},
    {0x909b516a, L"Century", L"Arial", 2, 1252},
    {0x92ae370d, L"MSOutlook", L"Arial", 4, 42},
    {0x93c9fbf1, L"LucidaFax", L"Arial", 2, 1252},
    {0x9565085e, L"BookAntiqua", L"Arial", 2, 1252},
    {0x9856d95d, L"AdobeMingStdL", L"Arial,Arial Unicode MS,Cambria,BatangChe",
     0, 949},
    {0x9bbadd6b, L"ColonnaMT", L"Arial", 0, 1252},
    {0x9cbd16a4, L"ShowcardGothic-Reg", L"Arial", 0, 1252},
    {0x9d73008e, L"MSSansSerif", L"Arial", 0, 1252},
    {0xa0607db1, L"GungsuhChe", L"Arial", 1, 1252},
    {0xa0bcf6a1, L"LatinWide", L"Arial", 2, 1252},
    {0xa1429b36, L"Symbol", L"Arial", 6, 42},
    {0xa1fa5abc, L"Wingdings2", L"Arial", 6, 42},
    {0xa1fa5abd, L"Wingdings3", L"Arial", 6, 42},
    {0xa427bad4, L"InformalRoman-Regular", L"Arial", 8, 1252},
    {0xa8b92ece, L"FZSTK--GBK1-0", L"Arial", 0, 936},
    {0xa8d83ece, L"CalifornianFB", L"Arial", 2, 1252},
    {0xaa3e082c, L"Kingsoft-Phonetic", L"Arial", 0, 1252},
    {0xaa6bcabe, L"HarlowSolidItalic", L"Arial", 0, 1252},
    {0xade5337c, L"MSUIGothic", L"Arial", 0, 1252},
    {0xb08dd941, L"WideLatin", L"Arial", 2, 1252},
    {0xb207f05d, L"PoorRichard", L"Arial", 2, 1252},
    {0xb3bc492f, L"JuiceITC-Regular", L"Arial", 0, 1252},
    {0xb5545399, L"Marlett", L"Arial", 4, 42},
    {0xb5dd1ebb, L"BritannicBold", L"Arial", 0, 1252},
    {0xb699c1c5, L"LucidaCalligraphy-Italic", L"Arial", 0, 1252},
    {0xb725d629, L"TimesNewRoman", L"Arial", 2, 1252},
    {0xb7eaebeb, L"AdobeHeitiStdR", L"Batang,Century,Dotum", 0, 936},
    {0xbd29c486, L"BerlinSansFBDemi-Bold", L"Arial", 0, 1252},
    {0xbe8a8db4, L"BookshelfSymbolSeven", L"Arial", 0, 1252},
    {0xc16c0118, L"AdobeHebrew", L"Bell MT,Berlin Sans FB,Calibri", 0, 1252},
    {0xc318b0af, L"MyriadProLight", L"Calibri,STFangsong,Times New Roman", 0,
     1252},
    {0xc65e5659, L"CambriaMath", L"Arial", 2, 1252},
    {0xc75c8f05, L"LucidaConsole", L"Arial", 1, 1252},
    {0xca7c35d6, L"Calibri", L"Arial", 0, 1252},
    {0xcb053f53, L"MicrosoftYaHei", L"Arial", 0, 936},
    {0xcb7190f9, L"Magneto-Bold", L"Arial", 0, 1252},
    {0xcca00cc5, L"System", L"Arial", 0, 1252},
    {0xccad6f76, L"Jokerman-Regular", L"Arial", 0, 1252},
    {0xccc5818c, L"EuroSign", L"Arial", 0, 1252},
    {0xcf3d7234, L"LucidaHandwriting-Italic", L"Arial", 0, 1252},
    {0xcf7b8fdb, L"MinionPro",
     L"Bell MT,Corbel,Times New Roman,Cambria,Berlin Sans FB", 0, 1252},
    {0xcfe5755f, L"Simhei", L"Arial", 1, 936},
    {0xd011f4ee, L"MSPGothic", L"Arial", 0, 1252},
    {0xd060e7ef, L"Vivaldi", L"Arial", 8, 1252},
    {0xd07edec1, L"FranklinGothic-Medium", L"Arial", 0, 1252},
    {0xd107243f, L"SimSun", L"Arial", 0, 936},
    {0xd1881562, L"ArialNarrow", L"Arial Narrow", 0, 1252},
    {0xd22b7dce, L"BodoniMTPosterCompressed", L"Arial", 0, 1252},
    {0xd22bfa60, L"ComicSansMS", L"Arial", 8, 1252},
    {0xd3bd0e35, L"Bauhaus93", L"Arial", 0, 1252},
    {0xd429ee7a, L"STFangsong", L"Arial", 0, 936},
    {0xd6679c12, L"BernardMTCondensed", L"Arial", 0, 1252},
    {0xd8e8a027, L"LucidaSans", L"Arial", 0, 1252},
    {0xd9fe7761, L"HighTowerText-Reg", L"Arial", 2, 1252},
    {0xda7e551e, L"STSong", L"Arial", 0, 936},
    {0xdaa6842d, L"STZhongsong", L"Arial", 0, 936},
    {0xdaaab93f, L"STFangsong", L"Arial", 0, 936},
    {0xdaeb0713, L"STSong", L"Arial", 0, 936},
    {0xdafedbef, L"STCaiyun", L"Arial", 0, 936},
    {0xdb00a3d9, L"Broadway", L"Arial", 0, 1252},
    {0xdb1f5ad4, L"STXinwei", L"Arial", 0, 936},
    {0xdb326e7f, L"STKaiti", L"Arial", 0, 936},
    {0xdb69595a, L"STHupo", L"Arial", 0, 936},
    {0xdba0082c, L"STXihei", L"Arial", 0, 936},
    {0xdbd0ab18, L"STXingkai", L"Arial", 0, 936},
    {0xdc1a7db1, L"STLiti", L"Arial", 0, 936},
    {0xdc33075f, L"KristenITC-Regular", L"Arial", 8, 1252},
    {0xdcc7009c, L"Harrington", L"Arial", 0, 1252},
    {0xdd712466, L"ArialBlack", L"Arial", 0, 1252},
    {0xdde87b3e, L"Impact", L"Arial", 0, 1252},
    {0xdf69fb32, L"SnapITC", L"Arial", 0, 1252},
    {0xdf8b25e8, L"CenturyGothic", L"Arial", 0, 1252},
    {0xe0f705c0, L"KristenITC", L"Arial", 8, 1252},
    {0xe1427573, L"Raavi", L"Arial", 0, 1252},
    {0xe2cea0cb, L"Magneto", L"Arial", 0, 1252},
    {0xe36a9e17, L"Ravie", L"Arial", 0, 1252},
    {0xe433f8e2, L"Parchment", L"Arial", 8, 1252},
    {0xe43dff4a, L"Wingdings", L"Arial", 4, 42},
    {0xe4e2c405, L"MTExtra", L"Arial", 6, 42},
    {0xe618cc35, L"InformalRoman", L"Arial", 8, 1252},
    {0xe6c27ffc, L"Mistral", L"Arial", 8, 1252},
    {0xe7ebf4b9, L"Courier", L"Courier New", 0, 1252},
    {0xe8bc4a9d, L"MSReferenceSpecialty", L"Arial", 0, 1252},
    {0xe90fb013, L"TempusSansITC", L"Arial", 0, 1252},
    {0xec637b42, L"Consolas", L"Verdana", 1, 1252},
    {0xed3a683b, L"STXinwei", L"Arial", 0, 936},
    {0xef264cd1, L"LucidaHandwriting", L"Arial", 0, 1252},
    {0xf086bca2, L"BaskervilleOldFace", L"Arial", 0, 1252},
    {0xf1028030, L"Mangal", L"Arial", 2, 1252},
    {0xf1da7eb9, L"ShowcardGothic", L"Arial", 0, 1252},
    {0xf210f06a, L"ArialMT", L"Arial", 0, 1252},
    {0xf477f16a, L"Latha", L"Arial", 0, 1252},
    {0xf616f3dd, L"LiSu", L"Arial", 1, 936},
    {0xfa479aa6, L"MicrosoftYaHei", L"Arial", 0, 936},
    {0xfcd19697, L"BookmanOldStyle", L"Arial", 0, 1252},
    {0xfe209a82, L"LucidaCalligraphy", L"Arial", 0, 1252},
    {0xfef135f8, L"AdobeHeitiStd-Regular", L"Batang,Century,Dotum", 0, 936},
};
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_
static const XFA_FONTINFO g_XFAFontsMap[] = {
    {0x01d5d33e, L"SimSun",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE",
     0, 936},
    {0x01e4f102, L"YouYuan",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE",
     1, 936},
    {0x030549dc, L"LiSu",
     L"WenQuanYi Zen Hei,WenQuanYi Zen Hei Sharp,WenQuanYi Zen Hei "
     L"Mono,WenQuanYi Micro Hei",
     1, 936},
    {0x032edd44, L"Simhei",
     L"WenQuanYi Zen Hei,WenQuanYi Zen Hei Sharp,WenQuanYi Zen Hei "
     L"Mono,WenQuanYi Micro Hei",
     1, 936},
    {0x03eac6fc, L"PoorRichard-Regular", L"Droid Sans Japanese,FreeSerif", 2,
     1252},
    {0x03ed90e6, L"Nina", L"FreeSerif", 0, 1252},
    {0x077b56b3, L"KingsoftPhoneticPlain",
     L"Tibetan Machine Uni,LKLUG,Samyak Gujarati,Droid Sans Thai,Droid Sans "
     L"Armenian,Untitled1,utkal,Lohit Oriya",
     0, 1252},
    {0x078ed524, L"MicrosoftSansSerif",
     L"Droid Sans Japanese,FreeSerif,WenQuanYi Micro Hei", 0, 1252},
    {0x089b18a9, L"Arial",
     L"Droid Sans Japanese,DejaVu Sans Condensed,FreeSerif,WenQuanYi Micro Hei",
     0, 1252},
    {0x0b2cad72, L"MonotypeCorsiva", L"Droid Sans Japanese,FreeSerif", 8, 1252},
    {0x0bb003e7, L"Kartika",
     L"FreeSans,Liberation Sans,Liberation Sans Narrow,Nimbus Sans "
     L"L,Garuda,FreeSerif,WenQuanYi Micro Hei",
     2, 1252},
    {0x0bb469df, L"VinerHandITC",
     L"Droid Sans Japanese,Ubuntu,Liberation Sans,Liberation Serif", 8, 1252},
    {0x0bc1a851, L"SegoeUI", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0x0c112ebd, L"KozukaGothicPro-VIM", L"FreeSerif", 0, 1252},
    {0x0cfcb9c1, L"AdobeThai", L"Droid Sans Japanese,Waree", 0, 847},
    {0x0e7de0f9, L"Playbill",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Ethiopic,Droid Sans Japanese,FreeSerif",
     0, 1252},
    {0x0eff47c3, L"STHupo", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0x107ad374, L"Constantia",
     L"Droid Sans Japanese,FreeSerif,WenQuanYi Micro Hei,Ubuntu", 2, 1252},
    {0x12194c2d, L"KunstlerScript", L"Droid Sans Japanese,Liberation Serif", 8,
     1252},
    {0x135ef6a1, L"MinionProSmBd", L"Liberation Serif", 0, 1252},
    {0x158c4049, L"Garamond",
     L"Droid Sans Japanese,Liberation Serif,Ubuntu,FreeSerif", 2, 1252},
    {0x160ecb24, L"STZhongsong",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x161ed07e, L"MSGothic",
     L"WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,AR PL "
     L"UMing CN,AR PL UMing HK,AR PL UMing TW",
     1, 1252},
    {0x171d1ed1, L"SnapITC-Regular",
     L"Liberation Sans Narrow,Ubuntu Condensed,Nimbus Sans L,DejaVu Sans", 0,
     1252},
    {0x18d1188f, L"Cambria", L"Droid Sans Japanese,FreeSerif,FreeMono", 2,
     1252},
    {0x18eaf350, L"ArialUnicodeMS",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x1a92d115, L"MingLiU",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     1, 1252},
    {0x1cc217c6, L"TrebuchetMS",
     L"Droid Sans Japanese,Liberation Serif,FreeSerif,Ubuntu", 0, 1252},
    {0x1d649596, L"BasemicTimes",
     L"Liberation Serif,Times New Roman,Droid Sans Japanese,FreeSerif,Ubuntu",
     0, 1252},
    {0x1e34ee60, L"BellMT",
     L"KacstQurn,Droid Sans Japanese,Ubuntu,Liberation Serif", 2, 1252},
    {0x1eb36945, L"CooperBlack",
     L"KacstQurn,Droid Sans Japanese,FreeMono,Liberation Mono, WenQuanYi Micro "
     L"Hei Mono",
     2, 1252},
    {0x1ef7787d, L"BatangChe",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing "
     L"TW,WenQuanYi Zen Hei,WenQuanYi Micro Hei",
     1, 1252},
    {0x20b3bd3a, L"BrushScriptMT",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,Droid Sans "
     L"Japanese,URW Chancery L,Liberation Sans",
     8, 1252},
    {0x220877aa, L"Candara", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0x22135007, L"FreestyleScript-Regular",
     L"KacstQurn,Droid Sans Japanese,Liberation Sans", 8, 1252},
    {0x251059c3, L"Chiller",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,Droid Sans "
     L"Japanese,Liberation Sans",
     0, 1252},
    {0x25bed6dd, L"MSReferenceSansSerif",
     L"DejaVu Sans Condensed,Ubuntu Condensed,Droid Sans Japanese,AR PL UKai "
     L"HK",
     0, 1252},
    {0x28154c81, L"Parchment-Regular", L"Droid Sans Japanese,Liberation Sans",
     8, 1252},
    {0x29711eb9, L"STLiti", L"AR PL UKai HK", 0, 936},
    {0x2b1993b4, L"Basemic",
     L"Liberation Serif,Droid Sans Japanese,Liberation Sans", 0, 1252},
    {0x2b316339, L"NiagaraSolid-Reg", L"Droid Sans Japanese,Liberation Sans", 0,
     1252},
    {0x2c147529, L"FootlightMTLight",
     L"KacstQurn,Droid Sans Japanese,Liberation Sans", 0, 1252},
    {0x2c198928, L"HarlowSolid",
     L"KacstQurn,Droid Sans Japanese,Liberation Sans", 0, 1252},
    {0x2c6ac6b2, L"LucidaBright",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Japanese,Liberation Sans",
     2, 1252},
    {0x2c9f38e2, L"KozukaMinchoPro-VIR", L"DejaVu Sans", 0, 1252},
    {0x2d5a47b0, L"STCaiyun", L"AR PL UKai HK", 0, 936},
    {0x2def26bf, L"BernardMT-Condensed",
     L"KacstQurn,Droid Sans Japanese,DejaVu Serif", 0, 1252},
    {0x2fd8930b, L"KozukaMinchoPr6NR", L"DejaVu Serif", 0, 1252},
    {0x3115525a, L"FangSong_GB2312",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 1252},
    {0x31327817, L"MyriadPro",
     L"Ubuntu Condensed,Droid Sans Japanese, FreeSerif", 0, 1252},
    {0x32244975, L"Helvetica",
     L"Ubuntu,DejaVu Sans Condensed,Liberation Sans,Liberation Sans "
     L"Narrow,Nimbus Sans L",
     0, 1252},
    {0x32ac995c, L"Terminal", L"DejaVu Serif", 0, 1252},
    {0x338d648a, L"NiagaraEngraved-Reg", L"Droid Sans Japanese,DejaVu Serif", 0,
     1252},
    {0x33bb65f2, L"Sylfaen", L"Droid Sans Japanese,DejaVu Sans", 2, 1252},
    {0x3402c30e, L"MSPMincho",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW", 2,
     1252},
    {0x3412bf31, L"SimSun-PUA",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing CN,AR PL UMing HK", 0,
     936},
    {0x36eb39b9, L"BerlinSansFB",
     L"Droid Sans Japanese,Liberation Serif,Ubuntu,FreeSerif", 0, 1252},
    {0x36f42055, L"UniversATT", L"Microsoft Sans Serif", 0, 1252},
    {0x3864c4f6, L"HighTowerText", L"Droid Sans Japanese,DejaVu Serif", 2,
     1252},
    {0x3a257d03, L"FangSong_GB2312",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei", 0, 1252},
    {0x3c7d1d07, L"Garamond3LTStd",
     L"Droid Sans Japanese,Ubuntu Condensed,DejaVu Sans Condensed,Liberation "
     L"Serif,Ubuntu,FreeSerif",
     2, 1252},
    {0x3cdae668, L"FreestyleScript",
     L"KacstQurn,Droid Sans Japanese,DejaVu Sans", 8, 1252},
    {0x3d55aed7, L"Jokerman", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0x3d5b4385, L"PMingLiU",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     2, 1252},
    {0x3d9b7669, L"EstrangeloEdessa", L"Droid Sans Japanese,DejaVu Sans", 0,
     1252},
    {0x3e532d74, L"FranklinGothicMedium", L"Droid Sans Japanese,Ubuntu", 0,
     1252},
    {0x3e6aa32d, L"NSimSun",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     1, 936},
    {0x3f6c36a8, L"Gautami",
     L"Droid Arabic Naskh,Droid Sans Ethiopic, mry_KacstQurn,Droid Sans "
     L"Japanese,FreeSans",
     0, 1252},
    {0x3ff32662, L"Chiller-Regular",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,FreeSans", 0, 1252},
    {0x409de312, L"ModernNo.20",
     L"KacstQurn,Droid Sans Japanese,Nimbus Sans L,Nimbus Sans L,FreeSans", 2,
     1252},
    {0x41443c5e, L"Georgia", L"Droid Sans Japanese,FreeSans", 2, 1252},
    {0x4160ade5, L"BellGothicStdBlack", L"FreeSans", 0, 1252},
    {0x421976c4, L"Modern-Regular", L"FreeSans", 2, 1252},
    {0x422a7252, L"Stencil", L"Droid Sans Japanese,FreeSans,Liberation Sans", 0,
     1252},
    {0x42c8554f, L"Fixedsys", L"FreeSerif", 0, 1252},
    {0x435cb41d, L"Roman", L"FreeSerif", 0, 1252},
    {0x47882383, L"CourierNew",
     L"FreeMono,WenQuanYi Micro Hei Mono,AR PL UKai CN,AR PL UKai HK,AR PL "
     L"UKai TW,AR PL UKai TW MBE,DejaVu Sans",
     1, 1252},
    {0x480a2338, L"BerlinSansFBDemi", L"Droid Sans Japanese,Liberation Serif",
     0, 1252},
    {0x480bf7a4, L"CourierStd", L"DejaVu Sans", 0, 1252},
    {0x481ad6ed, L"VladimirScript", L"Droid Sans Japanese,DejaVu Serif", 8,
     1252},
    {0x4911577a, L"YouYuan",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW", 1,
     936},
    {0x4a788d72, L"STXingkai", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0x4bf88566, L"SegoeCondensed", L"FreeSerif", 0, 1252},
    {0x4ccf51a4, L"BerlinSansFB-Reg", L"Droid Sans Japanese,Liberation Serif",
     0, 1252},
    {0x4ea967ce, L"GulimChe",
     L"WenQuanYi Zen Hei Mono,AR PL UKai CN,AR PL UKai HK,AR PL UKai TW,AR PL "
     L"UKai TW MBE",
     1, 1252},
    {0x4f68bd79, L"LetterGothicStd",
     L"FreeMono,Liberation Mono,Andale Mono,WenQuanYi Micro Hei Mono", 0, 1252},
    {0x51a0d0e6, L"KozukaGothicPr6NM", L"FreeSerif", 0, 1252},
    {0x531b3dea, L"BasemicSymbol", L"FreeSerif", 0, 1252},
    {0x5333fd39, L"CalifornianFB-Reg",
     L"Droid Sans Japanese,URW Chancery L,FreeSerif", 2, 1252},
    {0x53561a54, L"FZYTK--GBK1-0",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x55e0dde6, L"LucidaSansTypewriter",
     L"Ubuntu Mono,DejaVu Sans Mono,Nimbus Mono L,Liberation Mono,Courier 10 "
     L"Pitch,FreeMono",
     0, 1252},
    {0x574d4d3d, L"AdobeArabic", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0x5792e759, L"STKaiti", L"WenQuanYi Micro Hei Mono", 0, 936},
    {0x5921978e, L"LucidaSansUnicode", L"Droid Sans Japanese,DejaVu Sans", 0,
     1252},
    {0x594e2da4, L"Vrinda",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Arabic "
     L"Naskh,mry_KacstQurn,Droid Sans Japanese,FreeSans,FreeSerif",
     0, 1252},
    {0x59baa9a2, L"KaiTi_GB2312",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 1252},
    {0x5cfedf4f, L"BaskOldFace",
     L"KacstQurn,Droid Sans Japanese,Ubuntu,Liberation Serif", 0, 1252},
    {0x5e16ac91, L"TrajanPro",
     L"Nimbus Sans L,AR PL UMing HK,AR PL UKai HK,AR PL UMing TW,AR PL UMing "
     L"TW MBE,DejaVu Sans,DejaVu Serif",
     0, 1252},
    {0x5f388196, L"ITCLegacySansStdMedium",
     L"Liberation Serif,FreeSerif,FreeSans,Ubuntu", 0, 1252},
    {0x5f97921c, L"AdobeMyungjoStdM",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x5fefbfad, L"Batang",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     2, 1252},
    {0x605342b9, L"DotumChe",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW", 1,
     1252},
    {0x608c5f9a, L"KaiTi_GB2312",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x61efd0d1, L"MaturaMTScriptCapitals",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Japanese,DejaVu Serif,DejaVu Sans",
     0, 1252},
    {0x626608a9, L"MVBoli",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Ethiopic,Droid Sans Japanese,DejaVu Sans",
     0, 1252},
    {0x630501a3, L"SmallFonts", L"DejaVu Serif", 0, 1252},
    {0x65d0e2a9, L"FZYTK--GBK1-0",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x669f29e1, L"FZSTK--GBK1-0",
     L"AR PL UMing CN,AR PL UKai CN, AR PL UMing HK", 0, 936},
    {0x673a9e5f, L"Tunga",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Japanese,DejaVu Serif",
     0, 1252},
    {0x691aa4ce, L"NiagaraSolid", L"Droid Sans Japanese,DejaVu Serif", 0, 1252},
    {0x696259b7, L"Corbel", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0x696ee9be, L"STXihei", L"WenQuanYi Micro Hei Mono", 0, 936},
    {0x6c59cf69, L"Dotum", L"WenQuanYi Zen Hei Mono", 0, 1252},
    {0x707fa561, L"Gungsuh", L"WenQuanYi Zen Hei Mono", 2, 1252},
    {0x71416bb2, L"ZWAdobeF",
     L"KacstArt,KacstBookm,KacstDecorative,KacstDigital,KacstFarsi,KacstLetter,"
     L"KacstOffice,Dingbats,FreeSerif",
     0, 1252},
    {0x71b41801, L"Verdana",
     L"DejaVu Sans Condensed,Ubuntu Condensed,Droid Sans Japanese,DejaVu Sans",
     0, 1252},
    {0x73f25e4c, L"PalatinoLinotype", L"Droid Sans Japanese,FreeSerif", 0,
     1252},
    {0x73f4d19f, L"NiagaraEngraved", L"Droid Sans Japanese,FreeSerif", 0, 1252},
    {0x74001694, L"MyriadProBlack", L"Droid Sans Japanese,AR PL UKai HK", 0,
     1252},
    {0x74b14d8f, L"Haettenschweiler", L"Droid Sans Japanese,DejaVu Serif", 0,
     1252},
    {0x74cb44ee, L"NSimSun", L"WenQuanYi Zen Hei Mono", 1, 936},
    {0x76b4d7ff, L"Shruti",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Japanese,FreeSans",
     0, 1252},
    {0x788b3533, L"Webdings", L"FreeSans", 6, 42},
    {0x797dde99, L"MSSerif", L"FreeSans", 0, 1252},
    {0x7a0f9e9e, L"MSMincho",
     L"WenQuanYi Micro Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW",
     1, 1252},
    {0x7b439caf, L"OldEnglishTextMT",
     L"Droid Sans Japanese,Liberation Sans,Ubuntu", 0, 1252},
    {0x8213a433, L"LucidaSans-Typewriter",
     L"Ubuntu Mono,Liberation Mono,DejaVu Sans Mono", 0, 1252},
    {0x82fec929, L"AdobeSongStdL",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0x83581825, L"Modern", L"FreeSans", 0, 1252},
    {0x835a2823, L"Algerian",
     L"KacstQurn,Droid Sans Japanese,FreeSans,Liberation Sans,Ubuntu", 0, 1252},
    {0x83dab9f5, L"Script", L"FreeSans", 0, 1252},
    {0x847b56da, L"Tahoma",
     L"Droid Sans Japanese,DejaVu Sans Condensed,FreeSerif", 0, 1252},
    {0x8a783cb2, L"SimSun-PUA",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 1252},
    {0x8b5cac0e, L"Onyx", L"Droid Sans Japanese,Liberation Sans", 0, 1252},
    {0x8c6a499e, L"Gulim",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 1252},
    {0x8e0af790, L"JuiceITC", L"Droid Sans Japanese,Liberation Sans", 0, 1252},
    {0x8e8d43b2, L"Centaur",
     L"KacstQurn,Droid Sans Japanese,Khmer OS,Khmer OS System", 2, 1252},
    {0x8ee4dcca, L"BookshelfSymbol7", L"Liberation Sans", 0, 1252},
    {0x90794800, L"BellGothicStdLight", L"Liberation Sans", 0, 1252},
    {0x909b516a, L"Century",
     L"Droid Sans Japanese,Liberation Sans,Liberation Mono,Liberation Serif", 2,
     1252},
    {0x92ae370d, L"MSOutlook", L"Liberation Sans", 4, 42},
    {0x93c9fbf1, L"LucidaFax",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans "
     L"Ethiopic,mry_KacstQurn,Liberation Sans",
     2, 1252},
    {0x9565085e, L"BookAntiqua",
     L"Droid Sans Japanese,Liberation Sans,Liberation Serif", 2, 1252},
    {0x9856d95d, L"AdobeMingStdL", L"AR PL UMing HK", 0, 949},
    {0x9bbadd6b, L"ColonnaMT",
     L"KacstQurn,Droid Sans Japanese,Khmer OS,Khmer OS System", 0, 1252},
    {0x9cbd16a4, L"ShowcardGothic-Reg",
     L"Droid Sans Japanese,Liberation Sans,Ubuntu", 0, 1252},
    {0x9d73008e, L"MSSansSerif", L"FreeSerif", 0, 1252},
    {0xa0607db1, L"GungsuhChe",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     1, 1252},
    {0xa0bcf6a1, L"LatinWide", L"FreeSerif", 2, 1252},
    {0xa1429b36, L"Symbol", L"FreeSerif", 6, 42},
    {0xa1fa5abc, L"Wingdings2", L"FreeSerif", 6, 42},
    {0xa1fa5abd, L"Wingdings3", L"FreeSerif", 6, 42},
    {0xa427bad4, L"InformalRoman-Regular",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Japanese,FreeSerif",
     8, 1252},
    {0xa8b92ece, L"FZSTK--GBK1-0", L"AR PL UMing CN", 0, 936},
    {0xa8d83ece, L"CalifornianFB", L"Droid Sans Japanese,FreeSerif", 2, 1252},
    {0xaa3e082c, L"Kingsoft-Phonetic",
     L"Tibetan Machine Uni,LKLUG,Samyak Gujarati,Droid Sans "
     L"Thai,utkal,Kedage,Mallige,AR PL UKai CN",
     0, 1252},
    {0xaa6bcabe, L"HarlowSolidItalic",
     L"KacstQurn,Droid Sans Japanese,Liberation Serif", 0, 1252},
    {0xade5337c, L"MSUIGothic",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 1252},
    {0xb08dd941, L"WideLatin",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Japanese,Liberation Serif",
     2, 1252},
    {0xb12765e0, L"ITCLegacySansStdBook",
     L"AR PL UMing HK,AR PL UKai HK,FreeSerif,Ubuntu,FreeSans", 0, 1252},
    {0xb207f05d, L"PoorRichard", L"Droid Sans Japanese,Liberation Serif", 2,
     1252},
    {0xb3bc492f, L"JuiceITC-Regular", L"Droid Sans Japanese,Liberation Serif",
     0, 1252},
    {0xb5545399, L"Marlett", L"Liberation Serif", 4, 42},
    {0xb5dd1ebb, L"BritannicBold",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans "
     L"Ethiopic,mry_KacstQurn,Liberation Serif",
     0, 1252},
    {0xb699c1c5, L"LucidaCalligraphy-Italic",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Japanese,DejaVu Serif",
     0, 1252},
    {0xb725d629, L"TimesNewRoman", L"Droid Sans Japanese,Liberation Sans", 2,
     1252},
    {0xb7eaebeb, L"AdobeHeitiStdR",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0xbd29c486, L"BerlinSansFBDemi-Bold", L"Droid Sans Japanese,DejaVu Serif",
     0, 1252},
    {0xbe8a8db4, L"BookshelfSymbolSeven", L"DejaVu Sans", 0, 1252},
    {0xc16c0118, L"AdobeHebrew", L"Droid Sans Japanese,Ubuntu,Liberation Serif",
     0, 1252},
    {0xc318b0af, L"MyriadProLight",
     L"Droid Sans Japanese,AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     1252},
    {0xc65e5659, L"CambriaMath", L"Droid Sans Japanese,FreeSerif,FreeMono", 2,
     1252},
    {0xc75c8f05, L"LucidaConsole",
     L"Liberation Mono,DejaVu Sans Mono,FreeMono,WenQuanYi Micro Hei Mono", 1,
     1252},
    {0xca7c35d6, L"Calibri", L"Droid Sans Japanese,DejaVu Sans", 0, 1252},
    {0xcb053f53, L"MicrosoftYaHei",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0xcb7190f9, L"Magneto-Bold",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Japanese,DejaVu Serif",
     0, 1252},
    {0xcca00cc5, L"System", L"DejaVu Sans", 0, 1252},
    {0xccad6f76, L"Jokerman-Regular", L"Droid Sans Japanese,DejaVu Sans", 0,
     1252},
    {0xccc5818c, L"EuroSign", L"DejaVu Serif", 0, 1252},
    {0xcf3d7234, L"LucidaHandwriting-Italic",
     L"Liberation Sans Narrow,Ubuntu Condensed,Nimbus Sans L,DejaVu Serif", 0,
     1252},
    {0xcf7b8fdb, L"MinionPro", L"DejaVu Sans", 0, 1252},
    {0xcfe5755f, L"Simhei",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     1, 936},
    {0xd011f4ee, L"MSPGothic",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW", 0,
     1252},
    {0xd060e7ef, L"Vivaldi",
     L"KacstQurn,Droid Sans Japanese,Liberation Sans,Ubuntu", 8, 1252},
    {0xd07edec1, L"FranklinGothic-Medium", L"Droid Sans Japanese,Ubuntu", 0,
     1252},
    {0xd107243f, L"SimSun", L"WenQuanYi Zen Hei Mono", 0, 936},
    {0xd1881562, L"ArialNarrow",
     L"Liberation Sans Narrow,Droid Sans Japanese,FreeSerif", 0, 1252},
    {0xd22b7dce, L"BodoniMTPosterCompressed",
     L"Droid Sans Japanese,DejaVu Serif", 0, 1252},
    {0xd22bfa60, L"ComicSansMS",
     L"Droid Sans Japanese,FreeMono,Liberation Mono", 8, 1252},
    {0xd3bd0e35, L"Bauhaus93",
     L"KacstQurn,Droid Sans Japanese,Liberation Sans,Ubuntu", 0, 1252},
    {0xd429ee7a, L"STFangsong", L"WenQuanYi Micro Hei Mono", 0, 936},
    {0xd6679c12, L"BernardMTCondensed",
     L"KacstQurn,Droid Sans Japanese,Nimbus Sans L,URW Chancery "
     L"L,KacstOne,Liberation Sans",
     0, 1252},
    {0xd8e8a027, L"LucidaSans",
     L"Liberation Sans Narrow,Nimbus Sans L,KacstQurn,Droid Arabic Naskh,Droid "
     L"Sans Ethiopic,DejaVu Serif Condensed,Liberation Mono,Ubuntu",
     0, 1252},
    {0xd9fe7761, L"HighTowerText-Reg",
     L"Droid Sans Japanese,Ubuntu,Liberation Serif", 2, 1252},
    {0xda7e551e, L"STSong", L"WenQuanYi Micro Hei Mono", 0, 936},
    {0xdaa6842d, L"STZhongsong",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0xdaaab93f, L"STFangsong",
     L"WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen "
     L"Hei,WenQuanYi Zen Hei Sharp",
     0, 936},
    {0xdaeb0713, L"STSong",
     L"WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen "
     L"Hei,WenQuanYi Zen Hei Sharp",
     0, 936},
    {0xdafedbef, L"STCaiyun", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0xdb00a3d9, L"Broadway",
     L"KacstQurn,Droid Sans Japanese,DejaVu Sans,FreeMono,Liberation Mono", 0,
     1252},
    {0xdb1f5ad4, L"STXinwei", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0xdb326e7f, L"STKaiti",
     L"WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen "
     L"Hei,WenQuanYi Zen Hei Sharp",
     0, 936},
    {0xdb69595a, L"STHupo",
     L"WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen "
     L"Hei,WenQuanYi Zen Hei Sharp",
     0, 936},
    {0xdba0082c, L"STXihei",
     L" WenQuanYi Micro Hei Mono,WenQuanYi Zen Hei Mono,WenQuanYi Zen "
     L"Hei,WenQuanYi Zen Hei Sharp",
     0, 936},
    {0xdbd0ab18, L"STXingkai", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0xdc1a7db1, L"STLiti", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0xdc33075f, L"KristenITC-Regular",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,DejaVu Sans "
     L"Condensed,Ubuntu,Liberation Sans",
     8, 1252},
    {0xdcc7009c, L"Harrington",
     L"KacstQurn,Droid Sans Japanese,Liberation Serif,FreeSerif,Ubuntu", 0,
     1252},
    {0xdd712466, L"ArialBlack",
     L"Droid Sans Japanese,DejaVu Sans,DejaVu Serif,FreeMono", 0, 1252},
    {0xdde87b3e, L"Impact", L"Droid Sans Japanese,DejaVu Serif", 0, 1252},
    {0xdf69fb32, L"SnapITC",
     L"Liberation Sans Narrow,Ubuntu Condensed,DejaVu Sans,DejaVu "
     L"Serif,FreeMono",
     0, 1252},
    {0xdf8b25e8, L"CenturyGothic",
     L"Droid Sans Japanese,Liberation Mono,Liberation Sans,Liberation Serif", 0,
     1252},
    {0xe0f705c0, L"KristenITC",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,DejaVu Sans "
     L"Condensed,Ubuntu,Liberation Sans",
     8, 1252},
    {0xe1427573, L"Raavi",
     L"Droid Arabic Naskh,Droid Sans "
     L"Ethiopic,mry_KacstQurn,FreeSerif,Liberation Serif,Khmer OS",
     0, 1252},
    {0xe2cea0cb, L"Magneto",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,DejaVu "
     L"Serif,DejaVu Serif Condensed,DejaVu Sans",
     0, 1252},
    {0xe36a9e17, L"Ravie",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,DejaVu "
     L"Serif,DejaVu Sans,FreeMono",
     0, 1252},
    {0xe433f8e2, L"Parchment", L"Droid Sans Japanese,DejaVu Serif", 8, 1252},
    {0xe43dff4a, L"Wingdings", L"DejaVu Serif", 4, 42},
    {0xe4e2c405, L"MTExtra", L"DejaVu Serif", 6, 42},
    {0xe618cc35, L"InformalRoman",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid Sans "
     L"Japanese,Nimbus Sans L,DejaVu Sans Condensed,Ubuntu,Liberation Sans",
     8, 1252},
    {0xe6c27ffc, L"Mistral", L"Droid Sans Japanese,DejaVu Serif", 8, 1252},
    {0xe7ebf4b9, L"Courier", L"DejaVu Sans,DejaVu Sans Condensed,FreeSerif", 0,
     1252},
    {0xe8bc4a9d, L"MSReferenceSpecialty", L"DejaVu Serif", 0, 1252},
    {0xe90fb013, L"TempusSansITC",
     L"Droid Sans Japanese,Ubuntu,Liberation Serif,FreeSerif", 0, 1252},
    {0xec637b42, L"Consolas",
     L"DejaVu Sans Condensed,AR PL UKai CN,AR PL UKai HK,AR PL UKai "
     L"TW,FreeSerif,FreeSans",
     1, 1252},
    {0xed3a683b, L"STXinwei", L"AR PL UKai HK,AR PL UMing HK,AR PL UKai CN", 0,
     936},
    {0xef264cd1, L"LucidaHandwriting",
     L"Liberation Sans Narrow,Ubuntu Condensed,Nimbus Sans "
     L"L,KacstQurn,Liberation Mono",
     0, 1252},
    {0xf086bca2, L"BaskervilleOldFace",
     L"KacstQurn,Droid Sans Japanese,Liberation Serif,Ubuntu,FreeSerif", 0,
     1252},
    {0xf1028030, L"Mangal",
     L"FreeSans,TSCu_Paranar,Garuda,Liberation Sans,Liberation Sans "
     L"Narrow,Nimbus Sans L",
     2, 1252},
    {0xf1da7eb9, L"ShowcardGothic",
     L"Droid Sans Japanese,DejaVu Serif Condensed,DejaVu Sans "
     L"Condensed,Liberation Sans,Ubuntu",
     0, 1252},
    {0xf210f06a, L"ArialMT",
     L"Liberation Sans,Liberation Sans Narrow,FreeSans,Nimbus Sans L,Khmer OS "
     L"System,Khmer OS",
     0, 1252},
    {0xf477f16a, L"Latha",
     L"Liberation Sans Narrow,Nimbus Sans L,Droid Arabic "
     L"Naskh,mry_KacstQurn,FreeSerif,Nimbus Sans L",
     0, 1252},
    {0xf616f3dd, L"LiSu",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE",
     1, 936},
    {0xfa479aa6, L"MicrosoftYaHei",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
    {0xfcd19697, L"BookmanOldStyle",
     L"Droid Sans Japanese,Liberation Mono,Liberation Sans,Liberation Serif", 0,
     1252},
    {0xfe209a82, L"LucidaCalligraphy",
     L"KacstQurn,Droid Arabic Naskh,Droid Sans Ethiopic,mry_KacstQurn,Droid "
     L"Sans Japanese,DejaVu Serif,DejaVu Sans,FreeMono",
     0, 1252},
    {0xfef135f8, L"AdobeHeitiStd-Regular",
     L"WenQuanYi Zen Hei Mono,WenQuanYi Zen Hei,WenQuanYi Zen Hei "
     L"Sharp,WenQuanYi Micro Hei",
     0, 936},
};
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
static const XFA_FONTINFO g_XFAFontsMap[] = {
    {0x01d5d33e, L"SimSun", L"STHeiti,Heiti TC,STFangsong", 0, 936},
    {0x01e4f102, L"YouYuan", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0x030549dc, L"LiSu", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0x032edd44, L"Simhei", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0x03eac6fc, L"PoorRichard-Regular",
     L"Noteworthy,Avenir Next Condensed,Impact", 2, 1252},
    {0x03ed90e6, L"Nina", L"Microsoft Sans Serif", 0, 1252},
    {0x077b56b3, L"KingsoftPhoneticPlain",
     L"LastResort,Apple "
     L"Chancery,STIXVariants,STIXSizeOneSym,STIXSizeOneSym,Apple Braille",
     0, 1252},
    {0x078ed524, L"MicrosoftSansSerif", L"Songti SC,Apple Symbols", 0, 1252},
    {0x089b18a9, L"Arial",
     L"Arial Unicode MS,Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x0b2cad72, L"MonotypeCorsiva", L"Arial Narrow,Impact", 8, 1252},
    {0x0bb003e7, L"Kartika",
     L"Arial Unicode MS,Microsoft Sans Serif,Arial Narrow,Damascus", 2, 1252},
    {0x0bb469df, L"VinerHandITC", L"Comic Sans MS,Songti SC,STSong", 8, 1252},
    {0x0bc1a851, L"SegoeUI", L"Apple Symbols", 0, 1252},
    {0x0c112ebd, L"KozukaGothicPro-VIM", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0x0cfcb9c1, L"AdobeThai", L"Avenir Next Condensed Ultra Light", 0, 847},
    {0x0e7de0f9, L"Playbill", L"STIXNonUnicode", 0, 1252},
    {0x0eff47c3, L"STHupo", L"Kaiti SC,Songti SC,STHeiti", 0, 936},
    {0x107ad374, L"Constantia", L"Arial Unicode MS,Palatino,Baskerville", 2,
     1252},
    {0x12194c2d, L"KunstlerScript",
     L"Avenir Next Condensed Demi Bold,Arial Narrow", 8, 1252},
    {0x135ef6a1, L"MinionProSmBd", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x158c4049, L"Garamond", L"Impact,Arial Narrow", 2, 1252},
    {0x160ecb24, L"STZhongsong", L"STFangsong,Songti SC", 0, 936},
    {0x161ed07e, L"MSGothic",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing "
     L"TW,Microsoft Sans Serif,Apple Symbols",
     1, 1252},
    {0x171d1ed1, L"SnapITC-Regular", L"STHeiti,Arial Black", 0, 1252},
    {0x18d1188f, L"Cambria", L"Arial Unicode MS", 2, 1252},
    {0x18eaf350, L"ArialUnicodeMS", L"Microsoft Sans Serif,Apple Symbols", 0,
     936},
    {0x1a92d115, L"MingLiU", L"Heiti SC,STHeiti", 1, 1252},
    {0x1cc217c6, L"TrebuchetMS", L"Damascus,Impact,Arial Narrow", 0, 1252},
    {0x1d649596, L"BasemicTimes", L"Liberation Serif,Impact,Arial Narrow", 0,
     1252},
    {0x1e34ee60, L"BellMT",
     L"Papyrus,STIXNonUnicode,Microsoft Sans Serif,Avenir Light", 2, 1252},
    {0x1eb36945, L"CooperBlack",
     L"Marion,STIXNonUnicode,Arial Rounded MT Bold,Lucida Grande", 2, 1252},
    {0x1ef7787d, L"BatangChe",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE,Arial Unicode MS,Heiti TC",
     1, 1252},
    {0x20b3bd3a, L"BrushScriptMT",
     L"STIXNonUnicode,Damascus,Arial Narrow,Avenir Next Condensed,Cochin", 8,
     1252},
    {0x220877aa, L"Candara", L"Cochin,Baskerville,Marion", 0, 1252},
    {0x22135007, L"FreestyleScript-Regular",
     L"STIXNonUnicode,Nadeem,Zapf Dingbats", 8, 1252},
    {0x251059c3, L"Chiller",
     L"Zapf Dingbats,Damascus,STIXNonUnicode,Papyrus,KufiStandardGK,Baghdad", 0,
     1252},
    {0x25bed6dd, L"MSReferenceSansSerif",
     L"Tahoma,Apple Symbols,Apple LiGothic,Arial Unicode MS,Lucida "
     L"Grande,Microsoft Sans Serif",
     0, 1252},
    {0x28154c81, L"Parchment-Regular", L"Microsoft Sans Serif,Apple Symbols", 8,
     1252},
    {0x29711eb9, L"STLiti", L"Kaiti SC,Songti SC", 0, 936},
    {0x2b1993b4, L"Basemic", L"Impact,Arial Narrow", 0, 1252},
    {0x2b316339, L"NiagaraSolid-Reg", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x2c147529, L"FootlightMTLight",
     L"STIXNonUnicode,Avenir Next Condensed Heavy,PT Sans,Noteworthy", 0, 1252},
    {0x2c198928, L"HarlowSolid",
     L"Avenir Medium,Avenir Next Medium,Arial Unicode MS", 0, 1252},
    {0x2c6ac6b2, L"LucidaBright",
     L"PT Sans Narrow,Papyrus,Damascus,STIXNonUnicode,Arial Rounded MT "
     L"Bold,Comic Sans MS,Avenir Next",
     2, 1252},
    {0x2c9f38e2, L"KozukaMinchoPro-VIR", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0x2d5a47b0, L"STCaiyun", L"Kaiti SC,Songti SC", 0, 936},
    {0x2def26bf, L"BernardMT-Condensed",
     L"Impact,Avenir Next Condensed Demi Bold,American Typewriter", 0, 1252},
    {0x2fd8930b, L"KozukaMinchoPr6NR", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x3115525a, L"FangSong_GB2312", L"Hiragino Sans GB,STHeiti", 0, 1252},
    {0x31327817, L"MyriadPro", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x32244975, L"Helvetica",
     L"Arial Narrow,Arial Unicode MS,Damascus,STIXNonUnicode", 0, 1252},
    {0x32ac995c, L"Terminal", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x338d648a, L"NiagaraEngraved-Reg", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0x33bb65f2, L"Sylfaen", L"Arial Unicode MS,Marion", 2, 1252},
    {0x3402c30e, L"MSPMincho", L"Arial Unicode MS,Apple SD Gothic Neo", 2,
     1252},
    {0x3412bf31, L"SimSun-PUA", L"STHeiti,Heiti TC,STFangsong", 0, 936},
    {0x36eb39b9, L"BerlinSansFB", L"American Typewriter,Impact", 0, 1252},
    {0x36f42055, L"UniversATT", L"Microsoft Sans Serif", 0, 1252},
    {0x3864c4f6, L"HighTowerText", L"STIXGeneral,.Helvetica Neue Desk UI", 2,
     1252},
    {0x3a257d03, L"FangSong_GB2312", L"Hiragino Sans GB,STHeiti", 0, 1252},
    {0x3cdae668, L"FreestyleScript", L"Nadeem,Zapf Dingbats,STIXNonUnicode", 8,
     1252},
    {0x3d55aed7, L"Jokerman",
     L"Papyrus,Lucida Grande,Heiti TC,American Typewriter", 0, 1252},
    {0x3d5b4385, L"PMingLiU", L"Heiti SC,STHeiti", 2, 1252},
    {0x3d9b7669, L"EstrangeloEdessa", L"American Typewriter,Marion", 0, 1252},
    {0x3e532d74, L"FranklinGothicMedium", L"Impact,Arial Narrow", 0, 1252},
    {0x3e6aa32d, L"NSimSun", L"STHeiti,STFangsong", 1, 936},
    {0x3f6c36a8, L"Gautami",
     L"Damascus,STIXNonUnicode,STIXGeneral,American Typewriter", 0, 1252},
    {0x3ff32662, L"Chiller-Regular", L"Papyrus,KufiStandardGK,Baghdad", 0,
     1252},
    {0x409de312, L"ModernNo.20", L"Avenir Next Condensed,Impact", 2, 1252},
    {0x41443c5e, L"Georgia", L".Helvetica Neue Desk UI,Arial Unicode MS", 2,
     1252},
    {0x4160ade5, L"BellGothicStdBlack", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0x421976c4, L"Modern-Regular", L"Impact", 2, 1252},
    {0x422a7252, L"Stencil", L"STIXNonUnicode,Songti SC,Georgia,Baskerville", 0,
     1252},
    {0x42c8554f, L"Fixedsys", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x435cb41d, L"Roman", L"Arial Narrow", 0, 1252},
    {0x47882383, L"CourierNew", L"PCMyungjo,Osaka,Arial Unicode MS,Songti SC",
     1, 1252},
    {0x480a2338, L"BerlinSansFBDemi",
     L"STIXNonUnicode,American Typewriter,Avenir Next Condensed Heavy", 0,
     1252},
    {0x480bf7a4, L"CourierStd", L"Courier New", 0, 1252},
    {0x481ad6ed, L"VladimirScript",
     L"STIXNonUnicode,Avenir Next Condensed,Impact", 8, 1252},
    {0x4911577a, L"YouYuan", L"STHeiti,Heiti TC", 1, 936},
    {0x4a788d72, L"STXingkai", L"Kaiti SC,Songti SC", 0, 936},
    {0x4bf88566, L"SegoeCondensed", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x4ccf51a4, L"BerlinSansFB-Reg",
     L"STIXNonUnicode,American Typewriter,Impact", 0, 1252},
    {0x4ea967ce, L"GulimChe", L"Arial Unicode MS,Heiti TC,STFangsong", 1, 1252},
    {0x4f68bd79, L"LetterGothicStd",
     L"Courier New,Andale Mono,Ayuthaya,PCMyungjo,Osaka", 0, 1252},
    {0x51a0d0e6, L"KozukaGothicPr6NM", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x531b3dea, L"BasemicSymbol", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x5333fd39, L"CalifornianFB-Reg",
     L"American Typewriter,Avenir Next Condensed,Impact", 2, 1252},
    {0x53561a54, L"FZYTK--GBK1-0", L"STFangsong,Songti SC,STSong", 0, 936},
    {0x55e0dde6, L"LucidaSansTypewriter", L"Menlo,Courier New,Andale Mono", 0,
     1252},
    {0x574d4d3d, L"AdobeArabic", L"Arial Narrow", 0, 1252},
    {0x5792e759, L"STKaiti", L"Songti SC,Arial Unicode MS", 0, 936},
    {0x5921978e, L"LucidaSansUnicode", L"Lucida Grande,Arial Unicode MS,Menlo",
     0, 1252},
    {0x594e2da4, L"Vrinda", L"Geeza Pro,Damascus,STIXGeneral,Gill Sans", 0,
     1252},
    {0x59baa9a2, L"KaiTi_GB2312", L"Hiragino Sans GB,STHeiti", 0, 1252},
    {0x5cfedf4f, L"BaskOldFace",
     L"Avenir Next Condensed Heavy,PT Sans,Avenir Next Condensed", 0, 1252},
    {0x5e16ac91, L"TrajanPro", L"Arial Narrow,PT Sans Narrow,Damascus", 0,
     1252},
    {0x5f97921c, L"AdobeMyungjoStdM",
     L"AppleMyungjo,AppleGothic,Arial Unicode MS", 0, 936},
    {0x5fefbfad, L"Batang", L"Arial Unicode MS,Songti SC", 2, 1252},
    {0x605342b9, L"DotumChe", L"Arial Unicode MS,Heiti TC", 1, 1252},
    {0x608c5f9a, L"KaiTi_GB2312", L"Hiragino Sans GB,STHeiti,Heiti TC", 0, 936},
    {0x61efd0d1, L"MaturaMTScriptCapitals",
     L"Kokonor,Damascus,STIXNonUnicode,STHeiti,Arial Black,Avenir Next Heavy",
     0, 1252},
    {0x626608a9, L"MVBoli",
     L"Apple Braille,Geeza Pro,Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x630501a3, L"SmallFonts", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x65d0e2a9, L"FZYTK--GBK1-0", L"STFangsong,Songti SC,STSong", 0, 936},
    {0x669f29e1, L"FZSTK--GBK1-0", L"STHeiti,Heiti TC", 0, 936},
    {0x673a9e5f, L"Tunga",
     L"Damascus,STIXNonUnicode,Avenir Next Condensed,Avenir Next Condensed "
     L"Ultra Light,Futura",
     0, 1252},
    {0x691aa4ce, L"NiagaraSolid", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x696259b7, L"Corbel", L"Cochin,Baskerville,Marion", 0, 1252},
    {0x696ee9be, L"STXihei", L"STHeiti,Heiti TC,Songti SC,Arial Unicode MS", 0,
     936},
    {0x6c59cf69, L"Dotum", L"Arial Unicode MS,Songti SC", 0, 1252},
    {0x707fa561, L"Gungsuh", L"Arial Unicode MS,Heiti TC", 2, 1252},
    {0x71416bb2, L"ZWAdobeF",
     L"STIXSizeFourSym,STIXSizeThreeSym,STIXSizeTwoSym,STIXSizeOneSym", 0,
     1252},
    {0x71b41801, L"Verdana",
     L"Tahoma,Marion,Apple Symbols,.Helvetica Neue Desk UI,Lucida "
     L"Grande,Courier New",
     0, 1252},
    {0x73f25e4c, L"PalatinoLinotype", L"Palatino,Arial Unicode MS", 0, 1252},
    {0x73f4d19f, L"NiagaraEngraved", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x74001694, L"MyriadProBlack", L"Palatino,Baskerville,Marion,Cochin", 0,
     1252},
    {0x74b14d8f, L"Haettenschweiler", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x74cb44ee, L"NSimSun", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0x76b4d7ff, L"Shruti",
     L"Damascus,STIXNonUnicode,Arial Unicode MS,American Typewriter", 0, 1252},
    {0x788b3533, L"Webdings", L"Microsoft Sans Serif,Apple Symbols", 6, 42},
    {0x797dde99, L"MSSerif", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x7a0f9e9e, L"MSMincho",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE,Arial Unicode MS,Apple SD Gothic Neo",
     1, 1252},
    {0x7b439caf, L"OldEnglishTextMT",
     L"STIXNonUnicode,Arial Unicode MS,Baskerville,Avenir Next Medium", 0,
     1252},
    {0x8213a433, L"LucidaSans-Typewriter",
     L"Comic Sans MS,Avenir Next,Arial Rounded MT Bold", 0, 1252},
    {0x82fec929, L"AdobeSongStdL", L"Heiti TC,STHeiti", 0, 936},
    {0x83581825, L"Modern", L"Avenir Next Condensed,Impact", 0, 1252},
    {0x835a2823, L"Algerian",
     L"STIXNonUnicode,Baskerville,Avenir Next Medium,American Typewriter", 0,
     1252},
    {0x83dab9f5, L"Script", L"Arial Narrow", 0, 1252},
    {0x847b56da, L"Tahoma", L"Songti SC,Apple Symbols", 0, 1252},
    {0x8a783cb2, L"SimSun-PUA", L"STHeiti,Heiti TC,STFangsong", 0, 1252},
    {0x8b5cac0e, L"Onyx", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0x8c6a499e, L"Gulim", L"Arial Unicode MS,Songti SC", 0, 1252},
    {0x8e0af790, L"JuiceITC", L"Nadeem,Al Bayan", 0, 1252},
    {0x8e8d43b2, L"Centaur", L"Avenir Next Condensed,Noteworthy,Impact", 2,
     1252},
    {0x8ee4dcca, L"BookshelfSymbol7", L"Microsoft Sans Serif,Apple Symbols", 0,
     1252},
    {0x90794800, L"BellGothicStdLight", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0x909b516a, L"Century", L"Damascus,Andale Mono,Songti SC,Arial Unicode MS",
     2, 1252},
    {0x92ae370d, L"MSOutlook", L"Microsoft Sans Serif,Apple Symbols", 4, 42},
    {0x93c9fbf1, L"LucidaFax",
     L"PT Sans Narrow,Papyrus,Kokonor,Geeza Pro,Arial Rounded MT Bold,Lucida "
     L"Grande,Futura",
     2, 1252},
    {0x9565085e, L"BookAntiqua", L"Palatino,Microsoft Sans Serif,Apple Symbols",
     2, 1252},
    {0x9856d95d, L"AdobeMingStdL", L"AHiragino Sans GB,Heiti TC,STHeiti", 0,
     949},
    {0x9bbadd6b, L"ColonnaMT", L"Noteworthy,Avenir Next Condensed,Impact", 0,
     1252},
    {0x9cbd16a4, L"ShowcardGothic-Reg",
     L"Arial Unicode MS,Georgia,American Typewriter", 0, 1252},
    {0x9d73008e, L"MSSansSerif", L"Songti SC,Apple Symbols", 0, 1252},
    {0xa0607db1, L"GungsuhChe",
     L"WenQuanYi Zen Hei Mono,AR PL UMing CN,AR PL UMing HK,AR PL UMing TW,AR "
     L"PL UMing TW MBE,Arial Unicode MS,Heiti TC,STFangsong",
     1, 1252},
    {0xa0bcf6a1, L"LatinWide", L"Zapfino,Arial Black,STHeiti", 2, 1252},
    {0xa1429b36, L"Symbol", L"Microsoft Sans Serif,Apple Symbols", 6, 42},
    {0xa1fa5abc, L"Wingdings2", L"Microsoft Sans Serif,Apple Symbols", 6, 42},
    {0xa1fa5abd, L"Wingdings3", L"Microsoft Sans Serif,Apple Symbols", 6, 42},
    {0xa427bad4, L"InformalRoman-Regular",
     L"STIXNonUnicode,Arial Narrow,Avenir Next Condensed Demi Bold", 8, 1252},
    {0xa8b92ece, L"FZSTK--GBK1-0", L"STHeiti,Heiti TC,STFangsong", 0, 936},
    {0xa8d83ece, L"CalifornianFB",
     L"American Typewriter,Avenir Next Condensed,Impact", 2, 1252},
    {0xaa3e082c, L"Kingsoft-Phonetic",
     L"STIXVariants,STIXSizeOneSym,Apple Braille", 0, 1252},
    {0xaa6bcabe, L"HarlowSolidItalic",
     L"STIXNonUnicode,Avenir Medium,Avenir Next Medium,Arial Unicode MS", 0,
     1252},
    {0xade5337c, L"MSUIGothic", L"Arial Unicode MS,Apple SD Gothic Neo", 0,
     1252},
    {0xb08dd941, L"WideLatin",
     L"Marion,Papyrus,Nanum Pen Script,Zapf Dingbats,Damascus,Zapfino,Arial "
     L"Black,STHeiti",
     2, 1252},
    {0xb12765e0, L"ITCLegacySansStdBook",
     L"LastResort,.Helvetica Neue Desk UI,Arial Unicode MS,Palatino", 0, 1252},
    {0xb207f05d, L"PoorRichard", L"Noteworthy,Avenir Next Condensed,Impact", 2,
     1252},
    {0xb3bc492f, L"JuiceITC-Regular", L"Nadeem,Al Bayan,STIXNonUnicode", 0,
     1252},
    {0xb5545399, L"Marlett", L"Microsoft Sans Serif,Apple Symbols", 4, 42},
    {0xb5dd1ebb, L"BritannicBold",
     L"Damascus,STIXNonUnicode,Avenir Next Condensed Heavy,PT Sans", 0, 1252},
    {0xb699c1c5, L"LucidaCalligraphy-Italic", L"STHeiti,Arial Black", 0, 1252},
    {0xb725d629, L"TimesNewRoman", L"Microsoft Sans Serif,Apple Symbols", 2,
     1252},
    {0xb7eaebeb, L"AdobeHeitiStdR", L"Heiti TC,STHeiti", 0, 936},
    {0xbd29c486, L"BerlinSansFBDemi-Bold",
     L"American Typewriter,Avenir Next Condensed Heavy", 0, 1252},
    {0xbe8a8db4, L"BookshelfSymbolSeven", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0xc16c0118, L"AdobeHebrew",
     L".Helvetica Neue Desk UI,Palatino,American Typewriter", 0, 1252},
    {0xc318b0af, L"MyriadProLight", L"Palatino,Baskerville,Marion", 0, 1252},
    {0xc65e5659, L"CambriaMath", L"Arial Unicode MS", 2, 1252},
    {0xc75c8f05, L"LucidaConsole", L"Courier New,Menlo,Andale Mono", 1, 1252},
    {0xca7c35d6, L"Calibri", L"Apple Symbols,HeadLineA", 0, 1252},
    {0xcb053f53, L"MicrosoftYaHei", L"Arial Unicode MS", 0, 936},
    {0xcb7190f9, L"Magneto-Bold", L"Lucida Grande", 0, 1252},
    {0xcca00cc5, L"System", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0xccad6f76, L"Jokerman-Regular", L"Lucida Grande", 0, 1252},
    {0xccc5818c, L"EuroSign", L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0xcf3d7234, L"LucidaHandwriting-Italic",
     L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0xcf7b8fdb, L"MinionPro",
     L"Bell MT,Corbel,Times New Roman,Cambria,Berlin Sans FB", 0, 1252},
    {0xcfe5755f, L"Simhei", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0xd011f4ee, L"MSPGothic", L"Arial Unicode MS,Apple SD Gothic Neo", 0,
     1252},
    {0xd060e7ef, L"Vivaldi",
     L"STIXNonUnicode,Arial Unicode MS,Avenir Medium,Avenir Next Medium", 8,
     1252},
    {0xd07edec1, L"FranklinGothic-Medium", L"Impact,Arial Narrow", 0, 1252},
    {0xd107243f, L"SimSun", L"STHeiti,Heiti TC,STFangsong", 0, 936},
    {0xd1881562, L"ArialNarrow", L"PT Sans Narrow,Apple Symbols", 0, 1252},
    {0xd22b7dce, L"BodoniMTPosterCompressed",
     L"Microsoft Sans Serif,Apple Symbols", 0, 1252},
    {0xd22bfa60, L"ComicSansMS",
     L"Damascus,Georgia,.Helvetica Neue Desk UI,Lucida Grande,Arial Unicode MS",
     8, 1252},
    {0xd3bd0e35, L"Bauhaus93",
     L"STIXNonUnicode,Arial Unicode MS,Avenir Next,Avenir", 0, 1252},
    {0xd429ee7a, L"STFangsong", L"Songti SC,Arial Unicode MS", 0, 936},
    {0xd6679c12, L"BernardMTCondensed",
     L"Impact,Avenir Next Condensed Demi Bold", 0, 1252},
    {0xd8e8a027, L"LucidaSans",
     L"Arial Narrow,Khmer MN,Kokonor,Damascus,Microsoft Sans Serif,Apple "
     L"Symbols",
     0, 1252},
    {0xd9fe7761, L"HighTowerText-Reg",
     L"STIXGeneral,.Helvetica Neue Desk UI,Trebuchet MS", 2, 1252},
    {0xda7e551e, L"STSong", L"Arial Unicode MS", 0, 936},
    {0xdaa6842d, L"STZhongsong", L"STFangsong,Songti SC,STSong", 0, 936},
    {0xdaaab93f, L"STFangsong", L"Songti SC,Arial Unicode MS", 0, 936},
    {0xdaeb0713, L"STSong", L"Songti SC,Arial Unicode MS", 0, 936},
    {0xdafedbef, L"STCaiyun", L"Kaiti SC,Songti SC,STHeiti", 0, 936},
    {0xdb00a3d9, L"Broadway",
     L"Papyrus,STIXNonUnicode,Arial Black,Avenir Next Heavy,Heiti TC", 0, 1252},
    {0xdb1f5ad4, L"STXinwei", L"Kaiti SC,Songti SC,STHeiti", 0, 936},
    {0xdb326e7f, L"STKaiti", L"Songti SC,Arial Unicode MS", 0, 936},
    {0xdb69595a, L"STHupo", L"Kaiti SC,Songti SC,STHeiti", 0, 936},
    {0xdba0082c, L"STXihei", L"Songti SC,Arial Unicode MS", 0, 936},
    {0xdbd0ab18, L"STXingkai", L"Kaiti SC,Songti SC", 0, 936},
    {0xdc1a7db1, L"STLiti", L"Kaiti SC,Songti SC", 0, 936},
    {0xdc33075f, L"KristenITC-Regular",
     L"STIXNonUnicode,Damascus,Songti SC,STSong", 8, 1252},
    {0xdcc7009c, L"Harrington",
     L"STIXNonUnicode,Avenir Next Condensed Heavy,Noteworthy", 0, 1252},
    {0xdd712466, L"ArialBlack", L"Geeza Pro,Damascus,Songti SC,STSong", 0,
     1252},
    {0xdde87b3e, L"Impact", L"Arial Narrow,Marion", 0, 1252},
    {0xdf69fb32, L"SnapITC",
     L"Arial Narrow,PT Sans Narrow,Marion,STHeiti,Arial Black", 0, 1252},
    {0xdf8b25e8, L"CenturyGothic",
     L"Damascus,Andale Mono,Songti SC,Arial Unicode MS", 0, 1252},
    {0xe0f705c0, L"KristenITC", L"Songti SC,STSong", 8, 1252},
    {0xe1427573, L"Raavi",
     L"Damascus,STIXNonUnicode,Marion,Papyrus,Avenir Next Condensed "
     L"Heavy,American Typewriter",
     0, 1252},
    {0xe2cea0cb, L"Magneto",
     L"STIXNonUnicode,Damascus,Geeza Pro,Lucida Grande,Georgia,Heiti TC", 0,
     1252},
    {0xe36a9e17, L"Ravie", L"STHeiti,Arial Black", 0, 1252},
    {0xe433f8e2, L"Parchment", L"Microsoft Sans Serif,Apple Symbols", 8, 1252},
    {0xe43dff4a, L"Wingdings", L"Microsoft Sans Serif,Apple Symbols", 4, 42},
    {0xe4e2c405, L"MTExtra", L"Microsoft Sans Serif,Apple Symbols", 6, 42},
    {0xe618cc35, L"InformalRoman", L"Arial Narrow", 8, 1252},
    {0xe6c27ffc, L"Mistral", L"Apple Symbols", 8, 1252},
    {0xe7ebf4b9, L"Courier", L"Courier New", 0, 1252},
    {0xe8bc4a9d, L"MSReferenceSpecialty", L"Microsoft Sans Serif,Apple Symbols",
     0, 1252},
    {0xe90fb013, L"TempusSansITC",
     L"STIXNonUnicode,Microsoft Sans Serif,Avenir Light", 0, 1252},
    {0xec637b42, L"Consolas",
     L"AR PL UKai CN,AR PL UKai HK,AR PL UKai TW,AR PL UKai TW MBE,AR PL UMing "
     L"CN,AR PL UMing HK,Microsoft Sans Serif,Tahoma",
     1, 1252},
    {0xed3a683b, L"STXinwei", L"Kaiti SC,Songti SC,", 0, 936},
    {0xef264cd1, L"LucidaHandwriting",
     L"Arial Narrow,Avenir Next Condensed Demi Bold,Avenir Next "
     L"Condensed,Avenir Next Condensed Medium,STHeiti,Arial Black",
     0, 1252},
    {0xf086bca2, L"BaskervilleOldFace",
     L"STIXNonUnicode,Avenir Next Condensed Heavy,PT Sans", 0, 1252},
    {0xf1028030, L"Mangal",
     L"Arial Unicode MS,Microsoft Sans Serif,Arial Narrow,Tahoma", 2, 1252},
    {0xf1da7eb9, L"ShowcardGothic",
     L"Papyrus,Arial Unicode MS,Georgia,American Typewriter", 0, 1252},
    {0xf210f06a, L"ArialMT",
     L"Arial Unicode MS,Arial Narrow,STIXNonUnicode,Damascus,Avenir Next "
     L"Condensed Demi Bold,Avenir Next Condensed Medium,Avenir Next Condensed",
     0, 1252},
    {0xf477f16a, L"Latha",
     L"Arial Narrow,Damascus,STIXNonUnicode,American Typewriter", 0, 1252},
    {0xf616f3dd, L"LiSu", L"STHeiti,Heiti TC,STFangsong", 1, 936},
    {0xfa479aa6, L"MicrosoftYaHei", L"Arial Unicode MS", 0, 936},
    {0xfcd19697, L"BookmanOldStyle",
     L"Geeza Pro,Damascus,Andale Mono,Songti SC,Arial Unicode MS", 0, 1252},
    {0xfe209a82, L"LucidaCalligraphy",
     L"Kokonor,Damascus,STIXNonUnicode,STHeiti,Arial Black", 0, 1252},
    {0xfef135f8, L"AdobeHeitiStd-Regular", L"Heiti TC,STHeiti", 0, 936},
};
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_ANDROID_
static const XFA_FONTINFO g_XFAFontsMap[] = {
    {0x01d5d33e, L"SimSun", L"Droid Sans Fallback", 0, 936},
    {0x01e4f102, L"YouYuan", L"Droid Sans Fallback", 1, 936},
    {0x030549dc, L"LiSu", L"Droid Sans Fallback", 1, 936},
    {0x032edd44, L"Simhei", L"Droid Sans Fallback", 1, 936},
    {0x03eac6fc, L"PoorRichard-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback,Droid Arabic "
     L"Naskh,Droid Sans Ethiopic",
     2, 1252},
    {0x03ed90e6, L"Nina",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x077b56b3, L"KingsoftPhoneticPlain",
     L"Droid Sans Thai,Droid Sans Armenian,Droid Arabic Naskh,Droid Sans "
     L"Ethiopic,Droid Sans Fallback",
     0, 1252},
    {0x078ed524, L"MicrosoftSansSerif", L"Droid Sans Fallback", 0, 1252},
    {0x089b18a9, L"Arial", L"Droid Sans Fallback", 0, 1252},
    {0x0b2cad72, L"MonotypeCorsiva",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x0bb003e7, L"Kartika",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     2, 1252},
    {0x0bb469df, L"VinerHandITC",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x0bc1a851, L"SegoeUI", L"Droid Sans Fallback", 0, 1252},
    {0x0c112ebd, L"KozukaGothicPro-VIM",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x0cfcb9c1, L"AdobeThai",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 847},
    {0x0e7de0f9, L"Playbill",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0x0eff47c3, L"STHupo", L"Droid Sans Fallback", 0, 936},
    {0x107ad374, L"Constantia",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x12194c2d, L"KunstlerScript",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x135ef6a1, L"MinionProSmBd",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x158c4049, L"Garamond",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x160ecb24, L"STZhongsong", L"Droid Sans Fallback", 0, 936},
    {0x161ed07e, L"MSGothic", L"Droid Sans Fallback", 1, 1252},
    {0x171d1ed1, L"SnapITC-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x18d1188f, L"Cambria", L"Droid Sans Fallback", 2, 1252},
    {0x18eaf350, L"ArialUnicodeMS", L"Droid Sans Fallback", 0, 936},
    {0x1a92d115, L"MingLiU", L"Droid Sans Fallback", 1, 1252},
    {0x1cc217c6, L"TrebuchetMS",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x1d649596, L"BasemicTimes",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x1e34ee60, L"BellMT",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x1eb36945, L"CooperBlack",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x1ef7787d, L"BatangChe", L"Droid Sans Fallback", 1, 1252},
    {0x20b3bd3a, L"BrushScriptMT", L"Droid Arabic Naskh,Droid Sans Ethiopic", 8,
     1252},
    {0x220877aa, L"Candara",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x22135007, L"FreestyleScript-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x251059c3, L"Chiller",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif", 0, 1252},
    {0x25bed6dd, L"MSReferenceSansSerif", L"Droid Sans Fallback", 0, 1252},
    {0x28154c81, L"Parchment-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x29711eb9, L"STLiti", L"Droid Sans Fallback", 0, 936},
    {0x2b1993b4, L"Basemic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2b316339, L"NiagaraSolid-Reg",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2c147529, L"FootlightMTLight",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2c198928, L"HarlowSolid",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2c6ac6b2, L"LucidaBright",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto", 2, 1252},
    {0x2c9f38e2, L"KozukaMinchoPro-VIR",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2d5a47b0, L"STCaiyun", L"Droid Sans Fallback", 0, 936},
    {0x2def26bf, L"BernardMT-Condensed",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x2fd8930b, L"KozukaMinchoPr6NR",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x3115525a, L"FangSong_GB2312", L"Droid Sans Fallback", 0, 1252},
    {0x31327817, L"MyriadPro",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x32244975, L"Helvetica",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto", 0, 1252},
    {0x32ac995c, L"Terminal",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x338d648a, L"NiagaraEngraved-Reg",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x33bb65f2, L"Sylfaen",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x3402c30e, L"MSPMincho", L"Droid Sans Fallback", 2, 1252},
    {0x3412bf31, L"SimSun-PUA", L"Droid Sans Fallback", 0, 936},
    {0x36eb39b9, L"BerlinSansFB",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x36f42055, L"UniversATT", L"Microsoft Sans Serif", 0, 1252},
    {0x3864c4f6, L"HighTowerText",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x3a257d03, L"FangSong_GB2312", L"Droid Sans Fallback", 0, 1252},
    {0x3cdae668, L"FreestyleScript",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x3d55aed7, L"Jokerman",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x3d5b4385, L"PMingLiU", L"Droid Sans Fallback", 2, 1252},
    {0x3d9b7669, L"EstrangeloEdessa",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x3e532d74, L"FranklinGothicMedium",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x3e6aa32d, L"NSimSun", L"Droid Sans Fallback", 1, 936},
    {0x3f6c36a8, L"Gautami",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono,Droid Sans Fallback",
     0, 1252},
    {0x3ff32662, L"Chiller-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x409de312, L"ModernNo.20",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x41443c5e, L"Georgia",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x4160ade5, L"BellGothicStdBlack",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x421976c4, L"Modern-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x422a7252, L"Stencil",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x42c8554f, L"Fixedsys",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x435cb41d, L"Roman",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x47882383, L"CourierNew", L"Droid Sans Fallback", 1, 1252},
    {0x480a2338, L"BerlinSansFBDemi",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x480bf7a4, L"CourierStd", L"Droid Sans Fallback", 0, 1252},
    {0x481ad6ed, L"VladimirScript",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0x4911577a, L"YouYuan", L"Droid Sans Fallback", 1, 936},
    {0x4a788d72, L"STXingkai", L"Droid Sans Fallback", 0, 936},
    {0x4bf88566, L"SegoeCondensed",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x4ccf51a4, L"BerlinSansFB-Reg",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x4ea967ce, L"GulimChe", L"Droid Sans Fallback", 1, 1252},
    {0x4f68bd79, L"LetterGothicStd",
     L"Droid Sans Mono,Droid Arabic Naskh,Droid Sans Ethiopic,Droid Sans "
     L"Mono,Droid Serif,Droid Sans Fallback",
     0, 1252},
    {0x51a0d0e6, L"KozukaGothicPr6NM",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x531b3dea, L"BasemicSymbol",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x5333fd39, L"CalifornianFB-Reg",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x53561a54, L"FZYTK--GBK1-0", L"Droid Sans Fallback", 0, 936},
    {0x55e0dde6, L"LucidaSansTypewriter",
     L"Droid Sans Mono,Droid Arabic Naskh,Droid Sans Ethiopic", 0, 1252},
    {0x574d4d3d, L"AdobeArabic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x5792e759, L"STKaiti", L"Droid Sans Fallback", 0, 936},
    {0x5921978e, L"LucidaSansUnicode", L"Droid Sans Fallback", 0, 1252},
    {0x594e2da4, L"Vrinda",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0x59baa9a2, L"KaiTi_GB2312", L"Droid Sans Fallback", 0, 1252},
    {0x5cfedf4f, L"BaskOldFace",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x5f97921c, L"AdobeMyungjoStdM", L"Droid Sans Fallback", 0, 936},
    {0x5fefbfad, L"Batang", L"Droid Sans Fallback", 2, 1252},
    {0x605342b9, L"DotumChe", L"Droid Sans Fallback", 1, 1252},
    {0x608c5f9a, L"KaiTi_GB2312", L"Droid Sans Fallback", 0, 936},
    {0x61efd0d1, L"MaturaMTScriptCapitals",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0x626608a9, L"MVBoli",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0x630501a3, L"SmallFonts",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x65d0e2a9, L"FZYTK--GBK1-0", L"Droid Sans Fallback", 0, 936},
    {0x669f29e1, L"FZSTK--GBK1-0", L"Droid Sans Fallback", 0, 936},
    {0x673a9e5f, L"Tunga",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono,Droid Sans Fallback",
     0, 1252},
    {0x691aa4ce, L"NiagaraSolid",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x696259b7, L"Corbel",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x696ee9be, L"STXihei", L"Droid Sans Fallback", 0, 936},
    {0x6c59cf69, L"Dotum", L"Droid Sans Fallback", 0, 1252},
    {0x707fa561, L"Gungsuh", L"Droid Sans Fallback", 2, 1252},
    {0x71416bb2, L"ZWAdobeF",
     L"Droid Arabic Naskh,Droid Sans Armenian,Droid Sans Ethiopic,Droid Sans "
     L"Georgian,Droid Sans Hebrew,Droid Sans Thai",
     0, 1252},
    {0x71b41801, L"Verdana",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x73f25e4c, L"PalatinoLinotype", L"Droid Sans Fallback", 0, 1252},
    {0x73f4d19f, L"NiagaraEngraved",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x74001694, L"MyriadProBlack", L"Book Antiqua,Constantia,Dotum,Georgia", 0,
     1252},
    {0x74b14d8f, L"Haettenschweiler",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x74cb44ee, L"NSimSun", L"Droid Sans Fallback", 1, 936},
    {0x76b4d7ff, L"Shruti",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0x788b3533, L"Webdings",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 6, 42},
    {0x797dde99, L"MSSerif",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x7a0f9e9e, L"MSMincho", L"Droid Sans Fallback", 1, 1252},
    {0x7b439caf, L"OldEnglishTextMT",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x8213a433, L"LucidaSans-Typewriter",
     L"Droid Sans Mono,Droid Serif,Roboto,Droid Sans Fallback", 0, 1252},
    {0x82fec929, L"AdobeSongStdL", L"Droid Sans Fallback", 0, 936},
    {0x83581825, L"Modern",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x835a2823, L"Algerian",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x83dab9f5, L"Script",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x847b56da, L"Tahoma", L"Droid Sans Fallback", 0, 1252},
    {0x8a783cb2, L"SimSun-PUA", L"Droid Sans Fallback", 0, 1252},
    {0x8b5cac0e, L"Onyx",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x8c6a499e, L"Gulim", L"Droid Sans Fallback", 0, 1252},
    {0x8e0af790, L"JuiceITC",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x8e8d43b2, L"Centaur",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x8ee4dcca, L"BookshelfSymbol7",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x90794800, L"BellGothicStdLight",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x909b516a, L"Century",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x92ae370d, L"MSOutlook",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 4, 42},
    {0x93c9fbf1, L"LucidaFax",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     2, 1252},
    {0x9565085e, L"BookAntiqua",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0x9856d95d, L"AdobeMingStdL", L"Droid Sans Fallback", 0, 949},
    {0x9bbadd6b, L"ColonnaMT",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0x9cbd16a4, L"ShowcardGothic-Reg",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallbac", 0, 1252},
    {0x9d73008e, L"MSSansSerif",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xa0607db1, L"GungsuhChe", L"Droid Sans Fallback", 1, 1252},
    {0xa0bcf6a1, L"LatinWide",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0xa1429b36, L"Symbol",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 6, 42},
    {0xa1fa5abc, L"Wingdings2",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 6, 42},
    {0xa1fa5abd, L"Wingdings3",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 6, 42},
    {0xa427bad4, L"InformalRoman-Regular",
     L"Droid Arabic Naskh,Droid Sans Ethiopic", 8, 1252},
    {0xa8b92ece, L"FZSTK--GBK1-0", L"Droid Sans Fallback", 0, 936},
    {0xa8d83ece, L"CalifornianFB",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0xaa3e082c, L"Kingsoft-Phonetic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xaa6bcabe, L"HarlowSolidItalic",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xade5337c, L"MSUIGothic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xb08dd941, L"WideLatin",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     2, 1252},
    {0xb207f05d, L"PoorRichard",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0xb3bc492f, L"JuiceITC-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xb5545399, L"Marlett",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 4, 42},
    {0xb5dd1ebb, L"BritannicBold", L"Droid Arabic Naskh,Droid Sans Ethiopic", 0,
     1252},
    {0xb699c1c5, L"LucidaCalligraphy-Italic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xb725d629, L"TimesNewRoman", L"Droid Sans Fallback", 2, 1252},
    {0xb7eaebeb, L"AdobeHeitiStdR", L"Droid Sans Fallback", 0, 936},
    {0xbd29c486, L"BerlinSansFBDemi-Bold",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xbe8a8db4, L"BookshelfSymbolSeven",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xc16c0118, L"AdobeHebrew",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback,Droid Arabic "
     L"Naskh,Droid Sans Ethiopic",
     0, 1252},
    {0xc318b0af, L"MyriadProLight",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xc65e5659, L"CambriaMath", L"Droid Sans Fallback", 2, 1252},
    {0xc75c8f05, L"LucidaConsole",
     L"Droid Sans Mono,Droid Serif,Roboto,Droid Sans Fallback", 1, 1252},
    {0xca7c35d6, L"Calibri", L"Droid Sans Fallback", 0, 1252},
    {0xcb053f53, L"MicrosoftYaHei", L"Droid Sans Fallback", 0, 936},
    {0xcb7190f9, L"Magneto-Bold",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xcca00cc5, L"System",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xccad6f76, L"Jokerman-Regular",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xccc5818c, L"EuroSign",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xcf3d7234, L"LucidaHandwriting-Italic",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xcf7b8fdb, L"MinionPro",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xcfe5755f, L"Simhei", L"Droid Sans Fallback", 1, 936},
    {0xd011f4ee, L"MSPGothic", L"Droid Sans Fallback", 0, 1252},
    {0xd060e7ef, L"Vivaldi",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0xd07edec1, L"FranklinGothic-Medium",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xd107243f, L"SimSun", L"Droid Sans Fallback", 0, 936},
    {0xd1881562, L"ArialNarrow",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xd22b7dce, L"BodoniMTPosterCompressed",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xd22bfa60, L"ComicSansMS", L"Droid Serif,Roboto,Droid Sans Fallback", 8,
     1252},
    {0xd3bd0e35, L"Bauhaus93",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xd429ee7a, L"STFangsong", L"Droid Sans Fallback", 0, 936},
    {0xd6679c12, L"BernardMTCondensed",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xd8e8a027, L"LucidaSans",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto", 0, 1252},
    {0xd9fe7761, L"HighTowerText-Reg",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 2, 1252},
    {0xda7e551e, L"STSong", L"Droid Sans Fallback", 0, 936},
    {0xdaa6842d, L"STZhongsong", L"Droid Sans Fallback", 0, 936},
    {0xdaaab93f, L"STFangsong", L"Droid Sans Fallback", 0, 936},
    {0xdaeb0713, L"STSong",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 936},
    {0xdafedbef, L"STCaiyun", L"Droid Sans Fallback", 0, 936},
    {0xdb00a3d9, L"Broadway",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xdb1f5ad4, L"STXinwei", L"Droid Sans Fallback", 0, 936},
    {0xdb326e7f, L"STKaiti", L"Droid Sans Fallback", 0, 936},
    {0xdb69595a, L"STHupo", L"Droid Sans Fallback", 0, 936},
    {0xdba0082c, L"STXihei", L"Droid Sans Fallback", 0, 936},
    {0xdbd0ab18, L"STXingkai", L"Droid Sans Fallback", 0, 936},
    {0xdc1a7db1, L"STLiti", L"Droid Sans Fallback", 0, 936},
    {0xdc33075f, L"KristenITC-Regular",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto", 8, 1252},
    {0xdcc7009c, L"Harrington",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xdd712466, L"ArialBlack",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xdde87b3e, L"Impact",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xdf69fb32, L"SnapITC",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0xdf8b25e8, L"CenturyGothic",
     L"Droid Serif,Roboto,Droid Serif,Droid Sans Mono", 0, 1252},
    {0xe0f705c0, L"KristenITC",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto", 8, 1252},
    {0xe1427573, L"Raavi",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0xe2cea0cb, L"Magneto",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0xe36a9e17, L"Ravie",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0xe433f8e2, L"Parchment",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0xe43dff4a, L"Wingdings",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 4, 42},
    {0xe4e2c405, L"MTExtra",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 6, 42},
    {0xe618cc35, L"InformalRoman",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif", 8, 1252},
    {0xe6c27ffc, L"Mistral",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 8, 1252},
    {0xe7ebf4b9, L"Courier", L"Droid Sans Fallback", 0, 1252},
    {0xe8bc4a9d, L"MSReferenceSpecialty",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xe90fb013, L"TempusSansITC",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xec637b42, L"Consolas", L"Droid Sans Fallback", 1, 1252},
    {0xed3a683b, L"STXinwei", L"Droid Sans Fallback", 0, 936},
    {0xef264cd1, L"LucidaHandwriting",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0xf086bca2, L"BaskervilleOldFace",
     L"Roboto,Droid Serif,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xf1028030, L"Mangal",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     2, 1252},
    {0xf1da7eb9, L"ShowcardGothic",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallbac", 0, 1252},
    {0xf210f06a, L"ArialMT",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif", 0, 1252},
    {0xf477f16a, L"Latha",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Roboto,Droid Serif,Droid Sans "
     L"Mono",
     0, 1252},
    {0xf616f3dd, L"LiSu", L"Droid Sans Fallback", 1, 936},
    {0xfa479aa6, L"MicrosoftYaHei", L"Droid Sans Fallback", 0, 936},
    {0xfcd19697, L"BookmanOldStyle",
     L"Droid Serif,Roboto,Droid Sans Mono,Droid Sans Fallback", 0, 1252},
    {0xfe209a82, L"LucidaCalligraphy",
     L"Droid Arabic Naskh,Droid Sans Ethiopic,Droid Serif,Roboto,Droid Sans "
     L"Mono",
     0, 1252},
    {0xfef135f8, L"AdobeHeitiStd-Regular", L"Droid Sans Fallback", 0, 936},
};
#endif
void XFA_LocalFontNameToEnglishName(const CFX_WideStringC& wsLocalName,
                                    CFX_WideString& wsEnglishName) {
  wsEnglishName = wsLocalName;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_ || \
    _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_ ||   \
    _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_ ||   \
    _FXM_PLATFORM_ == _FXM_PLATFORM_ANDROID_
  uint32_t dwLocalNameHash = FX_HashCode_String_GetW(
      wsLocalName.raw_str(), wsLocalName.GetLength(), TRUE);
  int32_t iStart = 0;
  int32_t iEnd = sizeof(g_XFAFontsMap) / sizeof(XFA_FONTINFO) - 1;
  int32_t iMid = 0;
  do {
    iMid = (iStart + iEnd) / 2;
    uint32_t dwFontNameHash = g_XFAFontsMap[iMid].dwFontNameHash;
    if (dwFontNameHash == dwLocalNameHash) {
      wsEnglishName = g_XFAFontsMap[iMid].pPsName;
      break;
    } else if (dwFontNameHash < dwLocalNameHash) {
      iStart = iMid + 1;
    } else {
      iEnd = iMid - 1;
    }
  } while (iEnd >= iStart);
#endif
}
const XFA_FONTINFO* XFA_GetFontINFOByFontName(
    const CFX_WideStringC& wsFontName) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_ || \
    _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_ ||   \
    _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_ ||   \
    _FXM_PLATFORM_ == _FXM_PLATFORM_ANDROID_
  CFX_WideString wsFontNameTemp = wsFontName;
  wsFontNameTemp.Remove(L' ');
  uint32_t dwCurFontNameHash =
      FX_HashCode_String_GetW(wsFontNameTemp, wsFontNameTemp.GetLength(), TRUE);
  int32_t iStart = 0;
  int32_t iEnd = sizeof(g_XFAFontsMap) / sizeof(XFA_FONTINFO) - 1;
  int32_t iMid = 0;
  const XFA_FONTINFO* pFontInfo = NULL;
  do {
    iMid = (iStart + iEnd) / 2;
    uint32_t dwFontNameHash = g_XFAFontsMap[iMid].dwFontNameHash;
    if (dwFontNameHash == dwCurFontNameHash) {
      pFontInfo = &g_XFAFontsMap[iMid];
      break;
    } else if (dwFontNameHash < dwCurFontNameHash) {
      iStart = iMid + 1;
    } else {
      iEnd = iMid - 1;
    }
  } while (iEnd >= iStart);
  return pFontInfo;
#else
  return NULL;
#endif
}

CXFA_DefFontMgr::~CXFA_DefFontMgr() {
  int32_t iCounts = m_CacheFonts.GetSize();
  for (int32_t i = 0; i < iCounts; i++) {
    ((IFX_Font*)m_CacheFonts[i])->Release();
  }
  m_CacheFonts.RemoveAll();
}

IFX_Font* CXFA_DefFontMgr::GetFont(CXFA_FFDoc* hDoc,
                                   const CFX_WideStringC& wsFontFamily,
                                   uint32_t dwFontStyles,
                                   uint16_t wCodePage) {
  CFX_WideString wsFontName = wsFontFamily;
  IFX_FontMgr* pFDEFontMgr = hDoc->GetApp()->GetFDEFontMgr();
  IFX_Font* pFont = pFDEFontMgr->LoadFont(wsFontName, dwFontStyles, wCodePage);
  if (!pFont) {
    const XFA_FONTINFO* pCurFont =
        XFA_GetFontINFOByFontName(wsFontName.AsWideStringC());
    if (pCurFont && pCurFont->pReplaceFont) {
      uint32_t dwStyle = 0;
      if (dwFontStyles & FX_FONTSTYLE_Bold) {
        dwStyle |= FX_FONTSTYLE_Bold;
      }
      if (dwFontStyles & FX_FONTSTYLE_Italic) {
        dwStyle |= FX_FONTSTYLE_Italic;
      }
      const FX_WCHAR* pReplace = pCurFont->pReplaceFont;
      int32_t iLength = FXSYS_wcslen(pReplace);
      while (iLength > 0) {
        const FX_WCHAR* pNameText = pReplace;
        while (*pNameText != L',' && iLength > 0) {
          pNameText++;
          iLength--;
        }
        CFX_WideString wsReplace =
            CFX_WideString(pReplace, pNameText - pReplace);
        pFont = pFDEFontMgr->LoadFont(wsReplace, dwStyle, wCodePage);
        if (pFont) {
          break;
        }
        iLength--;
        pNameText++;
        pReplace = pNameText;
      }
    }
  }
  if (pFont) {
    m_CacheFonts.Add(pFont);
  }
  return pFont;
}

IFX_Font* CXFA_DefFontMgr::GetDefaultFont(CXFA_FFDoc* hDoc,
                                          const CFX_WideStringC& wsFontFamily,
                                          uint32_t dwFontStyles,
                                          uint16_t wCodePage) {
  IFX_FontMgr* pFDEFontMgr = hDoc->GetApp()->GetFDEFontMgr();
  IFX_Font* pFont =
      pFDEFontMgr->LoadFont(L"Arial Narrow", dwFontStyles, wCodePage);
  if (!pFont)
    pFont =
        pFDEFontMgr->LoadFont((const FX_WCHAR*)NULL, dwFontStyles, wCodePage);
  FXSYS_assert(pFont);
  if (pFont) {
    m_CacheFonts.Add(pFont);
  }
  return pFont;
}
struct XFA_PDFFONTNAME {
  const FX_CHAR* lpPsName;
  const FX_CHAR* lpNormal;
  const FX_CHAR* lpBold;
  const FX_CHAR* lpItalic;
  const FX_CHAR* lpBoldItalic;
};
const XFA_PDFFONTNAME g_XFAPDFFontName[] = {
    {"Adobe PI Std", "AdobePIStd", "AdobePIStd", "AdobePIStd", "AdobePIStd"},
    {"Myriad Pro Light", "MyriadPro-Light", "MyriadPro-Semibold",
     "MyriadPro-LightIt", "MyriadPro-SemiboldIt"},
};
CXFA_PDFFontMgr::CXFA_PDFFontMgr(CXFA_FFDoc* pDoc) {
  m_pDoc = pDoc;
}
CXFA_PDFFontMgr::~CXFA_PDFFontMgr() {
  m_FDE2PDFFont.RemoveAll();
  for (const auto& pair : m_FontMap) {
    if (pair.second)
      pair.second->Release();
  }
}
IFX_Font* CXFA_PDFFontMgr::FindFont(CFX_ByteString strPsName,
                                    FX_BOOL bBold,
                                    FX_BOOL bItalic,
                                    CPDF_Font** pDstPDFFont,
                                    FX_BOOL bStrictMatch) {
  CPDF_Document* pDoc = m_pDoc->GetPDFDoc();
  if (pDoc == NULL) {
    return NULL;
  }
  CPDF_Dictionary* pFontSetDict =
      pDoc->GetRoot()->GetDictBy("AcroForm")->GetDictBy("DR");
  if (!pFontSetDict) {
    return NULL;
  }
  pFontSetDict = (CPDF_Dictionary*)pFontSetDict->GetDictBy("Font");
  if (!pFontSetDict) {
    return NULL;
  }
  strPsName.Remove(' ');
  IFX_FontMgr* pFDEFontMgr = m_pDoc->GetApp()->GetFDEFontMgr();
  for (const auto& it : *pFontSetDict) {
    const CFX_ByteString& key = it.first;
    CPDF_Object* pObj = it.second;
    if (!PsNameMatchDRFontName(strPsName.AsByteStringC(), bBold, bItalic, key,
                               bStrictMatch)) {
      continue;
    }
    CPDF_Object* pDirect = pObj->GetDirect();
    if (!pDirect || !pDirect->IsDictionary()) {
      return NULL;
    }
    CPDF_Dictionary* pFontDict = (CPDF_Dictionary*)pDirect;
    if (pFontDict->GetStringBy("Type") != "Font") {
      return NULL;
    }
    CPDF_Font* pPDFFont = pDoc->LoadFont(pFontDict);
    if (!pPDFFont) {
      return NULL;
    }
    if (!pPDFFont->IsEmbedded()) {
      *pDstPDFFont = pPDFFont;
      return NULL;
    }
    return IFX_Font::LoadFont(&pPDFFont->m_Font, pFDEFontMgr);
  }
  return NULL;
}
IFX_Font* CXFA_PDFFontMgr::GetFont(const CFX_WideStringC& wsFontFamily,
                                   uint32_t dwFontStyles,
                                   CPDF_Font** pPDFFont,
                                   FX_BOOL bStrictMatch) {
  uint32_t dwHashCode =
      FX_HashCode_String_GetW(wsFontFamily.raw_str(), wsFontFamily.GetLength());
  CFX_ByteString strKey;
  strKey.Format("%u%u", dwHashCode, dwFontStyles);
  auto it = m_FontMap.find(strKey);
  if (it != m_FontMap.end())
    return it->second;
  CFX_ByteString bsPsName = CFX_ByteString::FromUnicode(wsFontFamily);
  FX_BOOL bBold = (dwFontStyles & FX_FONTSTYLE_Bold) == FX_FONTSTYLE_Bold;
  FX_BOOL bItalic = (dwFontStyles & FX_FONTSTYLE_Italic) == FX_FONTSTYLE_Italic;
  CFX_ByteString strFontName = PsNameToFontName(bsPsName, bBold, bItalic);
  IFX_Font* pFont =
      FindFont(strFontName, bBold, bItalic, pPDFFont, bStrictMatch);
  if (pFont)
    m_FontMap[strKey] = pFont;
  return pFont;
}
CFX_ByteString CXFA_PDFFontMgr::PsNameToFontName(
    const CFX_ByteString& strPsName,
    FX_BOOL bBold,
    FX_BOOL bItalic) {
  int32_t nCount = sizeof(g_XFAPDFFontName) / sizeof(XFA_PDFFONTNAME);
  for (int32_t i = 0; i < nCount; i++) {
    if (strPsName == g_XFAPDFFontName[i].lpPsName) {
      int32_t index = 1 + ((bItalic << 1) | bBold);
      return *(&g_XFAPDFFontName[i].lpPsName + index);
    }
  }
  return strPsName;
}
FX_BOOL CXFA_PDFFontMgr::PsNameMatchDRFontName(
    const CFX_ByteStringC& bsPsName,
    FX_BOOL bBold,
    FX_BOOL bItalic,
    const CFX_ByteString& bsDRFontName,
    FX_BOOL bStrictMatch) {
  CFX_ByteString bsDRName = bsDRFontName;
  bsDRName.Remove('-');
  int32_t iPsLen = bsPsName.GetLength();
  int32_t nIndex = bsDRName.Find(bsPsName);
  if (nIndex != -1 && !bStrictMatch) {
    return TRUE;
  }
  if (nIndex != 0) {
    return FALSE;
  }
  int32_t iDifferLength = bsDRName.GetLength() - iPsLen;
  if (iDifferLength > 1 || (bBold || bItalic)) {
    int32_t iBoldIndex = bsDRName.Find("Bold");
    FX_BOOL bBoldFont = iBoldIndex > 0;
    if (bBold ^ bBoldFont) {
      return FALSE;
    }
    if (bBoldFont) {
      iDifferLength =
          std::min(iDifferLength - 4, bsDRName.GetLength() - iBoldIndex - 4);
    }
    FX_BOOL bItalicFont = TRUE;
    if (bsDRName.Find("Italic") > 0) {
      iDifferLength -= 6;
    } else if (bsDRName.Find("It") > 0) {
      iDifferLength -= 2;
    } else if (bsDRName.Find("Oblique") > 0) {
      iDifferLength -= 7;
    } else {
      bItalicFont = FALSE;
    }
    if (bItalic ^ bItalicFont) {
      return FALSE;
    }
    if (iDifferLength > 1) {
      CFX_ByteString bsDRTailer = bsDRName.Right(iDifferLength);
      if (bsDRTailer == "MT" || bsDRTailer == "PSMT" ||
          bsDRTailer == "Regular" || bsDRTailer == "Reg") {
        return TRUE;
      }
      if (bBoldFont || bItalicFont) {
        return FALSE;
      }
      FX_BOOL bMatch = FALSE;
      switch (bsPsName.GetAt(iPsLen - 1)) {
        case 'L': {
          if (bsDRName.Right(5) == "Light") {
            bMatch = TRUE;
          }
        } break;
        case 'R': {
          if (bsDRName.Right(7) == "Regular" || bsDRName.Right(3) == "Reg") {
            bMatch = TRUE;
          }
        } break;
        case 'M': {
          if (bsDRName.Right(5) == "Medium") {
            bMatch = TRUE;
          }
        } break;
        default:
          break;
      }
      return bMatch;
    }
  }
  return TRUE;
}
FX_BOOL CXFA_PDFFontMgr::GetCharWidth(IFX_Font* pFont,
                                      FX_WCHAR wUnicode,
                                      int32_t& iWidth,
                                      FX_BOOL bCharCode) {
  if (wUnicode != 0x20 || bCharCode) {
    return FALSE;
  }
  CPDF_Font* pPDFFont = (CPDF_Font*)m_FDE2PDFFont.GetValueAt(pFont);
  if (!pPDFFont) {
    return FALSE;
  }
  wUnicode = (FX_WCHAR)pPDFFont->CharCodeFromUnicode(wUnicode);
  iWidth = pPDFFont->GetCharWidthF(wUnicode);
  return TRUE;
}
CXFA_FontMgr::CXFA_FontMgr() : m_pDefFontMgr(NULL) {}
CXFA_FontMgr::~CXFA_FontMgr() {
  DelAllMgrMap();
}
IFX_Font* CXFA_FontMgr::GetFont(CXFA_FFDoc* hDoc,
                                const CFX_WideStringC& wsFontFamily,
                                uint32_t dwFontStyles,
                                uint16_t wCodePage) {
  uint32_t dwHash = FX_HashCode_String_GetW(wsFontFamily.raw_str(),
                                            wsFontFamily.GetLength(), FALSE);
  CFX_ByteString bsKey;
  bsKey.Format("%u%u%u", dwHash, dwFontStyles, wCodePage);
  auto it = m_FontMap.find(bsKey);
  if (it != m_FontMap.end())
    return it->second;
  CFX_WideString wsEnglishName;
  XFA_LocalFontNameToEnglishName(wsFontFamily, wsEnglishName);
  CXFA_PDFFontMgr* pMgr = (CXFA_PDFFontMgr*)m_PDFFontMgrArray.GetValueAt(hDoc);
  CPDF_Font* pPDFFont = NULL;
  IFX_Font* pFont = NULL;
  if (pMgr) {
    pFont =
        pMgr->GetFont(wsEnglishName.AsWideStringC(), dwFontStyles, &pPDFFont);
    if (pFont)
      return pFont;
  }
  if (!pFont && m_pDefFontMgr) {
    pFont = m_pDefFontMgr->GetFont(hDoc, wsFontFamily, dwFontStyles, wCodePage);
  }
  if (!pFont && pMgr) {
    pPDFFont = NULL;
    pFont = pMgr->GetFont(wsEnglishName.AsWideStringC(), dwFontStyles,
                          &pPDFFont, FALSE);
    if (pFont)
      return pFont;
  }
  if (!pFont && m_pDefFontMgr) {
    pFont = m_pDefFontMgr->GetDefaultFont(hDoc, wsFontFamily, dwFontStyles,
                                          wCodePage);
  }
  if (pFont) {
    if (pPDFFont) {
      pMgr->m_FDE2PDFFont.SetAt(pFont, pPDFFont);
      pFont->SetFontProvider(pMgr);
    }
    m_FontMap[bsKey] = pFont;
  }
  return pFont;
}
void CXFA_FontMgr::LoadDocFonts(CXFA_FFDoc* hDoc) {
  if (!m_PDFFontMgrArray.GetValueAt(hDoc)) {
    m_PDFFontMgrArray.SetAt(hDoc, new CXFA_PDFFontMgr(hDoc));
  }
}
void CXFA_FontMgr::ReleaseDocFonts(CXFA_FFDoc* hDoc) {
  CXFA_PDFFontMgr* pMgr = NULL;
  if (m_PDFFontMgrArray.Lookup(hDoc, (void*&)pMgr)) {
    delete pMgr;
    m_PDFFontMgrArray.RemoveKey(hDoc);
  }
}
void CXFA_FontMgr::DelAllMgrMap() {
  FX_POSITION ps = m_PDFFontMgrArray.GetStartPosition();
  while (ps) {
    CXFA_FFDoc* hDoc = NULL;
    CXFA_PDFFontMgr* pMgr = NULL;
    m_PDFFontMgrArray.GetNextAssoc(ps, (void*&)hDoc, (void*&)pMgr);
    delete pMgr;
  }
  m_PDFFontMgrArray.RemoveAll();
  m_FontMap.clear();
}
void CXFA_FontMgr::SetDefFontMgr(CXFA_DefFontMgr* pFontMgr) {
  m_pDefFontMgr = pFontMgr;
}
