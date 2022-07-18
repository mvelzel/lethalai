#pragma once

#ifdef LethalAI_EXPORTS
#define LETHALAI_API __declspec(dllexport)
#else
#define LETHALAI_API __declspec(dllimport)
#endif

#define DLLExport __declspec(dllexport)

DLLExport void TestFunction(void* args);

DLLExport void ExportInit(void* args);

DLLExport void ExportReset(void* args);

DLLExport void ExportStep(void* args);
