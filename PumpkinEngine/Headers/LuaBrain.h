#ifndef _LUABRAIN_H_
#define _LUABRAIN_H_

#include "Lua5.4.4/lua.hpp"
#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CLuaBrain : public CBase
{
	SINGLETON(CLuaBrain)
private:
	lua_State*								m_pLuaState;
	std::string								m_path;
	std::string								m_fileName;
	std::string								m_errorStr;

private:
	explicit CLuaBrain();
	virtual ~CLuaBrain();

public:
	static int Lua_PlaySound(lua_State* L);
	void RunLuaScript(std::string eventTag);

public:
	std::string GetPath()		{ return m_path; }
	std::string GetFileName()	{ return m_fileName; }
	std::string GetErrorStr()	{ return m_errorStr; }
	
private:
	std::string DecodeLuaErrorToString(int error);

public:
	virtual void Destroy();
	RESULT Ready(std::string dataPath);
};

NAMESPACE_END

#endif //_LUABRAIN_H_