#version 450 core
 
out vec4 FragColor;

in vec4 position;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

//in vec3 tangentLightPos;
//in vec3 tangentViewPos;
//in vec3 tangentFragPos;

struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
}; 
  
//uniform Material material;
//uniform vec3 lightPos;
uniform vec3 viewPos;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D roughnessMap;
layout(binding = 3) uniform sampler2D metallicMap;

// calculates the color when using a directional light.
vec3 CalcDirLight(Material light, vec4 diffuseTexture, vec3 direction, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shininess);
    // combine results
    vec3 ambient = light.ambient *			vec3(diffuseTexture);
    vec3 diffuse = light.diffuse * diff *	vec3(diffuseTexture);
    vec3 specular = light.specular * spec * vec3(diffuseTexture);
    return (ambient + diffuse + specular);
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, texCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vec3(position));
    vec3 Q2  = dFdy(vec3(position));
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N   = normal;//normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{

    Material material;
	material.ambient   = vec3(.01);
	material.diffuse   = vec3(.5);
	material.specular  = vec3(.3);
	material.shininess = 8.;

    vec3 lightPos;
	lightPos = vec3(-2.0, 4.0, -1.0);

	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	//FragColor = vec4(normal, 1.0);
	//FragColor = mix(texture(albedo, texCoord), texture(albedo1, texCoord), 0.2);
	// ambient
	vec4 diffTex = texture(diffuseMap, texCoord);

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    vec3 ambient = lightColor * material.ambient;
  	
	// NORMAL MAPPING TEST

	// obtain normal from normal map in range [0,1]
    //vec3 norm = texture(normalMap, texCoord).rgb;
    //// transform normal vector to range [-1,1]
    //norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
	//
	//// get diffuse color
    //vec3 color = texture(diffuseMap, texCoord).rgb;
    //// diffuse
    //vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    //float diff = max(dot(lightDir, norm), 0.0);
    //vec3 diffuse = diff * color;
    //// specular
    //vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    //float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
	//
    //vec3 specular = vec3(0.2) * spec;
	vec3 norm = normal;

	vec3 viewDir = normalize(viewPos - position.xyz);
	
	vec3 result = CalcDirLight(material, diffTex, vec3(-0.2, -1.0, -0.3), norm, viewDir);
	
    vec3 lightDir = normalize(lightPos - position.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * diffTex.xyz);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
        
    result += ambient + diffuse + specular;


	FragColor = vec4(result, 1.0);
}