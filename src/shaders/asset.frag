#version 330 core

in vec3 fragmentColor;
in vec3 vertPos;
in vec3 vertNorm;
in vec2 texCoord;

out vec4 finalColor;
uniform sampler2D texture1;

void main(){
    finalColor = vec4(fragmentColor, 1.0f);
}