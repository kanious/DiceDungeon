#include "pch.h"
#include "../Headers/LuaBrain.h"
#include "../Headers/OpenGLDefines.h"
#include "../Headers/SoundMaster.h"

#include <sstream>
#include <atlconv.h>
#include <fstream>

USING(Engine)
SINGLETON_FUNCTION(CLuaBrain)
USING(std)
USING(glm)

CLuaBrain::CLuaBrain()
	: m_pLuaState(nullptr), m_path(""), m_fileName(""), m_errorStr("")
{
}

CLuaBrain::~CLuaBrain()
{
}

int CLuaBrain::Lua_PlaySound(lua_State* L)
{
	string soundName = lua_tostring(L, 1);

	CSoundMaster::GetInstance()->PlaySound(soundName);

	return 1;
}

void CLuaBrain::RunLuaScript(string eventTag)
{
	luaL_loadfile(m_pLuaState, m_path.c_str());
	lua_pushstring(m_pLuaState, eventTag.c_str());
	lua_setglobal(m_pLuaState, "event");
	lua_pcall(m_pLuaState, 0, 1, 0);
}

string CLuaBrain::DecodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}

	return "Lua: UNKNOWN error";
}

void CLuaBrain::Destroy()
{
	if (nullptr != m_pLuaState)
		lua_close(m_pLuaState);
}

RESULT CLuaBrain::Ready(string dataPath)
{
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);

	lua_pushcfunction(m_pLuaState, Lua_PlaySound);
	lua_setglobal(m_pLuaState, "PlaySound");

	stringstream ss;
	ss << dataPath << "LuaScript.Lua";
	m_path = ss.str();

	return PK_NOERROR;
}
