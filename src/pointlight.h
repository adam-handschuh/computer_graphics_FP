//
// Created by Adam on 25/12/2024.
//
#include <config.h>

#ifndef FINAL_POINTLIGHT_H
#define FINAL_POINTLIGHT_H

struct PointLight {
  // Lighting control
  const glm::vec3 wave500 = glm::vec3(0.0f, 128.0f, 255.0f);
  const glm::vec3 wave600 = glm::vec3(100.0f, 100.0f, 200.0f);
  const glm::vec3 wave700 = glm::vec3(50.0f, 0.0f, 150.0f);
  glm::vec3 lightIntensity = 0.1f * (5.0f * wave500 + 8.0f * wave600 + 25.0f * wave700);
  glm::vec3 lightPosition = glm::vec3(-150.0f, 500.0f, -150.0f);
  glm::vec3 lightUp = glm::vec3(0, 0, -1);
  float depthFoV = 100.0f;
  float depthNear = 100.0f;
  float depthFar = 6000.0f;
  float shadowMapWidth, shadowMapHeight;

  PointLight(float shadowMapWidth, float shadowMapHeight){
    this->shadowMapWidth = shadowMapWidth;
    this->shadowMapHeight = shadowMapHeight;
  }

  glm::mat4 lightPerspMatrix(){
    glm::mat4 light_projection_matrix;
    light_projection_matrix = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth/shadowMapHeight, depthNear, depthFar);
    return light_projection_matrix;
  }

  glm::mat4 lightSpaceView(){
    glm::mat4 lightViewMat = glm::lookAt(lightPosition,  lightPosition + glm::vec3 (0.0f,-1.0f,0.0f),lightUp);
    return lightViewMat;
  }

  glm::mat4 lightMatrix(){
    return lightPerspMatrix() * lightSpaceView();
  }
};

#endif // FINAL_POINTLIGHT_H
