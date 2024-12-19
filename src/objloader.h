//
// Created by Adam on 17/12/2024.
//

#ifndef FINAL_OBJLOADER_H
#define FINAL_OBJLOADER_H
#include<config.h>
struct Asset{

  std::string name;
  std::string materialFile;
  std::string currentMaterial;

  std::vector<glm::vec3> tempVerts;
  std::vector<GLuint> tempIndices;

  std::vector<GLfloat> vertexBufferData;
  std::vector<GLfloat> colourBufferData;
  std::vector<GLuint> indexBufferData;


  GLuint vertexArrayID;
  GLuint vertexBufferID;
  GLuint indexBufferID;
  GLuint colourBufferID;
  GLuint programID;
  GLuint mvpMatrixID;

  glm::vec3 position;
  glm::vec3 scale;

  Asset(std::string name){
    this->name = name;
  }

  void LoadMaterial(std::string filePath, std::string materialName){
    std::ifstream file("../models/" + filePath);
    if(file){
      std::string line;
      while(std::getline(file,line)){

        if(line.starts_with("newmtl ")){
          if(line.substr(7) == materialName){
            //MATERIAL FOUND
            bool informationRetrieved = false;
            while(!informationRetrieved && std::getline(file,line)) {
              //FINDING DIFFUSE (BASIC RGB) VALUES
              if (line.starts_with("Kd")) {
                std::istringstream stream(line.substr(3)); // Skip "v "
                GLfloat r, g, b;
                stream >> r >> g >> b;
                for(int i = 0; i < 4; i ++) {
                  colourBufferData.push_back(r);
                  colourBufferData.push_back(g);
                  colourBufferData.push_back(b);
                }
                std::cout << "Colour Data : " << std::endl << r << " " << g << " " << b << " | " << materialName << std::endl;
                informationRetrieved = true;
              }
            }
          }
        }

        //FINDING PHONG VALUES
      }
    }else{
      std::cerr << "Material file at " << filePath << " could not be loaded." << std::endl;
    }
  }
  void LoadModel(std::string name) {
    std::ifstream file("../models/" + name + ".obj");
    if (file) {
      std::string line;
      while (std::getline(file, line)) {
        //FINDING VERTEX POSITIONS
        if (line[0] == 'v' && line[1] == ' ') { // Check if the line starts with "v "
          std::istringstream stream(line.substr(2)); // Skip "v "
          float x, y, z;
          stream >> x >> y >> z; // Parse the three float values
          glm::vec3 vertex(x,y,z);
          tempVerts.push_back(vertex);
          stream.clear();
          //FINDING MATERIAL FILEPATH
        }else if(line.starts_with("mtllib ")){
          materialFile = line.substr(7);
          //FINDING MATERIAL INFO
        }else if(line.starts_with("usemtl ")){
          currentMaterial = line.substr(7);
          //FINDING INDEX VALUES
        }else if(line.starts_with("f ")){
          std::string indexInfo = line.substr(2);

          GLuint a,b,c,d; //4-vertices of a quad as detailed by .obj formats
          sscanf(indexInfo.c_str(), "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &a, &b, &c, &d);
          //create 2 triangles to make a quad in form of ABC & ACD (with -1 to offset .obj 1-initial indexing)
          tempIndices.push_back(a-1);
          tempIndices.push_back(b-1);
          tempIndices.push_back(c-1);
          tempIndices.push_back(a-1);
          tempIndices.push_back(c-1);
          tempIndices.push_back(d-1);

          //CREATE VERTEX DATA FOR A FACE
          vertexBufferData.push_back(tempVerts.at(a-1).x);
          vertexBufferData.push_back(tempVerts.at(a-1).y);
          vertexBufferData.push_back(tempVerts.at(a-1).z);

          vertexBufferData.push_back(tempVerts.at(b-1).x);
          vertexBufferData.push_back(tempVerts.at(b-1).y);
          vertexBufferData.push_back(tempVerts.at(b-1).z);

          vertexBufferData.push_back(tempVerts.at(c-1).x);
          vertexBufferData.push_back(tempVerts.at(c-1).y);
          vertexBufferData.push_back(tempVerts.at(c-1).z);

          vertexBufferData.push_back(tempVerts.at(d-1).x);
          vertexBufferData.push_back(tempVerts.at(d-1).y);
          vertexBufferData.push_back(tempVerts.at(d-1).z);


          LoadMaterial(materialFile, currentMaterial);
        }
      }
    } else {
      std::cerr << "File for " << name << ".obj could not be loaded."
                << std::endl;
    }
    file.close();
  }

  void initialise(glm::vec3 position, glm::vec3 scale){
    this->position = position;
    this->scale = scale;

    //Load model
    LoadModel(name);

    for(int i = 0; i < tempIndices.size(); i +=4){
      indexBufferData.push_back(i);
      indexBufferData.push_back(i+1);
      indexBufferData.push_back(i+2);
      indexBufferData.push_back(i);
      indexBufferData.push_back(i+2);
      indexBufferData.push_back(i+3);
    }

    int y = 0;
    for(GLfloat x : vertexBufferData){
      y ++;
      std::cout << x << " ";
      if(y % 3 == 0){
        std::cout << std::endl;
      }
      if(y % 12 == 0){
        std::cout << std::endl;
      }
    }


    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size()*sizeof(GLfloat), vertexBufferData.data(), GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colourBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glBufferData(GL_ARRAY_BUFFER, colourBufferData.size()*sizeof(GLfloat), colourBufferData.data(), GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size()*sizeof(GLuint), indexBufferData.data(), GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../src/shaders/asset.vert","../src/shaders/asset.frag");

    if (programID == 0)
    {
      std::cerr << "Failed to load the \"asset\" shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");

  }

  void render(glm::mat4 cameraMatrix,glm::vec3 cameraPos){
    glUseProgram(programID);

    //Handle Scene Buffers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glEnableVertexAttribArray(2);


    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 cam(cameraPos.x,0,cameraPos.z);
    modelMatrix = glm::translate(modelMatrix, position-cam);
    modelMatrix = glm::scale(modelMatrix, scale);


    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(
        GL_TRIANGLES,                 // mode
        indexBufferData.size(),      // number of indices
        GL_UNSIGNED_INT,               // type
        (void*)0                     // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
  }

  void cleanup(){
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colourBufferID);
    glDeleteBuffers(1, &vertexArrayID);
    glDeleteProgram(programID);
  }
};

#endif // FINAL_OBJLOADER_H
