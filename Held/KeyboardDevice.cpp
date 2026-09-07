#include "DxLib.h"
#include "KeyboardDevice.h"

KeyboardDevice::KeyboardDevice()
{
	BuildKeyMap();
}

void KeyboardDevice::Update()
{
	prev = curr;
	GetHitKeyStateAll(curr.data());

	mousePrev = mouseCurr;           
	mouseCurr = GetMouseInput();
}

bool KeyboardDevice::IsHeld(InputAction action) const
{
	if (action == InputAction::Attack)
	{
		return (mouseCurr & MOUSE_INPUT_LEFT) != 0;
	}

	if (action == InputAction::Dodge)
	{
		return (mouseCurr & MOUSE_INPUT_RIGHT) && !(mousePrev & MOUSE_INPUT_RIGHT);
	}

	return KeyHeld(keyMap[static_cast<size_t>(action)]);
}

bool KeyboardDevice::IsPressed(InputAction action) const
{
	if (action == InputAction::Attack)
	{
		return (mouseCurr & MOUSE_INPUT_LEFT) && !(mousePrev & MOUSE_INPUT_LEFT);
	}

	if (action == InputAction::Dodge)  
	{
		return (mouseCurr & MOUSE_INPUT_RIGHT) && !(mousePrev & MOUSE_INPUT_RIGHT);
	}

	return KeyPressed(keyMap[static_cast<size_t>(action)]);
}

bool KeyboardDevice::IsReleased(InputAction action) const
{
	if (action == InputAction::Attack)
	{
		return !(mouseCurr & MOUSE_INPUT_LEFT) && (mousePrev & MOUSE_INPUT_LEFT);
	}

	if (action == InputAction::Dodge)
	{
		return (mouseCurr & MOUSE_INPUT_RIGHT) && !(mousePrev & MOUSE_INPUT_RIGHT);
	}


	return KeyReleased(keyMap[static_cast<size_t>(action)]);
}

float KeyboardDevice::GetAxisX()const
{
	float x = 0.0f;
	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::MoveRight)]))
	{
		x += 1.0f;
	}

	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::MoveLeft)]))
	{
		x -= 1.0f;
	}

	return x;
}

float KeyboardDevice::GetAxisY()const
{
	float y = 0.0f;
	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::MoveForward)]))
	{
		y += 1.0f;
	}

	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::MoveBack)]))
	{
		y -= 1.0f;
	}
	return y;
}

float KeyboardDevice::GetCamAxisX()const
{
	float x = 0.0f;
	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::CamRight)]))
	{
		x += 1.0f;
	}

	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::CamLeft)]))
	{
		x -= 1.0f;
	}

	return x;
}

float KeyboardDevice::GetCamAxisY()const
{
	float y = 0.0f;
	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::CamUp)]))
	{
		y += 1.0f;
	}

	if (KeyHeld(keyMap[static_cast<size_t>(InputAction::CamDown)]))
	{
		y -= 1.0f;
	}
	return y;
}

void KeyboardDevice::BuildKeyMap()
{
	using IA = InputAction;
	auto& m = keyMap;

	// 移動
	m[static_cast<size_t>(IA::MoveForward)]		= KEY_INPUT_W;
	m[static_cast<size_t>(IA::MoveBack)]		= KEY_INPUT_S;
	m[static_cast<size_t>(IA::MoveLeft)]		= KEY_INPUT_A;
	m[static_cast<size_t>(IA::MoveRight)]		= KEY_INPUT_D;

	// カメラ（方向キー）
	m[static_cast<size_t>(IA::CamUp)]			= KEY_INPUT_UP;
	m[static_cast<size_t>(IA::CamDown)]			= KEY_INPUT_DOWN;
	m[static_cast<size_t>(IA::CamLeft)]			= KEY_INPUT_LEFT;
	m[static_cast<size_t>(IA::CamRight)]		= KEY_INPUT_RIGHT;

	// アクション
	m[static_cast<size_t>(IA::Attack)]			= KEY_INPUT_Z;
	m[static_cast<size_t>(IA::Magic)]			= KEY_INPUT_X;
	m[static_cast<size_t>(IA::Ultimate)]		= KEY_INPUT_C;
	m[static_cast<size_t>(IA::Jump)]			= KEY_INPUT_SPACE;
	m[static_cast<size_t>(IA::Dash)]			= KEY_INPUT_LSHIFT;
	m[static_cast<size_t>(IA::Dodge)]			= KEY_INPUT_I;
	m[static_cast<size_t>(IA::Interact)]		= KEY_INPUT_F;
	m[static_cast<size_t>(IA::Skip)]			= KEY_INPUT_E;

	// ターゲット
	m[static_cast<size_t>(IA::TargetLock)]		= KEY_INPUT_TAB;
	m[static_cast<size_t>(IA::TargetSwitch)]	= KEY_INPUT_TAB;

	// カメラ切り替え
	m[static_cast<size_t>(IA::ToggleCamera)]	= KEY_INPUT_V;

	// システム
	m[static_cast<size_t>(IA::Pause)]			= KEY_INPUT_ESCAPE;
	m[static_cast<size_t>(IA::Confirm)]			= KEY_INPUT_RETURN;
	m[static_cast<size_t>(IA::Cancel)]			= KEY_INPUT_BACK;
}