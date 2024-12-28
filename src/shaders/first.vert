#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 MLP;

void main(){
    gl_Position =  MLP * vec4(vertexPosition,1);
}