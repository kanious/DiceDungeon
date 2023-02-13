#include "ObjectFactory.h"
#include "GameObject.h"
#include "Layer.h"

#include "BGObject.h"
#include "Player.h"
#include "DefaultCamera.h"


USING(Engine)
USING(glm)
USING(std)

BGObject* ObjectFactory::CreateBGObject(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID,
    vec3 vPos, vec3 vRot, vec3 vScale)
{
    CGameObject* pGameObject = BGObject::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);

    if (nullptr != pGameObject)
        pLayer->AddGameObject(pGameObject);

    return dynamic_cast<BGObject*>(pGameObject);
}

Player* ObjectFactory::CreatePlayer(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID,
    vec3 vPos, vec3 vRot, vec3 vScale)
{
    CGameObject* pGameObject = Player::Create(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale);

    if (nullptr != pGameObject)
        pLayer->AddGameObject(pGameObject);

    return dynamic_cast<Player*>(pGameObject);
}

DefaultCamera* ObjectFactory::CreateCamera(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer,
    vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
{
    CGameObject* pGameObject = DefaultCamera::Create(sTag, lTag, oTag, pLayer, vPos, vRot, vScale, fov, fNear, fFar);

    if (nullptr != pGameObject)
        pLayer->AddGameObject(pGameObject);

    return dynamic_cast<DefaultCamera*>(pGameObject);
}
