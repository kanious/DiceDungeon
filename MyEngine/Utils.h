#ifndef _UTILS_H_
#define _UTILS_H_

#include "Defines.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API Utils
{
public:
	static _wchar_t* UUIDTest()
	{
		UUID uuid;
		RPC_STATUS ret_val = ::I_UuidCreate(&uuid);

		_wchar_t* wUuid = NULL;
		if (ret_val == RPC_S_OK)
			::UuidToStringW(&uuid, (RPC_WSTR*)&wUuid);
		return wUuid;
	}
};

NAMESPACE_END

#endif //_UTILS_H_