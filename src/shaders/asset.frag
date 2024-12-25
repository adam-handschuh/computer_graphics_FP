#version 330 core

in vec3 fragmentColor;
in vec3 vertPos;
in vec3 vertNorm;
in vec2 texCoord;

out vec4 finalColor;
uniform sampler2D texSampler;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

void main(){
    //Light Calculations
    vec3 lightDir = normalize(lightPosition - vertPos);
    vec3 normal = normalize(vertNorm);
    float cosTheta = max(dot(normal,lightDir),0.0f);
    float distance = length(lightPosition-vertPos);
    float attenuation = 2/((distance * distance));


    //UV must be flipped for texture due to phenomenon of .objs being exported with flipped UVs
    vec2 flippedTexUV = vec2(texCoord.x, 1.0-texCoord.y);


    //Final colour*texture
    finalColor = vec4(texture(texSampler,flippedTexUV).rgb*cosTheta*lightIntensity*attenuation,1.0f);
}