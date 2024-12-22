#version 330 core

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec3 vertexColor;
layout (location=2) in vec3 vertexNormal;
layout (location=3) in vec2 aTexCoord;

uniform mat4 MVP;

out vec3 fragmentColor;
out vec3 vertPos;
out vec3 vertNorm;
out vec2 texCoord;

void main(){
    gl_Position = MVP * vec4(vertexPosition, 1);
    fragmentColor = vertexColor;
    vertPos = vertexPosition;
    vertNorm = vertexNormal;
    texCoord = aTexCoord;
}