#ifndef _AI_H_
#define _AI_H_

#include "GameObject.h"
#include "EngineStruct.h"
#include "Enums.h"

namespace Engine
{
	class CMesh;
	class CInputDevice;
}
class Player;

class AI : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;
	Engine::CInputDevice*		m_pInputDevice;
	eAIType						m_eAIType;
	Player*						m_pTargetPlayer;
	_bool						m_bLeaving;

	_float						m_fSpeed;
	_float						m_fMaxSpeed;
	_float						m_fAccel;

	glm::vec3					m_vOriginPos;


private:
	explicit AI();
	virtual ~AI();

public:
	void SetAI(eAIType type, Player* pPlayer);
private:
	void Seek(const _float& dt);
	void Flee(const _float& dt);
	void Pursue(const _float& dt);
	void Evade(const _float& dt);
	void Approach(const _float& dt);
	_float GetAngle(glm::vec3 vDir1, glm::vec3 vDir2);
	_float safe_acos(_float value);
	void TurnAndMove(glm::vec3 vDir, const _float & dt);
	void CheckBoundary(glm::vec3& vPos);
	void KeyCheck();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
public:
	static AI* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
};


#endif //_AI_H_