#include "ObjectFactory.h"
#include "GameObject.h"
#include "Layer.h"

#include "BGObject.h"
#include "Player.h"
#include "PhysicsObject.h"
#include "DefaultCamera.h"


USING(Engine)
USING(glm)
USING(std)

// Create GameObject
CGameObject* ObjectFactory::CreateGameObject(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale)
{
    CGameObject* pGameObject = nullptr;

    switch (lTag)
    {
    case (_uint)LAYER_CAMERA:
    case (_uint)LAYER_STATIC_OBJECT:
    case (_uint)LAYER_TILE:
    case (_uint)LAYER_INTERACTIVE_OBJECT:
    case (_uint)LAYER_UI:
        pGameObject = BGObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
        break;

    case (_uint)LAYER_CHARACTER:
        pGameObject = Player::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, true);
        break;

    case (_uint)LAYER_ENEMY:
        //pGameObject = Player::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
        pGameObject = BGObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
        break;

    case (_uint)LAYER_EVENT_OBJECT:
        pGameObject = PhysicsObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);
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
