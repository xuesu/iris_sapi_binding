
#include "iris_sapi_binding_api.h"

#include <sapi.h>
#include <atlbase.h>
#include <atlstr.h>
#pragma warning (suppress: 4996)
#include <sphelper.h>


IRIS_BIND_API void printAllToken() {
    CComPtr<IEnumSpObjectTokens>   cpEnum;
    HRESULT hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
    if (FAILED(hr))
    {
        printf("Error: SpEnumTokens returns %ld!\n", hr);
        return;
    }
    ULONG ulCount = -1;
    hr = cpEnum->GetCount(&ulCount);
    if (FAILED(hr))
    {
        printf("Error: GetCount returns %ld!\n", hr);
        return;
    }
    CComPtr<ISpObjectToken>        cpVoiceToken = NULL;
    for(ULONG ui = 0;ui < ulCount;ui++)
    {
        hr = cpEnum->Next(1, &cpVoiceToken, NULL);
        if (FAILED(hr))
        {
            printf("Error: cpEnum->Next returns %ld!\n", hr);
            return;
        }
        LPWSTR pwsz = NULL;
        cpVoiceToken->GetId(&pwsz);
        printf("%ls\n", pwsz);
        cpVoiceToken.Release();
    }
}

IRIS_BIND_API CurrentVoiceSettings* init() {
    CurrentVoiceSettings* psettings = new CurrentVoiceSettings();
    if (FAILED(::CoInitialize(NULL)))
        return NULL;
    return psettings;
}

IRIS_BIND_API void uninit(CurrentVoiceSettings* psettings) {
    for (int i = 0; i < IRIS_SAPI_ENABLED_LANG_CNT; i++) {
        if (psettings->cpTokens[i] != NULL) {
            psettings->cpTokens[i].Release();
        }
    }
    free(psettings);
    ::CoUninitialize();
}

bool changeVoice(CurrentVoiceSettings* psettings, ISpVoice* pVoice, int voice_id) {
    if (voice_id < 0 || voice_id >= IRIS_SAPI_ENABLED_LANG_CNT) {
        return false;
    }
    HRESULT hr = 0;
    if (psettings->currentVoiceID != voice_id) {
        if (psettings->cpTokens[voice_id] == NULL) {
            hr = SpGetTokenFromId(IRIS_SAPI_ENABLED_VOICE_ID[voice_id], &psettings->cpTokens[voice_id]);
            if (FAILED(hr))
            {
                printf("Error: SpGetTokenFromId returns %ld!\n", hr);
                return false;
            }
        }
        psettings->currentVoiceID = voice_id;
    }
    hr = pVoice->SetVoice(psettings->cpTokens[voice_id]);
    if (FAILED(hr))
    {
        printf("Error: SetVoice returns %ld!\n", hr);
        return false;
    }
    return true;
}

IRIS_BIND_API bool speak2file(CurrentVoiceSettings* psettings, const char* fname, int voice_id, const wchar_t* text) {
    ISpVoice* pVoice = NULL;
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    if (FAILED(hr))
    {
        printf("Error: Cannot create CLSID_SpVoice Instance\n");
        return NULL;
    }
    if (!changeVoice(psettings, pVoice, voice_id)) {
        printf("Error: Cannot changeVoice to %d\n", voice_id);
        return false;
    }
    CComPtr <ISpStream> stream = NULL;
    hr = stream.CoCreateInstance(CLSID_SpStream);
    if (FAILED(hr))
    {
        printf("Error: Cannot create CLSID_SpStream Instance\n");
        return NULL;
    }
    int fnamelen = strlen(fname) + 1;
    WCHAR* pwfname = new WCHAR[fnamelen];
    MultiByteToWideChar(CP_UTF8, 0, fname, -1, pwfname, fnamelen);

    CSpStreamFormat streamFmt = CSpStreamFormat(SPSF_16kHz16BitMono, &hr);
    if (FAILED(hr))
    {
        printf("Error: SpEnuCSpStreamFormatmTokens returns %ld!\n", hr);
        return NULL;
    }
    hr = stream->BindToFile(pwfname, SPFM_CREATE_ALWAYS, &streamFmt.FormatId(), streamFmt.WaveFormatExPtr(), NULL);
    if (FAILED(hr))
    {
        printf("Error: BindToFile returns %ld!\n", hr);
        return false;
    }
    hr = pVoice->SetOutput(stream, TRUE);
    if (FAILED(hr))
    {
        printf("Error: SetOutput returns %ld!\n", hr);
        return false;
    }
    hr = pVoice->Speak(text, SPF_PURGEBEFORESPEAK, NULL);
    if (FAILED(hr))
    {
        printf("Error: Speak returns %ld!\n", hr);
        return false;
    }
    pVoice->Release();
    delete[] pwfname;
    return true;
}

IRIS_BIND_API bool speak2file_utf8(CurrentVoiceSettings* psettings, const char* fname, int voice_id, const char* utf_8_text) {
    int tlen = strlen(utf_8_text) + 1;
    WCHAR* pwtext = new WCHAR[tlen];
    MultiByteToWideChar(CP_UTF8, 0, utf_8_text, -1, pwtext, tlen);
    speak2file(psettings, fname, voice_id, pwtext);
    delete[] pwtext;
    return true;
}

int main(int argc, char* argv[])
{
    CurrentVoiceSettings* psettings = init();

    speak2file(psettings, "tmp1.wav", IRIS_SAPI_ENABLED_LANG_ZH, L"你好");
    speak2file(psettings, "tmp2.wav", IRIS_SAPI_ENABLED_LANG_JP, L"ありがとうございました。");
    speak2file(psettings, "tmp3.wav", IRIS_SAPI_ENABLED_LANG_ZH, L"谢谢");
    speak2file_utf8(psettings, "tmp4.wav", IRIS_SAPI_ENABLED_LANG_ZH, "hello great");
    uninit(psettings);
    return 0;
}
