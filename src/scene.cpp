#include "config.h"

//VARIABLES
static int windowWidth = 1366;
static int windowHeight = 768;
static GLFWwindow *window;   //Create window
static void key_callback(GLFWwindow *window, int key, int scanCode, int action, int mode);

//camera
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_lookAt(30, 30, 30);
static float camera_fov = 45.0f;
static float camera_near = 600.0f;
static float camera_far = 2500.0f;

//STRUCTS
struct Scene{


  void render(glm::mat4 vp){

  }

  void cleanup(){

  }
};

//MAIN LOOP
int main(){
  //SETTINGS
  glfwInit();                                           //Initialise glfw
  window = glfwCreateWindow(windowWidth,
                            windowHeight,
                            "My Window",
                            NULL,
                            NULL);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);   //Initialise glad

  //PRE-RENDER SETTINGS
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.5f,0.5f,0.5f,1.0f);
  glfwSetKeyCallback(window, key_callback);

  Scene scene;

  // Camera setup
  glm::mat4 viewMatrix, projectionMatrix;
  projectionMatrix = glm::perspective(glm::radians(camera_fov), (float)windowWidth / windowHeight, camera_near, camera_far);

  //RENDER LOOP
  do{
    glClear(GL_COLOR_BUFFER_BIT);

    //Render Camera-View
    viewMatrix = glm::lookAt(camera_pos, camera_lookAt, camera_up);
    glm::mat4 vp =  projectionMatrix* viewMatrix;

    scene.render(vp);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }while(!glfwWindowShouldClose(window));

  //END PROCESSES
  scene.cleanup();
  //Terminate glfw
  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    std::cout << "PRESSED R KEY" << std::endl;
  }
}
