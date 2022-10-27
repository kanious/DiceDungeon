#ifndef _TARGETCAMERA_H_
#define _TARGETCAMERA_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CCamera;
	class CTransform;
	class CInputDevice;
}
class ConfigurationManager;

class TargetCamera : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CCamera*			m_pCamera;
	Engine::CTransform*			m_pTarget;

protected:
	explicit TargetCamera();
	virtual ~TargetCamera();
public:
	virtual void Update(const _float&);
	virtual void Destroy();

public:
	void SetTarget(Engine::CTransform*);
private:
	void KeyCheck(const _float&);

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CTransform*);
public:
	static TargetCamera* Create(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CTransform*);
};

#endif //_TARGETCAMERA_H_