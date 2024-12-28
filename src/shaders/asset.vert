#version 330 core

layout (location=0) in vec3 vertexPosition;
layout (location=1) in vec3 vertexColor;
layout (location=2) in vec3 vertexNormal;
layout (location=3) in vec2 aTexCoord;

uniform mat4 MVP;
uniform mat4 MLP;

out vec3 fragmentColor;
out vec4 vertPos;
out vec3 worldPosition;
out vec3 vertNorm;
out vec2 texCoord;
out vec4 shadowCoord;

void main(){
    gl_Position = MVP * vec4(vertexPosition, 1);
    shadowCoord = MLP * vec4(vertexPosition,1);

    fragmentColor = vertexColor;
    vertPos = MVP * vec4(vertexPosition, 1);
    vertNorm = vertexNormal;
    texCoord = aTexCoord;

    vec4 temp = (MVP * vec4(vertexPosition, 1));
    worldPosition = temp.xyz;
}