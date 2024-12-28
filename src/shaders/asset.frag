#version 330 core

in vec3 fragmentColor;
in vec4 vertPos;
in vec3 vertNorm;
in vec2 texCoord;
in vec3 worldPosition;
in vec4 shadowCoord;

out vec4 finalColor;
uniform sampler2D texSampler;
uniform sampler2D shadowSampler;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

void main(){
    //Shadow Calculations
    vec3 projCoords = shadowCoord.xyz/shadowCoord.w;
    vec2 UVCoords;
    //Adjustments because of NDC space
    UVCoords.x = 0.5 * projCoords.x + 0.5;
    UVCoords.y = 0.5 * projCoords.y + 0.5;
    float depth = 0.5 * projCoords.z + 0.5;
    float existingDepth = texture(shadowSampler,UVCoords).r;

    //Shadow Mapping
    float visibility = 1.0;
    float bias = 1e-3;
    float distanceS = length(lightPosition-worldPosition);
    /*This is to check if the area is within the view of the light source
    (fixes some weird bug where dark area behind and around light)*/
    if (UVCoords.x >= 0.0 && UVCoords.x <= 1.0 &&
    UVCoords.y >= 0.0 && UVCoords.y <= 1.0 &&
    depth <= 1.0 && depth >= 0.0){
        //Checking for shadows
        if (existingDepth + bias <= depth){
            visibility = 0.2/(distanceS/(distanceS/1.0001));
        }
    }

    //Light Calculations
    vec3 lightDir = normalize(lightPosition - vertPos.xyz);
    vec3 normal = normalize(vertNorm);
    float cosTheta = max(dot(normal,lightDir),0.0f);
    float distance = length(lightPosition-vertPos.xyz);
    float attenuation = 1/(distance);


    //UV must be flipped for texture due to phenomenon of .objs being exported with flipped UVs
    vec2 flippedTexUV = vec2(texCoord.x, 1.0-texCoord.y);


    //Final colour*texture
    finalColor = vec4((texture(texSampler,flippedTexUV).rgb * cosTheta*lightIntensity*attenuation)*vec3(visibility),1.0f);
}