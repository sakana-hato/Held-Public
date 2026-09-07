#include "DxLib.h"
#include "InputSystem.h"
#include "Precompiled.h"

InputSystem::InputSystem()
	:keyboard(std::make_unique<KeyboardDevice>())
	,pad(std::make_unique<GamepadDevice>())
{
}

void InputSystem::Update()
{
	keyboard->Update();
	pad->Update();
}

bool InputSystem::IsHeld(InputAction action)const
{
    //どちらで操作していても同じように扱えるようOR結合する
	return keyboard->IsHeld(action) || pad->IsHeld(action);
}

bool InputSystem::IsPressed(InputAction action)const
{
	return keyboard->IsPressed(action) || pad->IsPressed(action);
}

bool InputSystem::IsReleased(InputAction action)const
{
	return keyboard->IsReleased(action) || pad->IsReleased(action);
}

float InputSystem::GetMoveX() const
{
    // パッドの値が 0 のときだけキーボードを使う
    const float padVal = pad->GetAxisX();

    if (std::fabs(padVal) > 0.0f)
    { 
        return padVal;
    }
    return keyboard->GetAxisX();
}

float InputSystem::GetMoveY() const
{
    const float padVal = pad->GetAxisY();
    if (std::fabs(padVal) > 0.0f)
    {
        return padVal;
    }

    return keyboard->GetAxisY();
}

float InputSystem::GetCamX() const
{
    const float padVal = pad->GetCamAxisX();
    if (std::fabs(padVal) > 0.0f) 
    {
        return padVal; 
    }
    return keyboard->GetCamAxisX();
}

float InputSystem::GetCamY() const
{
    const float padVal = pad->GetCamAxisY();
    if (std::fabs(padVal) > 0.0f)
    {
        return padVal;
    }
    return keyboard->GetCamAxisY();
}

bool InputSystem::IsPadConnected() const
{
    // パッドが返す値が全部 0 なら未接続
    XINPUT_STATE state = {};
    GetJoypadXInputState(DX_INPUT_PAD1, &state);
    return (state.Buttons != 0 ||state.ThumbLX != 0 || state.ThumbLY != 0 ||state.ThumbRX != 0 || state.ThumbRY != 0);
}