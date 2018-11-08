#pragma once
/*
@file	GLFWEW.h
*/
#include <glew.h>
#include "Gamepad.h"
#include <glfw3.h>
#include <glm/glm.hpp>

namespace GLFWEW {

	/*
	GLFWとGLEWのラッパークラス.
	*/
	class Window
	{
	public:
		static Window& instance();

		bool Init(int w, int h, const char* title, bool fullScreen = false);
		bool ShouldClose() const;
		void SwapBuffers() const;
		const GamePad& GetGamePad(int i) const;
		void UpdateGamePad();

		glm::vec2 GetLeftAxe(int i) { return LeftAxe[i]; }
		glm::vec2 GetRightAxe(int i) { return RightAxe[i]; }
		float GetBackAxe(int i) { return backAxe[i]; }

		static void SetCursorPosition(GLFWwindow* window, double posX, double posY);
		glm::vec2 GetCursorPosition() { return mousePos; }
		glm::vec2 GetCursorOffsetPosition() { return mouseOffsetPos; }
		void setCursorPosition() {

			if (firstMouse) {
				mouseLastPos.x = mousePos.x;
				mouseLastPos.y = mousePos.y;

				firstMouse = false;
			}
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			mousePos = glm::vec2(x, y);

			mouseOffsetPos.x = mousePos.x - mouseLastPos.x;
			mouseOffsetPos.y = mouseLastPos.y - mousePos.y;

			mouseLastPos.x = mousePos.x;
			mouseLastPos.y = mousePos.y;

		}

		void ToggleFullScreen();

	private:
		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		static const int MaxControllerNum = 4;

		bool isGLFWInitialized;
		bool isInitialized;
		bool firstMouse = true;
		GLFWwindow* window;
		GamePad gamepad[MaxControllerNum];
		GamePad gamepad2;

		glm::vec2 LeftAxe[MaxControllerNum];
		glm::vec2 RightAxe[MaxControllerNum];
		float backAxe[MaxControllerNum];

		glm::vec2 mousePos;
		glm::vec2 mouseLastPos;
		glm::vec2 mouseOffsetPos;

	};
}