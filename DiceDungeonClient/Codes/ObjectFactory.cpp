#include "ObjectFactory.h"
#include "GameObject.h"
#include "Layer.h"

#include "BGObject.h"
#include "Player.h"
#include "Enemy.h"
#include "PhysicsObject.h"
#include "DefaultCamera.h"
#include "SceneDungeon.h"


USING(Engine)
USING(glm)
USING(std)

// Create GameObject
CGameObject* ObjectFactory::CreateGameObject(SceneDungeon* pScene, _uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale)
{
    CGameObject* pGameObject = nullptr;

    switch (lTag)
    {
    case (_uint)LAYER_CHARACTER:
        pGameObject = Player::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, pScene);
        dynamic_cast<Player*>(pGameObject)->SetScene(pScene);
        break;

    case (_uint)LAYER_ENEMY:
        pGameObject = Enemy::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, pScene);
        dynamic_cast<Enemy*>(pGameObject)->SetScene(pScene);
        break;

    case (_uint)LAYER_EVENT_OBJECT:
        pGameObject = PhysicsObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
        break;

    default:
        pGameObject = BGObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
        break;
    }

    if (nullptr != pGameObject)
        pLayer->AddGameObject(pGameObject);

    return pGameObject;
}

// Create camera
DefaultCamera* ObjectFactory::CreateCamera(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer,
    vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
{
    CGameObject* pGameObject = DefaultCamera::Create(sTag, lTag, oTag, pLayer, vPos, vRot, vScale, fov, fNear, fFar);

    if (nullptr != pGameObject)
    {
        pLayer->AddGameObject(pGameObject);
        return dynamic_cast<DefaultCamera*>(pGameObject);
    }

    return nullptr;
}
