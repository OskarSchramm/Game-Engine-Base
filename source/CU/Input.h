#pragma once
#include <Windows.h>
#include <bitset>
#include <queue>

namespace CommonUtilities
{
	struct Point
	{
		int myX;
		int myY;
		Point() : myX(0), myY(0) {}
		Point(int aX, int aY) : myX(aX), myY(aY) {}
	};

	class Keyboard
	{
		friend class Input;
	public:
		enum class KeyboardType
		{
			Press,
			Release,
			Invalid
		};
		class KeyboardEvent
		{
		public:
			KeyboardEvent() : myType(KeyboardType::Invalid), myKeyCode(0u) {}
			KeyboardEvent(KeyboardType aType, unsigned char aKeyCode) : myType(aType), myKeyCode(aKeyCode) {}

			inline bool IsPress() const { return myType == KeyboardType::Press; }
			inline bool IsRelease() const { return myType == KeyboardType::Release; }
			inline bool IsValid() const { return myType != KeyboardType::Invalid; }
			inline unsigned char GetKeyCode() const { return myKeyCode; }

		private:
			KeyboardType myType;
			unsigned char myKeyCode;
		};
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;

		bool KeyIsPressed(unsigned char aKeyCode) const;
		bool KeyIsDown(unsigned char aKeyCode) const;
		bool KeyIsReleased(unsigned char aKeyCode) const;

		bool KeyIsEmpty() const;
		void ClearState();
	private:
		void OnKeyPressed(unsigned char aKeyCode);
		void OnKeyReleased(unsigned char aKeyCode);

		static constexpr unsigned int myNKeys = 256u;
		std::bitset<myNKeys> myKeyStates[2];
	};

	class Mouse
	{
		friend class Input;
	public:
		class MouseEvent
		{
		public:
			enum class MouseType
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				MPress,
				MRelease,
				XOnePress,
				XOneRelease,
				XTwoPress,
				XTwoRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
				Invalid
			};
			MouseEvent() : myType(MouseType::Invalid), myLeftIsPressed(false), myRightIsPressed(false), myMiddleIsPressed(false), myXOneIsPressed(false), myXTwoIsPressed(false) {}
			MouseEvent(MouseType aType, const Mouse& aParent) : myType(aType), myPos(aParent.GetPosX(), aParent.GetPosY()),
				myLeftIsPressed(aParent.myLeftIsPressed), myRightIsPressed(aParent.myRightIsPressed), myMiddleIsPressed(aParent.myMiddleIsPressed),
				myXOneIsPressed(aParent.myXOneIsPressed), myXTwoIsPressed(aParent.myXTwoIsPressed) {}

			inline bool IsValid() const { return myType != MouseType::Invalid; }
			MouseType GetType() const { return myType; }
			inline Point GetPos() const { return myPos; }
			inline void SetPos(const int aX, const int aY) { myPos.myX = aX; myPos.myY = aY; }
			inline int GetX() const { return myPos.myX; }
			inline int GetY() const { return myPos.myY; }

			//inline int GetDeltaX() const { return (myPos[Input::GetCurrentFrame()].myX - myPos[!Input::GetCurrentFrame()].myX); }
			//inline int GetDeltaY() const { return (myPos[Input::GetCurrentFrame()].myY - myPos[!Input::GetCurrentFrame()].myY); }
			//inline Point GetDeltaPos() const { return Point(GetDeltaX(), GetDeltaY());}

			inline bool LeftIsPressed() const { return myLeftIsPressed; }
			inline bool RightIsPressed() const { return myRightIsPressed; }
			inline bool MiddleIsPressed() const { return myMiddleIsPressed; }
			inline bool XOneIsPressed() const { return myXOneIsPressed; }
			inline bool XTwoIsPressed() const { return myXTwoIsPressed; }

		private:
			MouseType myType;
			Point myPos;
			bool myLeftIsPressed;
			bool myRightIsPressed;
			bool myMiddleIsPressed;
			bool myXOneIsPressed;
			bool myXTwoIsPressed;
		};

		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;

		Point GetPos() const;
		Point GetAbsPos() const;
		int GetPosX() const;
		int GetPosY() const;

		void SetPos(const int aX, const int aY);
		void SetPosX(const int aX);
		void SetPosY(const int aY);

		bool IsInWindow() const;
		void CaptureMouse();
		bool LeftIsPressed() const;
		bool RightIsPressed() const;
		bool MiddleIsPressed() const;
		bool XOneIsPressed() const;
		bool XTwoIsPressed() const;
		Mouse::MouseEvent Read();
		inline bool IsEmpty() const { return myBuffer.empty(); }
		void Flush();
	private:
		void OnMouseMove(const int aX, const int aY);
		void OnMouseLeave();
		void OnMouseEnter();
		void OnLeftPressed(const int aX, const int aY);
		void OnLeftReleased(const int aX, const int aY);
		void OnRightPressed(const int aX, const int aY);
		void OnRightReleased(const int aX, const int aY);
		void OnMiddlePressed(const int aX, const int aY);
		void OnMiddleReleased(const int aX, const int aY);
		void OnXOnePressed(const int aX, const int aY);
		void OnXOneReleased(const int aX, const int aY);
		void OnXTwoPressed(const int aX, const int aY);
		void OnXTwoReleased(const int aX, const int aY);
		void OnWheelUp(const int aX, const int aY);
		void OnWheelDown(const int aX, const int aY);
		void OnWheelDelta(const int aX, const int aY, const int someDelta);
		void TrimBuffer();

		Point myPos;
		bool myLeftIsPressed = false;
		bool myRightIsPressed = false;
		bool myMiddleIsPressed = false;
		bool myXOneIsPressed = false;
		bool myXTwoIsPressed = false;
		bool myIsInWindow = false;
		int myWheelDeltaCarry = 0;

		static constexpr unsigned int myBufferSize = 16u;
		std::queue<MouseEvent> myBuffer;
	};

	class Input
	{
		friend class Keyboard;
	public:
		Input() = default;
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		void Init(HWND someHwnd);
		bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);

		Mouse& GetMouse();
		Keyboard& GetKeyboard();

		void EndFrame();
		inline static HWND GetHwnd() { return myHwnd; }
	private:
		Mouse myMouse;
		Keyboard myKeyboard;

		static HWND myHwnd;

		int myWindowWidth;
		int myWindowHeight;
	};
}