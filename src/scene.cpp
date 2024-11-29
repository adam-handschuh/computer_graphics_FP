#include "config.h"

//VARIABLES
static int windowWidth = 1366;
static int windowHeight = 768;
static GLFWwindow *window;   //Create window
static void key_callback(GLFWwindow *window, int key, int scanCode, int action, int mode);
double mouseX, mouseY;

//camera
float cX = 300.0f;
float cY = 300.0f;
float cZ = 300.0f;   //camera positions

float camera_aimX;
float camera_aimY;

glm::vec3 camera_pos = glm::vec3(cX, cY, cZ);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_lookAt(0.0f, 0.0f, 0.0f);
static float camera_fov = 90.0f;
static float camera_near = 0.1f;
static float camera_far = 1000.0f;

//STRUCTS
struct Scene{
  GLfloat vertexBufferData[12]{
      // Water surface
      -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
  };

  GLfloat colorBufferData[12]{
      // Light-Blue
      0.5f, 0.5f, 1.0f,
      0.5f, 0.5f, 1.0f,
      0.5f, 0.5f, 1.0f,
      0.5f, 0.5f, 1.0f,
  };

  GLuint indexBufferData[6]{
      0, 1, 2,
      0, 2, 3,
  };

  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint colorBufferID;
  GLuint indexBufferID;
  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;


  void initialise(glm::vec3 position, glm::vec3 scale){
    this->position = position;
    this->scale = scale;

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../src/shaders/base.vert", "../src/shaders/base.frag");
    if (programID == 0)
    {
      std::cerr << "Failed to load shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");

  }

  void render(glm::mat4 cameraMatrix){
    glUseProgram(programID);

    //Handle Scene Buffers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);


    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(
        GL_TRIANGLES,                 // mode
        sizeof(indexBufferData),      // number of indices
        GL_UNSIGNED_INT,               // type
        (void*)0                     // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  void cleanup(){
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &vertexArrayID);
    glDeleteProgram(programID);
  }
};


//FUNCTIONS
void updateMouse(){
  glfwGetCursorPos(window, &mouseX, &mouseY);
}

//MAIN LOOP
int main(){
  //SETTINGS
  glfwInit();     //Initialise glfw

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);      // For MacOS
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(windowWidth,
                            windowHeight,
                            "My Window",
                            NULL,
                            NULL);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);


  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);   //Initialise glad

  //PRE-RENDER SETTINGS

  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glViewport(0,0,windowWidth,windowHeight);

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);


  Scene scene;
  scene.initialise(glm::vec3(0.0f,0.0f,0.0f),
                   glm::vec3(200.0f,200.0f,200.0f));

  // Camera setup
  glm::mat4 viewMatrix, projectionMatrix;
  projectionMatrix = glm::perspective(glm::radians(camera_fov), 16.0f/9.0f, camera_near, camera_far);

  //RENDER LOOP
  do{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //mouseStuff
    updateMouse();
    std::cout <<  mouseX << " " << mouseY << std::endl;
    camera_aimX = mouseY;
    camera_aimY = mouseX;

    //Render Camera-View
    camera_lookAt = glm::vec3(0,0,0);
    camera_pos = glm::vec3(cX,cY,cZ);
    viewMatrix = glm::lookAt(camera_pos, camera_lookAt, camera_up);
    glm::mat4 vp =  projectionMatrix * viewMatrix;

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
  if (key == GLFW_KEY_W) {
    cX -= 5;
  }
  if (key == GLFW_KEY_S) {
    cX += 5;
  }
  if (key == GLFW_KEY_A) {
    cZ -= 5;
  }
  if (key == GLFW_KEY_D) {
    cZ += 5;
  }
}
