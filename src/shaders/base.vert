#version 330 core

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

uniform mat4 MVP;
uniform float time;


out vec3 fragmentColor;
out vec3 vertPos;
out vec4 clipSpace;
out vec2 imgTexCoord;

void main(){
    clipSpace = MVP *vec4(vertexPosition, 1);
    gl_Position = clipSpace;
    fragmentColor = vertexColor;
    vertPos = vertexPosition;
    imgTexCoord = vertexUV + vec2(time * 0.01, 0.0);
}