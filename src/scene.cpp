#include "config.h"

// VARIABLES
static int windowWidth = 1366;
static int windowHeight = 768;
static GLFWwindow *window; // Create window
static void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                         int mode);
double mouseX, mouseY;

// camera
float cX = 0.0f;
float cY = 0.0f;
float cZ = 0.0f; // camera positions
float camera_aimX = 0.0f;
float camera_aimY = 0.0f;
float camera_speed = 5.0f;
glm::vec3 forwardAim;
glm::vec3 camera_pos = glm::vec3(cX, cY, cZ);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_lookAt(0.0f, 0.0f, 0.0f);
static float camera_fov = 70.0f;
static float camera_near = 0.1f;
static float camera_far = 2000.0f;

// FUNCTIONS
static void ProgramSetup() {
  // SETTINGS
  glfwInit(); // Initialise glfw

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(windowWidth, windowHeight, "My Window", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // Initialise glad

  // PRE-RENDER SETTINGS
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glViewport(0, 0, windowWidth, windowHeight);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
}
static GLuint LoadTextureTileBox(const char *texture_file_path) {
  int w, h, channels;
  uint8_t *img = stbi_load(texture_file_path, &w, &h, &channels, 3);
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // To tile textures on a box, we set wrapping to repeat
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (img) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 img);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture " << texture_file_path << std::endl;
  }
  stbi_image_free(img);

  return texture;
}
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  // Move
  if (key == GLFW_KEY_W) {
    cX += forwardAim.x * camera_speed;
    cY += forwardAim.y * camera_speed;
    cZ += forwardAim.z * camera_speed;
  }
  if (key == GLFW_KEY_S) {
    cX -= forwardAim.x * camera_speed;
    cY -= forwardAim.y * camera_speed;
    cZ -= forwardAim.z * camera_speed;
  }
  if (key == GLFW_KEY_A) {
    cX += forwardAim.z * camera_speed;
    cZ -= forwardAim.x * camera_speed;
  }
  if (key == GLFW_KEY_D) {
    cX -= forwardAim.z * camera_speed;
    cZ += forwardAim.x * camera_speed;
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

    textureID = LoadTextureTileBox("../images/sky.png");
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
      // Water surface
      -1.0f, 0.0f, -1.0f, 1.0f,  0.0f, -1.0f,
      1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 1.0f,
  };

  GLfloat colourBufferData[12]{
      // Light-Blue
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  };

  GLuint indexBufferData[6]{
      0, 1, 2, 0, 2, 3,
  };

  GLfloat normalBufferData[12]{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                               1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint colorBufferID;
  GLuint indexBufferID;
  GLuint programID;
  GLuint mvpMatrixID;
  GLuint normalBufferID;

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
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colourBufferData), colourBufferData,
                 GL_STATIC_DRAW);

    // Create a vertex buffer object to store the vertex normals
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferData), normalBufferData,
                 GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines
    // triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData),
                 indexBufferData, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../src/shaders/base.vert",
                                    "../src/shaders/base.frag");

    if (programID == 0) {
      std::cerr << "Failed to load the \"water\" shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
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
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glEnableVertexAttribArray(2);

    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

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
struct SpotLight { /*TODO: make a light*/
};

struct Island {
  glm::vec3 islePos;
  int isleScaleFactor;

  Asset* isleBase;
  Asset* building;
  Asset* building2;
  std::vector<Asset> buildings;

  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;

  std::vector<std::vector<float>> grid;
  const int baseSize = 10;
  const int buildSpace = 5;

  Island(glm::vec3 islePos, int isleScaleFactor) {
    this->islePos = islePos;
    this->isleScaleFactor = isleScaleFactor;
    createIsleGrid(isleScaleFactor);
  };

  void createIsleGrid(int scaleFac) {
    float random = (rand() % 100) /100;
    //CREATE A (baseSize*scaleFac X baseSize*scaleFac) square matrix of 0-1s
    for(int i = 0; i < baseSize*scaleFac; i ++){
      std::vector<float> row;
      for(int j = 0; j < baseSize*scaleFac; j ++) {
        float random = (rand() % 100)/100.0f;
        row.push_back(random);
      }
      grid.push_back(row);
    }
    //PLACE FLOATING GARDENS (2x2) - 2
    for(int i = 0; i < scaleFac*2;){
      //Generate a random position that is always at least 1 away from the edge
      int randomX = (rand() % baseSize*scaleFac) + 1;
      int randomY = (rand() % baseSize*scaleFac) + 1;
      if(randomX > baseSize*scaleFac - 2) {
        randomX -= (randomX - (baseSize*scaleFac-3));
      }
      if(randomY > baseSize*scaleFac - 2) {
        randomY -= (randomY - (baseSize*scaleFac-3));
      }
      //Find a plot that will not intersect with another garden
      if(grid.at(randomX).at(randomY) != 2.0f && grid.at(randomX+1).at(randomY) != 2.0f && grid.at(randomX).at(randomY+1) != 2.0f && grid.at(randomX).at(randomY) != 2.0f){
        grid.at(randomX).at(randomY) = 2.0f;
        grid.at(randomX+1).at(randomY) = 2.0f;
        grid.at(randomX).at(randomY+1) = 2.0f;
        grid.at(randomX+1).at(randomY+1) = 2.0f;
        i++;
      }
    }

    std::cout << std::endl << "Grid: " << std::endl;
    for(int i = 0; i < baseSize*scaleFac; i ++){
      for(int j = 0; j < baseSize*scaleFac; j ++){
        std::cout << grid.at(i).at(j) << ", ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  void initialise(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;
    isleBase = new Asset("isle_base");
    building = new Asset("temp_building");
    building2 = new Asset("temp_building2");

    building->initialise(position, scale);
    building2->initialise(position, scale);
    isleBase->initialise(position, glm::vec3(scale.x*(buildSpace/2)*isleScaleFactor*baseSize, scale.y, scale.z*(buildSpace/2)*isleScaleFactor*baseSize));

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../src/shaders/base.vert",
                                    "../src/shaders/base.frag");

    if (programID == 0) {
      std::cerr << "Failed to load the \"island\" shaders." << std::endl;
    }
  }

  void render(glm::mat4 cameraMatrix, glm::vec3 cameraPos) {
    glUseProgram(programID);
    isleBase->render(cameraMatrix,cameraPos);
    for(int i = 0; i < grid.size(); i ++){
      for(int j = 0; j < grid.size(); j ++) {
        glm::vec3 buildPos = glm::vec3(cameraPos.x + (i*buildSpace*scale.x), cameraPos.y, cameraPos.z + (j*buildSpace*scale.x));
        if(grid.at(i).at(j) > 0.5){
          building->render(cameraMatrix, buildPos);
        }else{
          building2->render(cameraMatrix, buildPos);
        }
      }
    }
  }

};

// MAIN LOOP
int main() {
  ProgramSetup();
  // Object initialisation
  Water water;
  water.initialise(glm::vec3(0.0f, -50.0f, 0.0f),
                   glm::vec3(1000.0f, 1.0f, 1000.0f));
  SkyBox skyBox;
  skyBox.initialise(glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1000.0f, 1000.0f, 1000.0f));

  Island isle(glm::vec3(0,-50.0f,0), 1);
  isle.initialise(glm::vec3(50.0f, -50.0f, 0.0f),
                  glm::vec3(20.0f,20.0f,20.0f));

  // Camera setup
  glm::mat4 viewMatrix, projectionMatrix;
  projectionMatrix = glm::perspective(glm::radians(camera_fov), 16.0f / 9.0f,
                                      camera_near, camera_far);

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

    water.render(vp);
    skyBox.render(vp);
    isle.render(vp,glm::vec3(cX, cY, cZ));

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (!glfwWindowShouldClose(window));

  // END PROCESSES
  water.cleanup();
  skyBox.cleanup();

  // Terminate glfw
  glfwTerminate();
  return 0;
}
