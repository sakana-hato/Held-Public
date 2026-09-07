#include "DxLib.h"
#include "Precompiled.h"
#include "Config.h"
#include "Katana.h"

void Katana::Init(int handle)
{
    attach.SetModel(handle);
    attach.SetScale(Config::Katana::MODEL_SCALE);

    //腰
    {
        MATRIX rot = MMult(MMult(MGetRotX(Config::Katana::WAIST_PITCH_RAD),MGetRotY(Config::Katana::WAIST_YAW_RAD)),MGetRotZ(Config::Katana::WAIST_ROLL_RAD));

        MATRIX tra = MGetTranslate(VGet(Config::Katana::WAIST_OFFSET_X,Config::Katana::WAIST_OFFSET_Y,Config::Katana::WAIST_OFFSET_Z));

        waistOffset = MMult(rot, tra);
    }

    //手
    {
        MATRIX rot = MMult(MMult(MGetRotX(Config::Katana::HAND_PITCH_RAD),MGetRotY(Config::Katana::HAND_YAW_RAD)),MGetRotZ(Config::Katana::HAND_ROLL_RAD));

        MATRIX tra = MGetTranslate(VGet(Config::Katana::HAND_OFFSET_X,Config::Katana::HAND_OFFSET_Y,Config::Katana::HAND_OFFSET_Z));

        handOffset = MMult(rot, tra);
    }
}

void Katana::Update(int ownerModel)
{
	attach.SetOffset(drawn ? handOffset : waistOffset);
	attach.Update(ownerModel, drawn ? handFrame : waistFrame);
}

void Katana::Draw() const
{
	attach.Draw();
}

Capsule Katana::GetBladeCapsule() const
{
    if (!drawn)
    {
        const VECTOR zero = VGet(0.0f, 0.0f, 0.0f);
        return Capsule{ zero, zero, 0.0f };
    }

    //刃カプセルの補正行列（回転→平行移動）
    MATRIX rot = MMult(MMult(MGetRotX(Config::Katana::BLADE_PITCH_RAD),MGetRotY(Config::Katana::BLADE_YAW_RAD)),MGetRotZ(Config::Katana::BLADE_ROLL_RAD));

    MATRIX ofs = MMult(rot, MGetTranslate(VGet(Config::Katana::BLADE_OFFSET_X,Config::Katana::BLADE_OFFSET_Y,Config::Katana::BLADE_OFFSET_Z)));

    //補正込みの刀ワールド行列
    const MATRIX mat = MMult(ofs, attach.GetWorldMatrix());

    //刃の根元・切っ先をローカルで作り、補正込み行列で変換
    const VECTOR root = VTransform(VGet(0.0f, 0.0f, Config::Katana::BLADE_ROOT_Z), mat);
    const VECTOR tip = VTransform(VGet(0.0f, 0.0f, Config::Katana::BLADE_TIP_Z), mat);

    return Capsule{ root, tip, Config::Katana::BLADE_RADIUS };
}