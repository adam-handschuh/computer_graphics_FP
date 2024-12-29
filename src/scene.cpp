#include "config.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

// VARIABLES
static int windowWidth = 1366;
static int windowHeight = 768;
static GLFWwindow *window; // Create window
static void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                         int mode);

// camera
float cX = 0.0f;
float cY = 300.0f;
float cZ = 0.0f; // camera positions

float speedX = 0;
float speedZ = 0;

float camera_aimX = 0.0f;
float camera_aimY = 0.0f;
float camera_speed = 50.0f;
glm::vec3 forwardAim;
glm::vec3 camera_pos = glm::vec3(cX, cY, cZ);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_lookAt(0.0f, 0.0f, 0.0f);
static float camera_fov = 70.0f;
static float camera_near = 0.1f;
static float camera_far = 8000.0f;

//Shadow Map
static int shadowMapWidth = 0;
static int shadowMapHeight = 0;

GLuint fbo;
GLuint depthTex;
GLuint mlpMatID;

//Reflections
GLuint refFbo;
GLuint refTex;

PointLight* moon;

GLuint firstPassID;

// FUNCTIONS
static void ProgramSetup() {
  // SETTINGS
  glfwInit(); // Initialise glfw

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(windowWidth, windowHeight, "Approaching a futuristic Emerald Isle", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // Initialise glad

  // PRE-RENDER SETTINGS
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glViewport(0, 0, windowWidth, windowHeight);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // DepthFBO
  // FBO setup
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  //Depth Texture setup
  glGenTextures(1, &depthTex);
  glBindTexture(GL_TEXTURE_2D, depthTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  //ReflectionFBO
  glGenFramebuffers(1, &refFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, refFbo);
  glGenTextures(1, &refTex);
  glBindTexture(GL_TEXTURE_2D, refTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refTex, 0);

  moon = new PointLight(windowWidth,windowHeight);

  firstPassID = LoadShadersFromFile("../src/shaders/first.vert", "../src/shaders/first.frag");
  if(firstPassID == 0)
  {
    std::cerr << "Failed to load shaders." << std::endl;
  }
  mlpMatID = glGetUniformLocation(firstPassID, "MLP");
}
static GLuint LoadTexture(std::string imageName) {
    // Load image
    GLuint textureID;

    int width, height, channels;
    uint8_t* data = stbi_load(("../images/" + imageName).c_str(), &width,
                              &height, &channels, 0);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      std::cout << ("../images/" + imageName).c_str() << std::endl;
    } else {
      std::cout << "could not load image" << std::endl;
    }
    stbi_image_free(data);

  // To tile textures on a box, we set wrapping to repeat
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return textureID;
}
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  // Move
  if (key == GLFW_KEY_W) {
    cX += forwardAim.x * camera_speed;
    cY += forwardAim.y * camera_speed;
    cZ += forwardAim.z * camera_speed;
    speedX = forwardAim.x * camera_speed;
    speedZ = forwardAim.z * camera_speed;
  }
  if (key == GLFW_KEY_S) {
    cX -= forwardAim.x * camera_speed;
    cY -= forwardAim.y * camera_speed;
    cZ -= forwardAim.z * camera_speed;
    speedX = -forwardAim.x * camera_speed;
    speedZ = -forwardAim.z * camera_speed;
  }
  if (key == GLFW_KEY_A) {
    cX += forwardAim.z * camera_speed;
    cZ -= forwardAim.x * camera_speed;
    speedX = forwardAim.z * camera_speed;
    speedZ = -forwardAim.x * camera_speed;
  }
  if (key == GLFW_KEY_D) {
    cX -= forwardAim.z * camera_speed;
    cZ += forwardAim.x * camera_speed;
    speedX = -forwardAim.z * camera_speed;
    speedZ = forwardAim.x * camera_speed;
  }
  // Look
  if (key == GLFW_KEY_UP) {
    camera_aimY += 5;
  }
  if (key == GLFW_KEY_DOWN) {
    camera_aimY -= 5;
  }
  if (key == GLFW_KEY_LEFT) {
    camera_aimX -= 5;
  }
  if (key == GLFW_KEY_RIGHT) {
    camera_aimX += 5;
  }
}

void firstPass(){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0,0,shadowMapWidth, shadowMapHeight);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(firstPassID);
  glm::mat4 mlp = moon->lightMatrix();
  glUniformMatrix4fv(mlpMatID, 1, GL_FALSE, &mlp[0][0]);
};
void secondPass(){
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glViewport(0,0,windowWidth,windowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,depthTex);
};

// STRUCTS
struct SkyBox {
  // SKYBOX
  GLfloat vertexBufferData[72]{
      // Front face
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,

      // Back face
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,

      // Left face
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,

      // Right face
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,

      // Top face
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,

      // Bottom face
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
  };

  GLfloat colorBufferData[72];

  GLuint indexBufferData[36] = {
      0,  1,  2,  0,  2,  3,

      4,  5,  6,  4,  6,  7,

      8,  9,  10, 8,  10, 11,

      12, 13, 14, 12, 14, 15,

      16, 17, 18, 16, 18, 19,

      20, 21, 22, 20, 22, 23,
  };

  GLfloat uvBufferData[48] = {
      // POZ Z
      0.25f,
      (2.0f / 3.0f),
      0.5f,
      (2.0f / 3.0f),
      0.5f,
      (1.0f / 3.0f),
      0.25f,
      (1.0f / 3.0f),
      // NEG Z
      0.75f,
      (2.0f / 3.0f),
      1.0f,
      (2.0f / 3.0f),
      1.0f,
      (1.0f / 3.0f),
      0.75f,
      (1.0f / 3.0f),
      // NEG X
      0.5f,
      (2.0f / 3.0f),
      0.75f,
      (2.0f / 3.0f),
      0.75f,
      (1.0f / 3.0f),
      0.5f,
      (1.0f / 3.0f),
      // POS X
      0.0f,
      (2.0f / 3.0f),
      0.25f,
      (2.0f / 3.0f),
      0.25f,
      (1.0f / 3.0f),
      0.0f,
      (1.0f / 3.0f),
      // NEG Y
      0.25f,
      1.0f,
      0.50f,
      1.0f,
      0.50f,
      2.0f / 3.0f,
      0.25f,
      2.0f / 3.0f,
      // POS Y
      0.25f,
      1.0f / 3.0f,
      0.50f,
      1.0f / 3.0f,
      0.50f,
      0.0f,
      0.25f,
      0.0f,
  };

  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint textureSamplerID;
  GLuint colorBufferID;
  GLuint indexBufferID;
  GLuint uvBufferID;
  GLuint textureID;
  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;

  void initialise(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData,
                 GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    for (int i = 0; i < 72; ++i)
      colorBufferData[i] = 1.0f;
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData,
                 GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines
    // triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData),
                 indexBufferData, GL_STATIC_DRAW);

    // Sort out textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData,
                 GL_STATIC_DRAW);

    textureID = LoadTexture("sky.png");
    programID = LoadShadersFromFile("../src/shaders/sky.vert",
                                    "../src/shaders/sky.frag");
    if (programID == 0) {
      std::cerr << "Failed to load the \"sky\" shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
  }

  void render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    // Handle Scene Buffers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glEnableVertexAttribArray(2);

    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 cam(0, cY, 0);
    modelMatrix = glm::translate(modelMatrix, position + cam);
    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // Set textureSampler to use texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glDrawElements(GL_TRIANGLES,            // mode
                   sizeof(indexBufferData), // number of indices
                   GL_UNSIGNED_INT,         // type
                   (void *)0                // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
  }

  void cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &vertexArrayID);
    glDeleteProgram(programID);
  }
};
struct Water {
  // WATER
  GLfloat vertexBufferData[12]{
      -1.0f,0.0f,-1.0f,
      -1.0f,0.0f,1.0f,
      1.0f,0.0f,1.0f,
      1.0f,0.0f,-1.0f
  };

  GLuint indexBufferData[6]{
      0,1,2,
      0,2,3
  };

  GLfloat colourBufferData[12]{
      0.0f,0.0f,1.0f,
      0.0f,0.0f,1.0f,
      0.0f,0.0f,1.0f,
      0.0f,0.0f,1.0f
  };

  GLfloat uvBufferData[8]{
      0.0f,0.0f,
      0.0f,1.0f,
      1.0f,1.0f,
      1.0f,0.0f
  };


  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint colourBufferID;
  GLuint indexBufferID;
  GLuint programID;
  GLuint mvpMatrixID;
  GLuint uvBufferID;
  GLuint refSamplerID;
  GLuint textureID;
  GLuint textureSamplerID;

  glm::vec3 position;
  glm::vec3 scale;

  void initialise(glm::vec3 position, glm::vec3 scale) {

    this->position = position;
    this->scale = scale;

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData,
                 GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colourBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colourBufferData), colourBufferData,
                 GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines
    // triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData),
                 indexBufferData, GL_STATIC_DRAW);

    // Sort out textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData,
                 GL_STATIC_DRAW);

    textureID = LoadTexture("test_sea.jpg");
    programID = LoadShadersFromFile("../src/shaders/base.vert",
                                    "../src/shaders/base.frag");

    if (programID == 0) {
      std::cerr << "Failed to load the \"water\" shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    refSamplerID = glGetUniformLocation(programID, "refSampler");
    textureSamplerID = glGetUniformLocation(programID, "imgSampler");
  }

  void render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    // Handle Scene Buffers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);


    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 cam(0, 0, 0);
    modelMatrix = glm::translate(modelMatrix, position+cam);
    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // Set textureSampler to use texture unit 0
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, refTex);
    glUniform1i(refSamplerID, 2);


    float timeValue = glfwGetTime(); // GLFW provides time since app start
    glUniform1f(glGetUniformLocation(programID, "time"), timeValue);


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 3);

    glDrawElements(GL_TRIANGLES,            // mode
                   sizeof(indexBufferData), // number of indices
                   GL_UNSIGNED_INT,         // type
                   (void *)0                // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
  }

  void cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colourBufferID);
    glDeleteBuffers(1, &vertexArrayID);
    glDeleteProgram(programID);
  }
};
struct Car {
  int direction = 1;        // 1 - north | 2 - east | 3 - south | 4 - west
  bool leftOrRight = false; // true - left | false - right
  glm::vec3 position;
  glm::vec3 scale;
  int gridX, gridY;
  bool readyForTurn = false;
  bool turned = true;
  float turningAngle = 0;
  float turnAngle = 0;

  std::vector<std::vector<float>> grid;

  float distCount = 0;

  float speed;
  Asset *carModel;

  void initialise(glm::vec3 position, glm::vec3 scale, std::vector<std::vector<float>> grid) {
    this->position = position;
    this->scale = scale;
    this->grid = grid;
    try {
      int randomX = (rand() % (grid.size() - 3))+1;
      int randomY = (rand() % (grid.size() - 3))+1;

      while (grid.at(randomX).at(randomY) != -1) {
        randomX = (rand() % (grid.size() - 3))+1;
        randomY = (rand() % (grid.size() - 3))+1;
      }
      gridX = randomX;
      gridY = randomY;
      std::cout << "X:" << gridX << "  Y:" << gridY << std::endl;

      this->position.x = 0;
      this->position.x -= grid.size() * ((2 * 20));
      this->position.x += (gridX + 1) * (2 * 20);

      this->position.z = 0;
      this->position.z += 2 * 20;
      this->position.z -= (gridY + 1) * (2 * 20);

      this->position.y = 100.0f;

      int random = rand() % 100;
      // 70% chance to be a car
      if(random > 30) {
        speed = 0.025;
        carModel = new Asset("temp_car");
      }else{
        speed = 0.05;
        carModel = new Asset("temp_bus");
      }
      carModel->initialise(position, scale, moon);

      if ((gridX == 0 || gridX == grid.size() - 1) &&
          (gridY > 0 && gridY < grid.size() - 1)) {
        direction = 2;
      } else if ((gridY == 0 || gridY == grid.size() - 1) &&
                 (gridX > 0 && gridX < grid.size() - 1)) {
        direction = 1;
      } else {
        if (grid.at(gridX - 1).at(gridY) == -1 ||
            grid.at(gridX + 1).at(gridY)) {
          direction = 1;
        } else if (grid.at(gridX).at(gridY - 1) == -1 ||
                   grid.at(gridX).at(gridY + 1)) {
          direction = 2;
        }
      }

      if (direction == 1) {
        turnAngle = 90;
      } else {
        turnAngle = 0;
      }
    }catch(std::errc e) {
      std::cout << "Failed" << std::endl << std::endl;
    }
  }

  void render(glm::mat4 cameraMatrix, glm::vec3 camPos, bool secondPass) {
    // speed adjustments

    if(!turned){
      if(leftOrRight){
        if(turningAngle < 90){
          turningAngle ++;
          turnAngle ++;
        }else{
          turningAngle = 0;
          turned = true;
        }
      }else{
        if(turningAngle > -90){
          turningAngle --;
          turnAngle --;
        }else{
          turningAngle = 0;
          turned = true;
        }
      }
    }

    // Decide turning direction
    if(direction == 1) {
      position.x -= speed;
      if (distCount >= 2 * 20) {
        readyForTurn = true;
        distCount = 0;
        gridX--;
      }
      if (shouldTurnHere()) {
        readyForTurn = false;
        turned = false;
        direction = leftOrRight ? 4 : 2;
      }
    }else if(direction == 2) {
      position.z -= speed;

      if (distCount >= 2 * 20) {
        readyForTurn = true;
        distCount = 0;
        gridY++;
      }
      if (shouldTurnHere()) {
        readyForTurn = false;
        turned = false;
        direction = leftOrRight ? 1 : 3;
      }
    }else if(direction == 3) {
      position.x += speed;

      if (distCount >= 2 * 20) {
        readyForTurn = true;
        distCount = 0;
        gridX++;
      }
      if (shouldTurnHere()) {
        readyForTurn = false;
        turned = false;
        direction = leftOrRight ? 2 : 4;
      }
    }else if(direction == 4){
      position.z += speed;

      if (distCount >= 2 * 20) {
        readyForTurn = true;
        distCount = 0;
        gridY--;
      }
      if (shouldTurnHere()) {
        readyForTurn = false;
        turned = false;
        direction = leftOrRight ? 3 : 1;
      }
    }
    carModel->rotate(glm::vec3(0.0f,1.0f,0.0f), turnAngle*M_PI/180);
    carModel->render(cameraMatrix, camPos - position, secondPass);
    distCount += speed;
  }

  bool shouldTurnHere() {
    if(readyForTurn){
      int random = rand() % 100;
      switch(direction){
      case 1:
        if(random > 75 && roadEast()){
          leftOrRight = false;
          return true;
        }else if(random > 50 && roadWest()){
          leftOrRight = true;
          return true;
        }else if(!roadNorth()){
          if(roadEast()){
            leftOrRight = false;
            return true;
          }
          if(roadWest()){
            leftOrRight = true;
            return true;
          }
        }
        break;
      case 2:
        if(random > 75 && roadSouth()){
          leftOrRight = false;
          return true;
        }else if(random > 50 && roadNorth()){
          leftOrRight = true;
          return true;
        }else if(!roadEast()){
          if(roadSouth()){
            leftOrRight = false;
            return true;
          }
          if(roadNorth()){
            leftOrRight = true;
            return true;
          }
        }
        break;
      case 3:
        if(random > 75 && roadEast()){
          leftOrRight = true;
          return true;
        }else if(random > 50 && roadWest()){
          leftOrRight = false;
          return true;
        }else if(!roadSouth()){
          if(roadEast()){
            leftOrRight = true;
            return true;
          }
          if(roadWest()){
            leftOrRight = false;
            return true;
          }
        }
        break;
      case 4:
        if(random > 75 && roadSouth()){
          leftOrRight = true;
          return true;
        }else if(random > 50 && roadNorth()){
          leftOrRight = false;
          return true;
        }else if(!roadWest()){
          if(roadNorth()){
            leftOrRight = false;
            return true;
          }
          if(roadSouth()){
            leftOrRight = true;
            return true;
          }
        }
        break;
      }
    }
    return false;
  }

  bool roadNorth(){
    if(gridX != 0) {
      if (grid.at(gridX - 1).at(gridY) == -1) {
        return true;
      }
    }
    return false;
  }

  bool roadEast(){
    if(gridY != grid.size()-1) {
      if (grid.at(gridX).at(gridY + 1) == -1) {
        return true;
      }
    }
    return false;
  }

  bool roadSouth(){
    if(gridX != grid.size()-1) {
      if (grid.at(gridX + 1).at(gridY) == -1) {
        return true;
      }
    }
    return false;
  }

  bool roadWest(){
    if(gridY != 0) {
      if (grid.at(gridX).at(gridY - 1) == -1) {
        return true;
      }
    }
    return false;
  }

};
struct Island {
  glm::vec3 islePos;
  int isleScaleFactor;

  Asset *isleBase;
  Asset *building;
  Asset *building2;
  Asset *building3;
  Asset *garden;

  //Leaves
  std::vector<Asset> leaves;
  std::vector<glm::vec3> leafOffsets;
  std::vector<GLfloat> leafSpeeds;
  std::vector<GLfloat> leafSways;

  std::vector<Car> cars;
  std::vector<PointLight> lights;

  int randomAngle;

  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;

  std::vector<std::vector<float>> grid;
  const int baseSize = 10;
  const int buildSpace = 2;
  float gardenHoverAnim = 0;

  Island(glm::vec3 islePos, int isleScaleFactor) {
    this->islePos = islePos;
    this->isleScaleFactor = isleScaleFactor;
  };

  void setIsleScaleFactor(int isleScaleFactor){
    this->isleScaleFactor = isleScaleFactor;
  }

  void createIsleGrid(int scaleFac) {
    // CREATE AN OUTER LAYER OF -1s
    grid.clear();
    std::vector<float> nullRow;
    for (int i = 0; i < baseSize * scaleFac; i++) {
      nullRow.push_back(-1);
      nullRow.push_back(-1);
      nullRow.push_back(-1);
    }

    // CREATE A (baseSize*scaleFac X baseSize*scaleFac) square matrix of 0-1s
    for (int i = 0; i < baseSize * scaleFac; i++) {
      grid.push_back(nullRow);
      std::vector<float> row;
      for (int j = 0; j < baseSize * scaleFac; j++) {
        float random = (rand() % 100) / 100.0f;
        row.push_back(-1);
        row.push_back(random);
      }
      row.push_back(-1);
      grid.push_back(row);
    }
    grid.push_back(nullRow);

    // PLACE FLOATING GARDENS (2x2) - 2
    for (int i = 0; i < scaleFac * 2;) {
      // Generate a random position that is always at least 1 away from the edge
      int randomX = (rand() % baseSize * scaleFac) + 1;
      int randomY = (rand() % baseSize * scaleFac) + 1;
      if (randomX > baseSize * scaleFac - 2) {
        randomX -= (randomX - (baseSize * scaleFac - 3));
      }
      if (randomY > baseSize * scaleFac - 2) {
        randomY -= (randomY - (baseSize * scaleFac - 3));
      }
      // Find a plot that will not intersect with another garden
      if (grid.at(randomX).at(randomY) != 2.0f &&
          grid.at(randomX + 1).at(randomY) != 2.0f &&
          grid.at(randomX).at(randomY + 1) != 2.0f &&
          grid.at(randomX).at(randomY) != 2.0f) {
        if (grid.at(randomX).at(randomY) != 0.0f &&
            grid.at(randomX + 1).at(randomY) != 0.0f &&
            grid.at(randomX).at(randomY + 1) != 0.0f &&
            grid.at(randomX).at(randomY) != 0.0f &&
            grid.at(randomX).at(randomY) != -1) {
          grid.at(randomX).at(randomY) = 2.0f;
          i++;
        }
      }
    }
  }

  void initialise(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;
    createIsleGrid(isleScaleFactor);

    //Basic models
    isleBase = new Asset("isle_base");
    building = new Asset("temp_building");
    building2 = new Asset("temp_building2");
    building3 = new Asset("temp_building3");
    garden = new Asset("temp_garden");

    building->initialise(position, scale,moon);
    building2->initialise(position, scale,moon);
    building3->initialise(position,scale,moon);
    garden->initialise(position, scale,moon);

    //Car models
    for(int i = 0; i < baseSize*isleScaleFactor; i ++){
      Car car;
      car.initialise(this->position, glm::vec3(1.0f,1.0f,1.0f), grid);
      cars.push_back(car);
    }

    //Leaf models
    for(int i = 0; i < 20; i ++){
      Asset leaf = Asset("leaf");
      leaf.initialise(position,scale,moon);
      leaves.push_back(leaf);
      glm::vec3 offset = glm::vec3((((rand()%400)-200)/10.0f), 0, (((rand()%400)-200)/10.0f));
      leafOffsets.push_back(offset);
      GLfloat speed = (rand()%10)/1000.0f;
      GLfloat sway = rand()%200;
      leafSpeeds.push_back(speed);
      leafSways.push_back(sway);
    }




    isleBase->initialise(
        position,
        glm::vec3(scale.x * buildSpace * isleScaleFactor * baseSize, scale.y,
                  scale.z * buildSpace * isleScaleFactor * baseSize),moon);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../src/shaders/base.vert",
                                    "../src/shaders/base.frag");

    if (programID == 0) {
      std::cerr << "Failed to load the \"island\" shaders." << std::endl;
    }
  }

  void render(glm::mat4 cameraMatrix, glm::vec3 cameraPos, bool secondPass) {
    gardenHoverAnim += 0.025f;
    if (gardenHoverAnim >= 360.0f) {
      gardenHoverAnim = 0.0f;
    }

    glUseProgram(programID);
    isleBase->render(cameraMatrix, cameraPos, secondPass);
    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid.size(); j++) {
        glm::vec3 buildPos;
        if (grid.at(i).at(j) <= 1 && grid.at(i).at(j) > 0) {
          buildPos =
              glm::vec3(cameraPos.x + (i * (buildSpace)*scale.x), cameraPos.y,
                        cameraPos.z + (j * (buildSpace)*scale.x));
          if (grid.at(i).at(j) > 0.66) {
            building->render(cameraMatrix, buildPos,secondPass);
          } else if(grid.at(i).at(j) < 0.66 && grid.at(i).at(j) > 0.2){
            building2->render(cameraMatrix, buildPos, secondPass);
          }else{
            building3->render(cameraMatrix, buildPos, secondPass);
          }
        } else if (grid.at(i).at(j) == 2) {
          buildPos = glm::vec3(cameraPos.x + (i * (buildSpace)*scale.x),
                               cameraPos.y +
                                   (5 * (sin(gardenHoverAnim * M_PI / 180.0f))) - 200,
                               cameraPos.z + (j * (buildSpace)*scale.x));
          garden->render(cameraMatrix, buildPos, secondPass);
          //Leaf animation
          for(int i = 0; i < leaves.size(); i ++){
            leafOffsets.at(i).y += leafSpeeds.at(i);
            if(leafOffsets.at(i).y >= 50.0f){
              leafOffsets.at(i).y = 0;
            }
            leafSways.at(i) += 0.1;
            leaves.at(i).rotate(glm::vec3(1,0,0), sin(leafSways.at(i)*M_PI/180.0f));
            leaves.at(i).render(cameraMatrix,buildPos+leafOffsets.at(i),secondPass);
          }
        }
      }
    }
    for (int i = 0; i < cars.size(); i ++) {
      cars.at(i).render(cameraMatrix, cameraPos, secondPass);
    }
  }

  void cleanup() {
    isleBase->cleanup();
    building->cleanup();
    building2->cleanup();
    building3->cleanup();
    for (Car car : cars) {
      car.carModel->cleanup();
    }
  }
};

// MAIN LOOP
int main() {
  ProgramSetup();
  // Object initialisation
  std::vector<PointLight> pLights;

  Water water;
  water.initialise(glm::vec3(0.0f, 0.0f, 0.0f),
                   glm::vec3(5000.0f, 1.0f, 5000.0f));
  SkyBox skyBox;
  skyBox.initialise(glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(5000.0f, 5000.0f, 5000.0f));

  Island isle(glm::vec3(0, 50.0f, 0), 1);
  isle.initialise(glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(20.0f, 20.0f, 20.0f));

  Asset moonModel ("moon");
  moonModel.initialise(moon->lightPosition, glm::vec3(-10,10,-10),moon);

  // Camera setup
  glm::mat4 viewMatrix, projectionMatrix;
  projectionMatrix = glm::perspective(glm::radians(camera_fov), 16.0f / 9.0f,
                                      camera_near, camera_far);

  shadowMapWidth = windowWidth;
  shadowMapHeight = windowHeight;

  int random = 0;

  // RENDER LOOP
  do {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render Camera-View
    forwardAim = glm::vec3(
        cos(camera_aimX * M_PI / 180) * cos(camera_aimY * M_PI / 180),
        sin(camera_aimY * M_PI / 180),
        sin(camera_aimX * M_PI / 180) * cos(camera_aimY * M_PI / 180));
    camera_pos = glm::vec3(0, cY, 0);
    camera_lookAt = camera_pos + forwardAim;
    viewMatrix = glm::lookAt(camera_pos, camera_lookAt, camera_up);
    glm::mat4 vp = projectionMatrix * viewMatrix;

    //if island is out of bounds, reset scale and spawn ahead
    if(sqrt((cX*cX) + (cZ*cZ)) > 5600){
      random = rand() % 4;
      isle.setIsleScaleFactor(random);

      isle.initialise(glm::vec3(0, 0.0f, 0),
                      glm::vec3(20.0f, 20.0f, 20.0f));
      cX = (-cX/10)*9;
      cZ = (-cZ/10)*9;
    }

    std::cout << "random : " << random << std::endl;

    firstPass();
    skyBox.render(moon->lightMatrix());
    isle.render(moon->lightMatrix(), glm::vec3(cX, 0.0f, cZ), false);
    water.render(moon->lightMatrix());

    //reflections

    glm::mat4 rp,rProj,rView;
    rView = glm::lookAt(glm::vec3(camera_pos.x, -camera_pos.y, camera_pos.z), glm::vec3(camera_pos.x, -camera_pos.y, camera_pos.z) + glm::vec3(forwardAim.x, -forwardAim.y, forwardAim.z), glm::vec3(0,-1,0));
    rp = projectionMatrix * rView;
    glBindFramebuffer(GL_FRAMEBUFFER, refFbo);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    moonModel.render(rp,-moon->lightPosition, true);
    skyBox.render(rp);
    isle.render(rp, glm::vec3(cX, 0.0f, cZ), true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    secondPass();
    moonModel.render(vp,-moon->lightPosition, true);
    water.render(vp);
    skyBox.render(vp);
    isle.render(vp, glm::vec3(cX, 0.0f, cZ), true);






    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (!glfwWindowShouldClose(window));

  // END PROCESSES
  water.cleanup();
  skyBox.cleanup();
  isle.cleanup();

  // Terminate glfw
  glfwTerminate();
  return 0;
}
