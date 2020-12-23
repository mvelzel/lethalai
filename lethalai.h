#pragma once

#ifdef LethalAI_EXPORTS
#define LETHALAI_API __declspec(dllexport)
#else
#define LETHALAI_API __declspec(dllimport)
#endif

