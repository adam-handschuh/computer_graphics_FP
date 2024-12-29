#version 330 core

in vec3 fragmentColor;
in vec3 vertPos;
in vec4 clipSpace;
in vec2 imgTexCoord;

uniform sampler2D refSampler;
uniform sampler2D imgSampler;

out vec4 finalColor;

void main(){
    //Perspective division
    vec2 ndc = ((clipSpace.xy/clipSpace.w)/2.0) + 0.5;

    vec2 reflectCoords = vec2(1.0-ndc.x, ndc.y);
    //Final output
    finalColor = vec4(mix(texture(imgSampler,imgTexCoord).rgb,texture(refSampler, reflectCoords).rgb,0.70),1.0f);
}