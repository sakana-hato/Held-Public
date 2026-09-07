#include "DxLib.h"
#include "Precompiled.h"
#include "GamepadDevice.h"

GamepadDevice::GamepadDevice()
{
	BuildButtonMap();
}

void GamepadDevice::Update()
{
	prev = curr;
	GetJoypadXInputState(DX_INPUT_PAD1, &curr);
}

bool GamepadDevice::IsHeld(InputAction action)const
{
	return BtnHeld(buttonMap[static_cast<size_t>(action)]);
}

bool GamepadDevice::IsPressed(InputAction action)const
{
	return BtnPressed(buttonMap[static_cast<size_t>(action)]);
}

bool GamepadDevice::IsReleased(InputAction action)const
{
	return BtnReleased(buttonMap[static_cast<size_t>(action)]);
}

float GamepadDevice::GetAxisX()const
{
	const float raw = static_cast<float>(curr.ThumbLX) / STICK_MAX_RAW;
	return ApplyDeadzone(raw);
}

float GamepadDevice::GetAxisY()const
{
	const float raw = static_cast<float>(curr.ThumbLY) / STICK_MAX_RAW;
	return ApplyDeadzone(raw);

}

float GamepadDevice::GetCamAxisX()const
{
	const float raw = static_cast<float>(curr.ThumbRX) / STICK_MAX_RAW;
	return ApplyDeadzone(raw);
}

float GamepadDevice::GetCamAxisY()const
{
	const float raw = static_cast<float>(curr.ThumbRY) / STICK_MAX_RAW;
	return ApplyDeadzone(raw);
}

void GamepadDevice::SetVibration(int leftPower, int rightPower)
{
	StartJoypadVibration(DX_INPUT_PAD1, leftPower, rightPower);
}

void GamepadDevice::StopVibration()
{
	StopJoypadVibration(DX_INPUT_PAD1,0);
}

void GamepadDevice::BuildButtonMap()
{
	using IA = InputAction;
	auto& m = buttonMap;

    // XINPUT_BUTTON* 定数を使用
    m[static_cast<size_t>(IA::Jump)]            = XINPUT_BUTTON_A;
    m[static_cast<size_t>(IA::Dodge)]           = XINPUT_BUTTON_B;
    m[static_cast<size_t>(IA::Attack)]          = XINPUT_BUTTON_X;
    m[static_cast<size_t>(IA::Magic)]           = XINPUT_BUTTON_Y;
    m[static_cast<size_t>(IA::Ultimate)]        = XINPUT_BUTTON_RIGHT_SHOULDER;
    m[static_cast<size_t>(IA::TargetLock)]      = XINPUT_BUTTON_LEFT_SHOULDER;
    m[static_cast<size_t>(IA::TargetSwitch)]    = XINPUT_BUTTON_RIGHT_THUMB;
    m[static_cast<size_t>(IA::Dash)]            = XINPUT_BUTTON_LEFT_THUMB;
    m[static_cast<size_t>(IA::Pause)]           = XINPUT_BUTTON_START;
    m[static_cast<size_t>(IA::ToggleCamera)]    = XINPUT_BUTTON_BACK;
    m[static_cast<size_t>(IA::Confirm)]         = XINPUT_BUTTON_A;
    m[static_cast<size_t>(IA::Interact)]        = XINPUT_BUTTON_A;
    m[static_cast<size_t>(IA::Skip)]            = XINPUT_BUTTON_A;
    m[static_cast<size_t>(IA::Cancel)]          = XINPUT_BUTTON_B;

    // 移動・カメラはスティック軸なのでボタンマップは使わない）
    m[static_cast<size_t>(IA::MoveForward)] = 0;
    m[static_cast<size_t>(IA::MoveBack)]    = 0;
    m[static_cast<size_t>(IA::MoveLeft)]    = 0;
    m[static_cast<size_t>(IA::MoveRight)]   = 0;
    m[static_cast<size_t>(IA::CamUp)]       = 0;
    m[static_cast<size_t>(IA::CamDown)]     = 0;
    m[static_cast<size_t>(IA::CamLeft)]     = 0;
    m[static_cast<size_t>(IA::CamRight)]    = 0;
}

float  GamepadDevice::ApplyDeadzone(float raw)const
{
    if (std::fabs(raw) < STICK_DEADZONE)
    {
        return 0.0f;
    }

    //デットゾーン再マップ
    const float sign        = (raw > 0.0f) ? 1.0f : -1.0f;
    const float adjusted    = (std::fabs(raw) - STICK_DEADZONE) / (1.0f - STICK_DEADZONE);
    return sign * adjusted;
}