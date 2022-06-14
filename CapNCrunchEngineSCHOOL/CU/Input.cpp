#include "Input.h"
#include "pch.h"
#include "framework.h"

using namespace CommonUtilities;

//KeyBoard stuff
bool Keyboard::KeyIsPressed(unsigned char aKeyCode) const
{
	return (myKeyStates[1][aKeyCode]) != 0 &&
		(myKeyStates[0][aKeyCode]) == 0;
}

bool Keyboard::KeyIsDown(unsigned char aKeyCode) const
{
	return (myKeyStates[1][aKeyCode]) != 0;
}

bool Keyboard::KeyIsReleased(unsigned char aKeyCode) const
{
	return (myKeyStates[1][aKeyCode]) == 0 &&
		(myKeyStates[0][aKeyCode]) != 0;
}

void Keyboard::OnKeyPressed(unsigned char aKeyCode)
{
	myKeyStates[1][aKeyCode] = true;
}

void Keyboard::OnKeyReleased(unsigned char aKeyCode)
{
	myKeyStates[1][aKeyCode] = false;
}

void Keyboard::ClearState()
{
	myKeyStates[1].reset();
}
//End Of KeyBord stuff

//Mouse Stuff
Point Mouse::GetPos() const
{
	return myPos;
}

Point Mouse::GetAbsPos() const
{
	return myPos;
}

int Mouse::GetPosX() const
{
	return myPos.myX;
}

int Mouse::GetPosY() const
{
	return myPos.myY;
}

void Mouse::SetPos(const int aX, const int aY)
{
	SetPos(aX, aY);
}

void Mouse::SetPosX(const int aX)
{
	SetPosX(aX);
}

void Mouse::SetPosY(const int aY)
{
	SetPosY(aY);
}

void Mouse::CaptureMouse()
{
	if(!myIsInWindow)
		OnMouseEnter();

	SetCapture(Input::GetHwnd()); //Capture the mouse in the window
}

bool Mouse::IsInWindow() const
{
	return myIsInWindow;
}

bool Mouse::LeftIsPressed() const
{
	return myLeftIsPressed;
}

bool Mouse::RightIsPressed() const
{
	return myRightIsPressed;
}

bool Mouse::MiddleIsPressed() const
{
	return myMiddleIsPressed;
}

bool Mouse::XOneIsPressed() const
{
	return myXOneIsPressed;
}

bool Mouse::XTwoIsPressed() const
{
	return myXTwoIsPressed;
}

Mouse::MouseEvent Mouse::Read()
{
	if (myBuffer.size() > 0u)
	{
		Mouse::MouseEvent e = myBuffer.front();
		myBuffer.pop();
		return e;
	}
	else
	{
		return Mouse::MouseEvent();
	}
}

void Mouse::Flush()
{
	myBuffer = std::queue<MouseEvent>();
}

void Mouse::OnMouseMove(const int aX, const int aY)
{
	myPos.myX = aX;
	myPos.myY = aY;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave()
{
	myIsInWindow = false;
	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter()
{
	myIsInWindow = true;
	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(const int aX, const int aY)
{
	myLeftIsPressed = true;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(const int aX, const int aY)
{
	myLeftIsPressed = false;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(const int aX, const int aY)
{
	myRightIsPressed = true;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(const int aX, const int aY)
{
	myRightIsPressed = false;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(const int aX, const int aY)
{
	myMiddleIsPressed = true;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::MPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(const int aX, const int aY)
{
	myMiddleIsPressed = false;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnXOnePressed(const int aX, const int aY)
{
	myXOneIsPressed = true;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::XOnePress, *this));
	TrimBuffer();
}

void Mouse::OnXOneReleased(const int aX, const int aY)
{
	myXOneIsPressed = false;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::XOneRelease, *this));
	TrimBuffer();
}

void Mouse::OnXTwoPressed(const int aX, const int aY)
{
	myXTwoIsPressed = true;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::XTwoPress, *this));
	TrimBuffer();
}

void Mouse::OnXTwoReleased(const int aX, const int aY)
{
	myXTwoIsPressed = false;

	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::XTwoRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(const int aX, const int aY)
{
	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(const int aX, const int aY)
{
	myBuffer.emplace(Mouse::MouseEvent(Mouse::MouseEvent::MouseType::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(const int aX, const int aY, const int someDelta)
{
	myWheelDeltaCarry += someDelta;

	while (myWheelDeltaCarry >= WHEEL_DELTA)
	{
		myWheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(aX, aY);
	}
	while (myWheelDeltaCarry <= -WHEEL_DELTA)
	{
		myWheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(aX, aY);
	}
}

void Mouse::TrimBuffer()
{
	while (myBuffer.size() > myBufferSize)
	{
		myBuffer.pop();
	}
}
//End of mouse stuff

HWND Input::myHwnd = NULL;

//Input Stuff
void Input::Init(HWND someHwnd)
{
	myHwnd = someHwnd;

	int mouseX = 0;
	int mouseY = 0;

	RECT rect;
	if (GetWindowRect(someHwnd, &rect))
	{
		myWindowWidth = rect.right - rect.left;
		myWindowHeight = rect.bottom - rect.top;

		mouseX = myWindowWidth / 2;
		mouseY = myWindowHeight / 2;
	}

	myMouse.OnMouseMove(mouseX, mouseY);
}

bool Input::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		myKeyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		myKeyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < myWindowWidth && pt.y >= 0 && pt.y < myWindowHeight)
		{
			myMouse.OnMouseMove(pt.x, pt.y);
			if (!myMouse.IsInWindow())
			{
				myMouse.CaptureMouse();
				myMouse.OnMouseEnter();
			}
			else
			{
				ReleaseCapture();
				myMouse.OnMouseLeave();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				myMouse.OnMouseMove(pt.x, pt.y);
			}
		}
		myMouse.OnMouseMove(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnMiddlePressed(pt.x, pt.y);
		break;
	}
	case WM_XBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		if(wParam == MK_XBUTTON1)
			myMouse.OnXOnePressed(pt.x, pt.y);
		else if (wParam == MK_XBUTTON2)
			myMouse.OnXTwoPressed(pt.x, pt.y);

		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		myMouse.OnMiddleReleased(pt.x, pt.y);
		break;
	}
	case WM_XBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		if (wParam == MK_XBUTTON1)
			myMouse.OnXOneReleased(pt.x, pt.y);
		else if (wParam == MK_XBUTTON2)
			myMouse.OnXTwoReleased(pt.x, pt.y);

		break;
	}
	case WM_MOUSEHWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		myMouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	case WM_CHAR:
	{
		//Char input here
		//break;
	}
	default:
		return false;
	}

	return true;
}

void Input::EndFrame()
{
	myKeyboard.myKeyStates[0] = myKeyboard.myKeyStates[1];
}

Mouse& Input::GetMouse()
{
	return myMouse;
}

Keyboard& Input::GetKeyboard()
{
	return myKeyboard;
}
