#include "config.h"

//VARIABLES
static GLFWwindow *window;   //Create window
static void key_callback(GLFWwindow *window, int key, int scanCode, int action, int mode);


//STRUCTS

//MAIN LOOP
int main(){
  //SETTINGS
  glfwInit();           //Initialise glfw
  window = glfwCreateWindow(800,800, "My Window", NULL, NULL);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); //Initialise glad

  //PRE-RENDER SETTINGS
  glClearColor(0.5f,0.5f,0.5f,1.0f);
  glfwSetKeyCallback(window, key_callback);


  //RENDER LOOP
  while(!glfwWindowShouldClose(window)){
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
  }

  //Terminate glfw
  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    std::cout << "PRESSED R KEY" << std::endl;
  }
}
