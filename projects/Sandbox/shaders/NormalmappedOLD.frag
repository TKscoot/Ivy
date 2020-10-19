#version 450 core
 
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

// calculates the color when using a directional light.
vec3 CalcDirLight(Material light, vec3 diffuseTexture, vec3 direction, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient *			diffuseTexture;
    vec3 diffuse = light.diffuse * diff *	diffuseTexture;
    vec3 specular = light.specular * spec * diffuseTexture;
    return (ambient + diffuse + specular);
}


void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0;
    // transform normal vector to range [-1,1]
    normal = normalize(normal);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;




	vec3 lightColor = vec3(1.0, 1.0, 1.0);
    //// ambient
    //vec3 ambient = 0.1 * color;
    //// diffuse
    //vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.FragPos);
    //float diff = max(dot(normal, lightDir), 0.0);
    //vec3 diffuse = diff * color;
    //// specular
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); 
	//vec3 specular = vec3(0.2) * spec;






	vec3 viewDir = normalize(viewPos - fs_in.FragPos.xyz);
	
	vec3 result = CalcDirLight(material, color, vec3(-0.2, -1.0, -0.3), normal, viewDir);
	
    vec3 lightDir = normalize(lightPos - fs_in.FragPos.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * color.xyz);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
        
    result += /*ambient + */diffuse + specular;

	FragColor = vec4(result, 1.0);






    //FragColor = vec4(color, 1.0) * vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(normal, 1.0);
}
