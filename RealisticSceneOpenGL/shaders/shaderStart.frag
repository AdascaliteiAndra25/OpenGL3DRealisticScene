#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform int directionalLightEnabled; 
uniform int pointLightEnabled;
uniform int spotLightEnabled;

uniform int fogEnabled;


float shininess = 40.0f;

struct Light {    
    vec3 position;
    vec3 direction; //pentru spot light
    float cutOff;   // pentru spot light
    float outerCutoff; //pentru spot light
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform Light pointLights[5];
uniform Light spotLights[2];


vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;


vec3 spotLightColor = vec3(10.0f,10.0f,10.0f);

void computeDirLight()
{
     if (directionalLightEnabled == 1) { 
        //compute eye space coordinates
        vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
        vec3 normalEye = normalize(normalMatrix * fNormal);

        //normalize light direction
        vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

        //compute view direction (in eye coordinates, the viewer is situated at the origin
        vec3 viewDir = normalize(- fPosEye.xyz);

        //compute ambient light
        ambient = ambientStrength * lightColor;

        //compute diffuse light
        diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

        //compute specular light
        vec3 reflectDir = reflect(-lightDirN, normalEye);
        float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
        specular = specularStrength * specCoeff * lightColor;
        
        } else {
             ambient = vec3(0.0f);
            diffuse = vec3(0.0f);
            specular = vec3(0.0f);
            
        }
}


vec3 computePointLight(Light light, sampler2D diffuseTexture, sampler2D specularTexture)
{
    //compute eye space coordinates
    vec4 fPosEye = vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //compute ambient light
    vec3 ambient= light.ambient * texture(diffuseTexture, fTexCoords).rgb;

    //compute diffuse light
    vec3 lightDirN = normalize(light.position - fPosEye.xyz);
    float diffCoeff = max(dot(normalEye, lightDirN), 0.0f);
	vec3 diffuse = light.diffuse * (diffCoeff * texture(diffuseTexture, fTexCoords).rgb);

    //compute specular light
    vec3 viewDirN = normalize(light.position - fPosEye.xyz);
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f),shininess);
    vec3 specular = light.specular * (specCoeff * texture(specularTexture, fTexCoords).rgb);

    //calcualte distance from light to fragment and attenation
	float distance = length(light.position - fPosEye.xyz);
	float attenation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //attenuate each component
    ambient *=  attenation;
    diffuse *= attenation;
    specular *= attenation;

    return min((ambient + diffuse + specular), 1.0f);
}

vec3 computeSpotLight(Light light, sampler2D diffuseTexture, sampler2D specularTexture){
	vec3 cameraPosEye = vec3(0.0f);
    vec4 fPosEye = vec4(fPosition, 1.0f);

	vec3 lightDir = normalize(light.position - fPosEye.xyz);
	vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir,0.0f)));
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float diff = max(dot(fNormal, lightDir), 0.0f);
	float spec = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	float distance = length(light.position - fPosition);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff)/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * light.ambient * vec3(texture(diffuseTexture, fTexCoords));
	vec3 diffuse = spotLightColor * light.diffuse * diff * vec3(texture(diffuseTexture, fTexCoords));
	vec3 specular = spotLightColor * light.specular * spec * vec3(texture(specularTexture, fTexCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
    return ambient + diffuse + specular;
}




float computeFog()
{
vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

 float fogDensity = 0.05f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
 return clamp(fogFactor, 0.0f, 1.0f);
}



void main() {

    computeDirLight();

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
   
  
   if(pointLightEnabled == 1){
    for (int i = 0; i < 5; ++i) {
        color += computePointLight(pointLights[i], diffuseTexture, specularTexture);
     }
    }

    if(spotLightEnabled == 1){
    for (int i = 0; i <= 1; ++i) {
        color += computeSpotLight(spotLights[i], diffuseTexture, specularTexture);
        }
    }
     if(fogEnabled == 1){
           float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);

        } else {
        fColor = vec4(color, 1.0f);
    }



}
