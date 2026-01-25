#include "player.h"

#include <string>

#include "config.h"
#include "math/quaternion.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "input/playerConsts.h"
#include "playerColliders.h"

#include "scene/entity.h"
#include "fish.h"

using namespace Math;

void Player::init(Collision::CollisionScene *scene, PlayerData *data, PlayerState *state, int8_t playerNumber)
{
    mScene = scene;
    mPlayerData = data;
    mPlayerState = state;
    mEntityId = GetNextEntityId();

    mCollider = Collision::Collider{
        .entityId = mEntityId,
        .actor = data,
        .type = PlayerColliderType,
        .scale = 1.0f,
        .hasGravity = true,
        .isFixed = false,
        .collisionLayers = static_cast<uint16_t>(Collision::CollisionLayers::CollisionLayerTangible),
        .collisionGroup = static_cast<uint16_t>(FIRST_PLAYER_COLLIDER_GROUP + playerNumber),
    };

    mCollider.center.y = PlayerColliderType.data.cylinder.halfHeight;
    mCollider.recalcBB();

    mScene->add(&mCollider);

    mDamageTrigger = Collision::Collider{
        .entityId = mEntityId,
        .actor = data->getAttackActor(),
        .type = DamageTriggerType,
        .scale = 1.0f,
        .hasGravity = false,
        .isTrigger = true,
        .collisionLayers = static_cast<uint16_t>(Collision::CollisionLayers::CollisionLayerTangible),
        .collisionGroup = static_cast<uint16_t>(FIRST_PLAYER_COLLIDER_GROUP + playerNumber),
    };

    mDamageTrigger.center.y = DamageTriggerType.data.sphere.radius;
    mDamageTrigger.recalcBB();

    mScene->add(&mDamageTrigger, false);
}

Player::~Player()
{
    mScene->remove(&mCollider);
    mScene->remove(&mDamageTrigger);
}

void Player::draw_billboard(T3DViewport &viewport) const
{
    if (mPlayerState->getState() != PlayerStateEnum::STATE_FISHING)
    {
        return;
    }

    // World position
    T3DVec3 localPos = (T3DVec3){{0.0f, BILLBOARD_YOFFSET, 0.0f}};

    // Screen position
    T3DVec3 worldPos = (T3DVec3){{
        mPlayerData->getPosition().x + localPos.v[0] * 0.125f,
        mPlayerData->getPosition().y + localPos.v[1] * 0.125f,
        mPlayerData->getPosition().z + localPos.v[2] * 0.125f,
    }};

    T3DVec3 screenPos;
    t3d_viewport_calc_viewspace_pos(viewport, screenPos, worldPos);

    int x = floorf(screenPos.v[0]);
    int y = floorf(screenPos.v[1]);

    const rdpq_textparms_t param{};
    std::string mBillboardText = mPlayerState->canCatch() ? "HOOKED!" : "Fishing...";
    rdpq_text_printf(&param, Core::FONT_BILLBOARD, x, y, "%s", mBillboardText.c_str());
}