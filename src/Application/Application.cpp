#include"Application/Application.h"

namespace Stella
{
   

	Application::Application()
	{
        Initialize();

	}

	Application::~Application()
	{
	}

    

	void Application::Initialize()
	{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        // glfw window creation
        // --------------------
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Remove in release
        m_Window = glfwCreateWindow(m_Width, m_Height, "Stella", NULL, NULL);
        if (m_Window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_Window);

        glfwSetFramebufferSizeCallback(m_Window, 
            [](GLFWwindow* window, int width, int height)
            {
                glViewport(0, 0, width, height);
            }
        );
        glfwSetCursorPosCallback(m_Window, 
            [](GLFWwindow* window, double xposIn, double yposIn)
            {

                Event e;

                e.type = EventTypes::MouseMove;
                e.window = window;
                e.mx = xposIn;
                e.my = yposIn;
                e.ts = glfwGetTime();
                QueueEvent(e, window);
            }
        );
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xposIn, double yposIn)
            {
                Event e;

                e.type = EventTypes::MouseScroll;
                e.window = window;
                e.msx = xposIn;
                e.msy = yposIn;
                e.ts = glfwGetTime();
                QueueEvent(e, window);
            }
        );

        glfwSetWindowUserPointer(m_Window, (void*)&m_EventQueue);
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        Lumen::ShaderManager::CreateShaders();
        m_Renderer = std::make_unique<Renderer>();
	}

    void Application::Start()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            OnUpdate();
        }
    }

	void Application::OnUpdate()
	{ 
        float currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        processInput(m_Window);
        this->PollEvents();

        m_Renderer->Render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();      
	}

     GLFWwindow*& Application::GetWindow() 
    {
        return m_Window;
    }




     void Application::OnEvent(Stella::Event e)
     {

         if (e.type == Stella::EventTypes::MouseScroll)
         {
            // std::cout << "MouseScroll Event" << std::endl;
             m_Renderer->m_Camera->ProcessMouseScroll(static_cast<float>(e.msy));
         }

         if (e.type == Stella::EventTypes::MouseMove)
         {
            // std::cout << "MouseMove Event" << std::endl;
             float xpos = static_cast<float>(e.mx);
                 float ypos = static_cast<float>(e.my);
                 if (m_FirstMouse)
                 {
                     m_LastX = xpos;
                     m_LastY = ypos;
                     m_FirstMouse = false;
                 }

                 float xoffset = xpos - m_LastX;
                 float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

                 m_LastX = xpos;
                 m_LastY = ypos;

                 m_Renderer->m_Camera->ProcessMouseMovement(xoffset, yoffset);

         }

         if (e.type == Stella::EventTypes::MousePress)
         {
         }

         if (e.type == Stella::EventTypes::WindowResize)
         {
         }

     }

     void Application::PollEvents()
     {
         while (!m_EventQueue.empty())
         {

             OnEvent(m_EventQueue.front());
             m_EventQueue.pop();
         }
     }

     void Application::processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Renderer->m_Camera->ProcessKeyboard(FORWARD, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Renderer->m_Camera->ProcessKeyboard(BACKWARD, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Renderer->m_Camera->ProcessKeyboard(LEFT, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Renderer->m_Camera->ProcessKeyboard(RIGHT, m_DeltaTime);
        static int plusPress = GLFW_RELEASE;
        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE && plusPress == GLFW_PRESS)
        {
            m_Renderer->debugLayer++;
            if (m_Renderer->debugLayer > 4)
            {
                m_Renderer->debugLayer = 0;
            }
        }
        plusPress = glfwGetKey(window, GLFW_KEY_KP_ADD);

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS )
        {
           // blinn = !blinn;
            //blinnKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
        {
            //blinnKeyPressed = false;
        }
    }

   

    //void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    //{
    //    float xpos = static_cast<float>(xposIn);
    //    float ypos = static_cast<float>(yposIn);
    //    if (firstMouse)
    //    {
    //        lastX = xpos;
    //        lastY = ypos;
    //        firstMouse = false;
    //    }

    //    float xoffset = xpos - lastX;
    //    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    //    lastX = xpos;
    //    lastY = ypos;

    //    camera.ProcessMouseMovement(xoffset, yoffset);
    //}
    //void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    //{
    //    camera.ProcessMouseScroll(static_cast<float>(yoffset));
    //}

    void QueueEvent(const Event& e, GLFWwindow* window)
    {
        void* ptr = glfwGetWindowUserPointer(window);

        if (ptr)
        {
            std::queue<Event>* q = (std::queue<Event>*) ptr;
            q->push(e);
        }
    }

 
}