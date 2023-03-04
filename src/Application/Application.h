#pragma once
#include<glad.h>
#include<glfw3.h>
#include<string>
#include<iostream>
#include"Renderer/Renderer.h"
#include<functional>
#include<queue>
#include"Event.h"
#include<memory>

namespace Stella
{

	class Application
	{
	public:
		Application();
		~Application();
		void Initialize();
		void Start();
		void OnUpdate();
		 GLFWwindow*& GetWindow() ;

	private:
		GLFWwindow* m_Window;
		unsigned int m_Width = 1800;
		unsigned int m_Height = 1000;
		std::queue<Event>m_EventQueue;
		std::unique_ptr<Renderer> m_Renderer;

		// timing
		float m_DeltaTime = 0.0f;
		float m_LastFrame = 0.0f;
		float m_LastX = (float)m_Width / 2.0f;
		float m_LastY = (float)m_Height / 2.0f;
		bool  m_FirstMouse = true;

	private:
		void OnUserUpdate(double ts);
		void OnUserCreate(double ts);
		void OnImguiRender(double ts);
		void OnEvent(Stella::Event e);
		void PollEvents();
		void processInput(GLFWwindow* window);

		// Callback Functions
		void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	};




	void QueueEvent(const Event& e, GLFWwindow* window);

}

