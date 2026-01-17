#include "player.h"

#include <string>

#include "../config.h"

#include "../math/quaternion.h"

#include "entity.h"
#include "fish.h"

namespace Fishing
{
    void Player::init(Collision::Scene *scene, PlayerData *data, PlayerState *state, int8_t playerNumber)
    {
        mScene = scene;
        mPlayerData = data;
        mPlayerState = state;
        mEntityId = GetNextEntityId();

        mCollider = Collision::Collider{
            .entityId = mEntityId,
            .position = &data->position,
            .rotation = &data->rotation,
            .velocity = &data->velocity,
            .type = PlayerColliderType,
            .scale = 1.0f,
            .hasGravity = true,
            .isFixed = false,
            .collisionLayers = CollisionLayerTangible,
            .collisionGroup = uint16_t(FIRST_PLAYER_COLLIDER_GROUP + playerNumber),
        };

        mCollider.center.y = PlayerColliderType.data.cylinder.halfHeight;
        mCollider.recalcBB();

        mScene->add(&mCollider);
    }

    Player::~Player()
    {
        mScene->remove(&mCollider);
    }

    void Player::draw_billboard(T3DViewport &viewport) const
    {
        if (mPlayerState->getState() != STATE_FISHING)
        {
            return;
        }

        // World position
        T3DVec3 localPos = (T3DVec3){{0.0f, BILLBOARD_YOFFSET, 0.0f}};

        // Screen position
        T3DVec3 worldPos = (T3DVec3){{
            mPlayerData->position.x + localPos.v[0] * 0.125f,
            mPlayerData->position.y + localPos.v[1] * 0.125f,
            mPlayerData->position.z + localPos.v[2] * 0.125f,
        }};

        T3DVec3 screenPos;
        t3d_viewport_calc_viewspace_pos(viewport, screenPos, worldPos);

        int x = floorf(screenPos.v[0]);
        int y = floorf(screenPos.v[1]);

        const rdpq_textparms_t param{};
        std::string mBillboardText = mPlayerState->canCatch() ? "HOOKED!" : "Fishing...";
        rdpq_text_printf(&param, FONT_BILLBOARD, x, y, "%s", mBillboardText.c_str());
    }

    void Player::get_attack_position(Vector2 &attack_pos) const
    {
        attack_pos.x = mPlayerData->position.x + mPlayerData->rotation.x * ATTACK_OFFSET;
        attack_pos.y = mPlayerData->position.z + mPlayerData->rotation.y * ATTACK_OFFSET;
    }
}