#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>
#include <Vector>
#include <random>
#include <string>
#include<iostream>

#include<glad/glad.h>
#include<glfw3.h>

#include <assimp/pbrmaterial.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"Model/model.h"
#include"Renderer/camera.h"
#include"Renderer/ShaderManager.h"
#include<filesystem>
#include"Application/Application.h" 
#include"Definitions.h"


bool blinn = false;
bool blinnKeyPressed = false;
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void processInput(GLFWwindow* window);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;


int main()
{
    Stella::Application app;
    app.Start();
    return 0;
}

//int vain()
//{// glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//
//    Lumen::ShaderManager::CreateShaders();
//    Lumen::Shader& BlinnPhongShader = Lumen::ShaderManager::GetShader("Basic_BlinnPhong");
//
//
//    float planeVertices[] = 
//    {
//        // positions            // normals         // texcoords
//         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
//        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
//        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
//
//         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
//        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
//         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
//    };
//    Lumen::VertexArray PlaneVAO;
//    Lumen::VertexBuffer PlaneVBO;
//    PlaneVAO.Bind();
//    PlaneVBO.Bind();
//    PlaneVBO.BufferData(sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
//    PlaneVBO.VertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)0);
//    PlaneVBO.VertexAttribPointer(1, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    PlaneVBO.VertexAttribPointer(2, 2, GL_FLOAT,0, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//    PlaneVAO.Unbind();
//
//    Lumen::Texture PlaneTexture;
//    PlaneTexture.CreateTexture("src/assets/textures/wood.png", false, false);
//
//
//
//    // lighting info
//    // -------------
//    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
//
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//
//
//        glClearColor(0.3f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        PlaneVAO.Bind();
//        BlinnPhongShader.Use();
//        PlaneTexture.Bind();
//
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        BlinnPhongShader.SetMatrix4("projection", projection);
//        BlinnPhongShader.SetMatrix4("view", view);
//
//        // set light uniforms
//        BlinnPhongShader.SetVector3f("viewPos", camera.Position);
//        BlinnPhongShader.SetVector3f("lightPos", lightPos);
//        BlinnPhongShader.SetInteger("blinn", blinn);
//        BlinnPhongShader.SetInteger("floorTexture", 0);
//        
//
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//
//    }
//
//
//    glfwTerminate();
//    return 0;
//
//	//TransformParameters t;
//
//
//	//auto m = Model(("D:/VSProject/ToyEngine/src/assets/models/Sponza/Sponza.gltf"),t,false);
//
//	//std::string path("D:/VSProject/ToyEngine/src/assets/models/Sponza/5061699253647017043.png");
//	//int width, height, nComponents;
//	//unsigned char* data = stbi_load(path.c_str(), &width, &height, &nComponents, 0);
//
//}
//
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//
//    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
//    {
//        blinn = !blinn;
//        blinnKeyPressed = true;
//    }
//    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
//    {
//        blinnKeyPressed = false;
//    }
//}
//
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
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
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}