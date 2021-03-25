// Lights
struct DirLight {
	float intensity;

    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; //-
};

struct PointLight {
    vec3 position;
    
    float constant;//-
    float linear;//-
    float quadratic;//-
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;//-
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
