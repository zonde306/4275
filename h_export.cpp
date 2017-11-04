/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include <io.h>
#include "extdll.h"
#include "util.h"
#include "cbase.h"

enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;

int(*g_oPrecacheModel)(char* s) = nullptr;
int(*g_oPrecacheSound)(char* s) = nullptr;
void(*g_oSetModel)(edict_t *e, const char *m) = nullptr;
int(*g_oModelIndex)(const char *m) = nullptr;
void(*g_oEmitSound)(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch) = nullptr;
byte*(*g_oLoadFileForMe)(char *filename, int *pLength) = nullptr;
void(*g_oGetGameDir)(char *szGetGameDir) = nullptr;
void(*g_oSetClientKeyValue)(int clientIndex, char *infobuffer, char *key, char *value) = nullptr;

int Hooked_PrecacheModel(char* s);
int Hooked_PrecacheSound(char* s);
void Hooked_SetModel(edict_t *e, const char *m);
int Hooked_ModelIndex(const char *m);
void Hooked_EmitSound(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch);
byte* Hooked_LoadFileForMe(char *filename, int *pLength);
void Hooked_GetGameDir(char *szGetGameDir);
void Hooked_SetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value);

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}

void DLLEXPORT GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

	g_oPrecacheModel = g_engfuncs.pfnPrecacheModel;
	g_oPrecacheSound = g_engfuncs.pfnPrecacheSound;
	g_oSetModel = g_engfuncs.pfnSetModel;
	g_oModelIndex = g_engfuncs.pfnModelIndex;
	g_oEmitSound = g_engfuncs.pfnEmitSound;
	g_oLoadFileForMe = g_engfuncs.pfnLoadFileForMe;
	g_oGetGameDir = g_engfuncs.pfnGetGameDir;
	g_oSetClientKeyValue = g_engfuncs.pfnSetClientKeyValue;

	pengfuncsFromEngine->pfnPrecacheModel = Hooked_PrecacheModel;
	pengfuncsFromEngine->pfnPrecacheSound = Hooked_PrecacheSound;
	pengfuncsFromEngine->pfnSetModel = Hooked_SetModel;
	pengfuncsFromEngine->pfnModelIndex = Hooked_ModelIndex;
	pengfuncsFromEngine->pfnEmitSound = Hooked_EmitSound;
	pengfuncsFromEngine->pfnLoadFileForMe = Hooked_LoadFileForMe;
	pengfuncsFromEngine->pfnGetGameDir = Hooked_GetGameDir;
	pengfuncsFromEngine->pfnSetClientKeyValue = Hooked_SetClientKeyValue;
	g_engfuncs.pfnPrecacheModel = Hooked_PrecacheModel;
	g_engfuncs.pfnPrecacheSound = Hooked_PrecacheSound;
	g_engfuncs.pfnSetModel = Hooked_SetModel;
	g_engfuncs.pfnModelIndex = Hooked_ModelIndex;
	g_engfuncs.pfnEmitSound = Hooked_EmitSound;
	g_engfuncs.pfnLoadFileForMe = Hooked_LoadFileForMe;
	g_engfuncs.pfnGetGameDir = Hooked_GetGameDir;
	g_engfuncs.pfnSetClientKeyValue = Hooked_SetClientKeyValue;
}
#else
extern "C"
{
void GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

	pengfuncsFromEngine->pfnPrecacheModel = Hooked_PrecacheModel;
	pengfuncsFromEngine->pfnPrecacheSound = Hooked_PrecacheSound;
	pengfuncsFromEngine->pfnSetModel = Hooked_SetModel;
	pengfuncsFromEngine->pfnModelIndex = Hooked_ModelIndex;
	pengfuncsFromEngine->pfnEmitSound = Hooked_EmitSound;
	pengfuncsFromEngine->pfnLoadFileForMe = Hooked_LoadFileForMe;
	pengfuncsFromEngine->pfnGetGameDir = Hooked_GetGameDir;
	pengfuncsFromEngine->pfnSetClientKeyValue = Hooked_SetClientKeyValue;
}
}
#endif

int Hooked_PrecacheModel(char * s)
{
	if (strncmp(s, "models/hostage", 14) == 0 ||
		strstr(s, "spetsnaz") != NULL ||
		strstr(s, "militia") != NULL)
	{
		return g_oPrecacheModel("models/hostage.mdl");
	}

	return g_oPrecacheModel(s);
}

int Hooked_PrecacheSound(char * s)
{
	if (strncmp(s, "hostage/", 8) == 0)
	{
		static int i = 0;
		switch (i++ % 5)
		{
		case 0:
			return g_oPrecacheSound("hostage/hos1.wav");
		case 1:
			return g_oPrecacheSound("hostage/hos2.wav");
		case 2:
			return g_oPrecacheSound("hostage/hos3.wav");
		case 3:
			return g_oPrecacheSound("hostage/hos4.wav");
		case 4:
		default:
			return g_oPrecacheSound("hostage/hos5.wav");
		}
	}

	return g_oPrecacheSound(s);
}

void Hooked_SetModel(edict_t * e, const char * m)
{
	if (strncmp(m, "models/hostage", 14) == 0)
	{
		g_oSetModel(e, "models/hostage.mdl");
		return;
	}

	g_oSetModel(e, m);
	return;
}

int Hooked_ModelIndex(const char * m)
{
	if (strncmp(m, "models/hostage", 14) == 0)
		return g_oModelIndex("models/hostage.mdl");

	return g_oModelIndex(m);
}

void Hooked_EmitSound(edict_t * entity, int channel, const char * sample, float volume, float attenuation, int fFlags, int pitch)
{
	if (strncmp(sample, "hostage/", 8) == 0)
	{
		if (strncmp(sample, "hostage/huse/", 13) == 0)
		{
			switch (rand() % 5)
			{
			case 0:
				g_oEmitSound(entity, channel, "hostage/hos1.wav", volume, attenuation, fFlags, pitch);
				break;
			case 1:
				g_oEmitSound(entity, channel, "hostage/hos2.wav", volume, attenuation, fFlags, pitch);
				break;
			case 2:
				g_oEmitSound(entity, channel, "hostage/hos3.wav", volume, attenuation, fFlags, pitch);
				break;
			case 3:
				g_oEmitSound(entity, channel, "hostage/hos4.wav", volume, attenuation, fFlags, pitch);
				break;
			case 4:
			default:
				g_oEmitSound(entity, channel, "hostage/hos5.wav", volume, attenuation, fFlags, pitch);
				break;
			}
		}
		
		return;
	}
	
	g_oEmitSound(entity, channel, sample, volume, attenuation, fFlags, pitch);
	return;
}

char *va(char *format, ...)
{
	va_list     argptr;
	static char string[1024];

	va_start(argptr, format);
	_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	return string;
}

byte * Hooked_LoadFileForMe(char * filename, int * pLength)
{
	int length = strlen(filename);
	if (strcmp(&filename[length - 4], ".nav") == 0)
	{
		int i = length;
		while (i > 0 && filename[i] != '/' && filename[i] != '\\')
			i--;
		if (i > 0)
			i++;
#ifndef __linux__
		if (access(va("czero\\maps\\%s", &filename[i]), 0) != -1)
#else
		if (access(va("czero/maps/%s", &filename[i]), 0) != -1)
#endif
		{
			return g_oLoadFileForMe(va("../czero/maps/%s", &filename[i]), pLength);
		}
	}
	
	return g_oLoadFileForMe(filename, pLength);
}

void Hooked_GetGameDir(char * szGetGameDir)
{
	strcpy(szGetGameDir, "czero");
}

void Hooked_SetClientKeyValue(int clientIndex, char * infobuffer, char * key, char * value)
{
	if (stricmp(key, "model") == 0)
	{
		if (stricmp(value, "spetsnaz") == 0)
		{
			char *models[] = { "urban", "gsg9", "sas", "gign" };
			g_oSetClientKeyValue(clientIndex, infobuffer, key, models[rand() % 4]);
			return;
		}
		else if (stricmp(value, "militia") == 0)
		{
			char *models[] = { "terror", "leet", "arctic", "guerilla" };
			g_oSetClientKeyValue(clientIndex, infobuffer, key, models[rand() % 4]);
			return;
		}
	}
	
	g_oSetClientKeyValue(clientIndex, infobuffer, key, value);
	return;
}
