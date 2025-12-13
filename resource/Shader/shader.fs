#version 460 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

struct Material
{
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D texture_diffuse3;
  sampler2D texture_specular1;
  sampler2D texture_specular2;
  float shininess;
};

uniform Material material;

struct DirectionalLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct SpotLight
{
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

#define NR_POINT_LIGHTS 4

uniform DirectionalLight sunLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight flashLight;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  // vec3 norm = normalize(Normal);
  // vec3 viewDir = normalize(viewPos - FragPos);

  // vec3 fragRGB = CalcDirectionalLight(sunLight, norm, viewDir);

  // for (int i = 0; i < NR_POINT_LIGHTS; i++)
  // {
  //   fragRGB += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
  // }

  // fragRGB += CalcSpotLight(flashLight, norm, FragPos, viewDir);

  // FragColor = vec4(fragRGB, 1.0);

  FragColor = texture(material.texture_diffuse1, TexCoords);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
  // ambient
  vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

  // diffuse
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 specular = light.specular * (pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * texture(material.texture_specular1, TexCoords).rgb);

  return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  // Point light
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb * attenuation;

  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb  * attenuation;

  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 specular = light.specular * (pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * texture(material.texture_specular1, TexCoords).rgb) * attenuation;

  return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 specular = light.specular * (pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * texture(material.texture_specular1, TexCoords).rgb);
  
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse *= intensity;
  specular *= intensity;

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}