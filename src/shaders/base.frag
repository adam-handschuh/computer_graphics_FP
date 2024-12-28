#version 330 core

in vec3 fragmentColor;
in vec3 vertPos;
in vec3 vertNorm;

out vec4 finalColor;

void main(){
    vec3 temp = vec3(0.0f,100.0f,0.0f);
    vec3 lightDir = normalize(temp-vertPos);
    vec3 normal = normalize(vertNorm);
    float cosTheta = max(dot(normal,lightDir),0.0f);

    //Lambertian (Enhancement stuff)
    float distance = length(vertPos.xz);
    float attenuation = (0.1/(distance));
    //Tone-Mapping
    vec3 c = fragmentColor / (fragmentColor + vec3(1.0));
    //Gamma-Correction
    vec3 cGamma = pow(c, vec3(1.0 / 2.2));

    //Lambertian (Output stuff with shadows)
    finalColor = vec4((cGamma*cosTheta*attenuation),1.0f);
//    finalColor = vec4(fragmentColor,1.0f);
}