#pragma once
#include <sapi.h>
#include <atlbase.h>
#include <atlstr.h>

const int IRIS_SAPI_ENABLED_LANG_EN = 0;
const int IRIS_SAPI_ENABLED_LANG_DE = 1;
const int IRIS_SAPI_ENABLED_LANG_ZH = 2;
const int IRIS_SAPI_ENABLED_LANG_JP = 3;
const int IRIS_SAPI_ENABLED_LANG_CNT = 4;

const wchar_t IRIS_SAPI_ENABLED_VOICE_ID_PREFIX[IRIS_SAPI_ENABLED_LANG_CNT][100] = {
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_EN-US",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_DE-DE",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_ZH-CN",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_JA-JP" };
const wchar_t IRIS_SAPI_ENABLED_VOICE_ID[IRIS_SAPI_ENABLED_LANG_CNT][100] = {
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_EN-US_DAVID_11.0",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_DE-DE_HEDDA_11.0",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_ZH-CN_HUIHUI_11.0",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\TTS_MS_JA-JP_HARUKA_11.0" };

struct CurrentVoiceSettings {
    int currentVoiceID = -1;
    CComPtr<ISpObjectToken> cpTokens[IRIS_SAPI_ENABLED_LANG_CNT] = { 0, 0, 0, 0 };

};

#define IRIS_BIND_API __declspec(dllexport)

#ifdef __cplusplus
extern "C"
#endif
IRIS_BIND_API void printAllToken();

#ifdef __cplusplus
extern "C"
#endif
IRIS_BIND_API CurrentVoiceSettings * init();

#ifdef __cplusplus
extern "C"
#endif 
IRIS_BIND_API void uninit(CurrentVoiceSettings * psettings);

#ifdef __cplusplus
extern "C"
#endif 
IRIS_BIND_API bool speak2file(CurrentVoiceSettings * psettings, const char* fname, int voice_id, const wchar_t* text);

#ifdef __cplusplus
extern "C"
#endif 
IRIS_BIND_API bool speak2file_utf8(CurrentVoiceSettings * psettings, const char* fname, int voice_id, const char* utf_8_text);