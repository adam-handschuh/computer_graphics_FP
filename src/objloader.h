//
// Created by Adam on 17/12/2024.
//

#ifndef FINAL_OBJLOADER_H
#define FINAL_OBJLOADER_H
#include "stb_image.h"
#include <config.h>
struct Asset {

  std::string name;
  std::string materialFile;
  std::string currentMaterial;

  std::vector<glm::vec3> tempVerts;
  int indexCount = 0;
  std::vector<glm::vec3> tempNorms;
  std::vector<std::string> textures;

  std::vector<GLfloat> vertexBufferData;
  std::vector<GLfloat> colourBufferData;
  std::vector<GLuint> indexBufferData;
  std::vector<GLfloat> uvBufferData;
  std::vector<GLfloat> normalBufferData;

  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint indexBufferID;
  GLuint colourBufferID;

  GLuint uvBufferID;
  GLuint normalBufferID;
  GLuint textureID;

  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;

  glm::vec3 rotation = glm::vec3(1.0f,1.0f,1.0f);
  float angle = 0;

  Asset(std::string name) { this->name = name; }

  void LoadMaterial(std::string filePath, std::string materialName) {
    std::ifstream file("../models/" + filePath);
    if (file) {
      std::string line;
      while (std::getline(file, line)) {

        if (line.starts_with("newmtl ")) {
          if (line.substr(7) == materialName) {
            // MATERIAL FOUND
            bool informationRetrieved = false;
            while (!informationRetrieved && std::getline(file, line)) {
              // FINDING DIFFUSE (BASIC RGB) VALUES
              if (line.starts_with("Kd")) {
                std::istringstream stream(line.substr(3)); // Skip "v "
                GLfloat r, g, b;
                stream >> r >> g >> b;
                for (int i = 0; i < 4; i++) {
                  colourBufferData.push_back(r);
                  colourBufferData.push_back(g);
                  colourBufferData.push_back(b);
                }
              }
              if (line.starts_with("map_Kd ")) {
                std::string pathToTexture = line.substr(7);

                size_t lastSlashIndex = pathToTexture.find_last_of("\\");
                std::string imageName =
                    pathToTexture.substr(lastSlashIndex + 1);

                textures.push_back(imageName);
              }
              if (line.empty()) {
                informationRetrieved = true;
              }
            }
          }
        }

        // FINDING PHONG VALUES
      }
    } else {
      std::cerr << "Material file at " << filePath << " could not be loaded."
                << std::endl;
    }
  }
  void LoadModel(std::string name) {
    int indexCounter = 0;
    std::ifstream file("../models/" + name + ".obj");
    if (file) {
      std::string line;
      std::unordered_map<std::string, GLuint> vertexCache; // For unique vertices

      while (std::getline(file, line)) {
        if (line.starts_with("v ")) { // Check if the line starts with "v "
          // FINDING VERTEX POSITIONS
          std::istringstream stream(line.substr(2)); // Skip "v "
          float x, y, z;
          stream >> x >> y >> z; // Parse the three float values
          glm::vec3 vertex(x, y, z);
          tempVerts.push_back(vertex);
          stream.clear();
        } else if (line.starts_with("vn ")) {
          // FINDING VERTEX POSITIONS
          std::istringstream stream(line.substr(3)); // Skip "v "
          float x, y, z;
          stream >> x >> y >> z; // Parse the three float values
          glm::vec3 normal(x, y, z);
          tempNorms.push_back(normal);
          stream.clear();
        } else if (line.starts_with("vt ")) {
          // FINDING UV VALUES
          std::istringstream stream(line.substr(3));
          float u, v;
          stream >> u >> v;
          uvBufferData.push_back(u);
          uvBufferData.push_back(v);
          stream.clear();
        } else if (line.starts_with("mtllib ")) {
          // FINDING MATERIAL FILEPATH
          materialFile = line.substr(7);
        } else if (line.starts_with("usemtl ")) {
          // FINDING MATERIAL INFO
          currentMaterial = line.substr(7);
        } else if (line.starts_with("f ")) {
          // FINDING INDEX VALUES
          std::istringstream stream(line.substr(2));
          std::string indexInfo;

          GLuint v,n;
          int vertexCount = 0;
          std::vector<int> tempIndices;
          std::vector<GLuint> faceIndices;

          while(stream >> indexInfo){
            sscanf(indexInfo.c_str(), "%d/%*d/%d", &v, &n);
            if (vertexCache.find(indexInfo) == vertexCache.end()) {
              vertexBufferData.push_back(tempVerts.at(v - 1).x);
              vertexBufferData.push_back(tempVerts.at(v - 1).y);
              vertexBufferData.push_back(tempVerts.at(v - 1).z);

              normalBufferData.push_back(tempNorms.at(n - 1).x);
              normalBufferData.push_back(tempNorms.at(n - 1).y);
              normalBufferData.push_back(tempNorms.at(n - 1).z);
              vertexCount++;
              vertexCache[indexInfo] = indexCounter++;
            }
            faceIndices.push_back(vertexCache[indexInfo]);
          }

          for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
            indexBufferData.push_back(faceIndices[0]);
            indexBufferData.push_back(faceIndices[i]);
            indexBufferData.push_back(faceIndices[i + 1]);
          }
          LoadMaterial(materialFile, currentMaterial);

        }
      }
    } else {
      std::cerr << "File for " << name << ".obj could not be loaded."
                << std::endl;
    }
    file.close();
  }

  

  void initialise(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;

    // Load model
    LoadModel(name);

    // Load texture
    if(textures.size() > 0) {
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      // Load image
      int width, height, channels;
      unsigned char *data = stbi_load(("../images/" + textures.at(0)).c_str(), &width, &height, &channels, 0);
      if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << ("../images/" + textures.at(0)).c_str() << std::endl;
      }else{
        std::cout << "SSSSSSS" << std::endl;
      }
      stbi_image_free(data);
    }

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(GLfloat),
                 vertexBufferData.data(), GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colourBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glBufferData(GL_ARRAY_BUFFER, colourBufferData.size() * sizeof(GLfloat),
                 colourBufferData.data(), GL_STATIC_DRAW);

    // Create a vertex buffer object to store the vertex normals
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normalBufferData.size() * sizeof(GLfloat),
                 normalBufferData.data(), GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines
    // triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferData.size() * sizeof(GLuint),
                 indexBufferData.data(), GL_STATIC_DRAW);

    // Sort out textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvBufferData.size() * sizeof(GLfloat),
                 uvBufferData.data(), GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders

    //textureID = LoadTextureTileBox("../images/" + textures.at(0));

    programID = LoadShadersFromFile("../src/shaders/asset.vert",
                                    "../src/shaders/asset.frag");

    if (programID == 0) {
      std::cerr << "Failed to load the \"asset\" shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
  }

  void rotate(glm::vec3 rotation, float angle){
    this->rotation = rotation;
    this->angle = angle;
  }

  void render(glm::mat4 cameraMatrix, glm::vec3 localPos) {
    glUseProgram(programID);

    // Handle Scene Buffers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);

    int textureLocation = glGetUniformLocation(programID, "texture1");
    glUniform1i(textureLocation, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position-localPos);
    modelMatrix = glm::scale(modelMatrix, scale);

    modelMatrix = glm::rotate(modelMatrix, angle, rotation);

    glm::mat4 mvp = cameraMatrix * modelMatrix;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);


    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_TRIANGLES,           // mode
                   indexBufferData.size(), // number of indices
                   GL_UNSIGNED_INT,        // type
                   (void *)0               // element array buffer offset
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

#endif // FINAL_OBJLOADER_H
