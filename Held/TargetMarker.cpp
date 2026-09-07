#include "DxLib.h"
#include "Precompiled.h"
#include "TargetMarker.h"
#include "Config.h"

void TargetMarker::Init(int modelHandle, float scale)
{
    _modelHandle = modelHandle;
    _scale = scale;
}

void TargetMarker::Update(float dt)
{
    _time += dt;
}

void TargetMarker::Draw(const VECTOR& targetHeadPos) const
{
    if (_modelHandle < 0)
    {
        return;
    }

    //ã‰º‚Ì”g—h‚êisin‚Åã‰ºj
    const float bob = std::sin(_time * Config::Target::BOB_SPEED) * Config::Target::BOB_HEIGHT;

    //•\Ž¦ˆÊ’u‘ÎÛ‚Ì“ªã{—h‚ê
    VECTOR pos = targetHeadPos;
    pos.y += Config::Target::MARKER_HEIGHT + bob;

    //YŽ²‚Å‰ñ“]
    const float yaw = _time * Config::Target::ROTATE_SPEED;

    MV1SetPosition(_modelHandle, pos);
    MV1SetRotationXYZ(_modelHandle, VGet(0.0f, yaw, 0.0f));
    MV1SetScale(_modelHandle, VGet(_scale, _scale, _scale));
    MV1DrawModel(_modelHandle);
}