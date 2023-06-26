#version 330 core

// Data from vertex shader.
in vec3 iPosWorld;
in vec3 iNormalWorld; 
in vec2 iTexcorrd;

// --------------------------------------------------------
// Add your uniform variables.
// --------------------------------------------------------
uniform sampler2D mapKd;
uniform int haveMapKd;
// Transformation matrices.
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;
// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
// Light data.
uniform vec3 cameraPos;
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;

uniform vec3 spotLightPos;
uniform vec3 spotLightIntensity;
uniform vec3 spotLightDirection;
uniform float spotLightTotalWidthInDegree;
uniform float spotLightCutoffStartInDegree;
//--------------------------------------------------------
out vec4 FragColor;


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 eyeDir, vec3 ReflectDir, float Ns)
{
    return Ks * I * pow(max(0, dot(eyeDir, ReflectDir)), Ns);
}

void main()
{
    vec3 texColor = texture2D(mapKd, iTexcorrd).rgb;
    vec3 applyKd;
    if(haveMapKd == 1){
        applyKd = texColor;
    }
    else{
        applyKd = Kd;
    }

    vec3 N = normalize(iNormalWorld);
    vec3 visColor = 0.5 * N + 0.5;
    vec3 viewDir = normalize(cameraPos - iPosWorld);
	// -------------------------------------------------------------
    // Ambient light.
    vec3 ambient = Ka * ambientLight;
    // -------------------------------------------------------------
    // Directional light.
    vec3 vsLightDir = vec4(-dirLightDir, 0.0).xyz;
    // Diffuse.
    //vec3 diffuse = Diffuse(Kd, dirLightRadiance, N, vsLightDir);
    vec3 diffuse = Diffuse(applyKd, dirLightRadiance, N, vsLightDir);
    // Specular.
    vec3 vR = 2 * dot(N, vsLightDir) * N - vsLightDir;
    vR = normalize(vR);
    vec3 specular = Specular(Ks, dirLightRadiance, viewDir, vR, Ns);
    vec3 dirLight = diffuse + specular;
    // -------------------------------------------------------------
    // Point light.
    vec4 tmpPos = vec4(pointLightPos, 1.0);
    vec3 vsLightPos = tmpPos.xyz / tmpPos.w;
    vsLightDir = normalize(vsLightPos - iPosWorld);
    float distSurfaceToLight = distance(vsLightPos, iPosWorld);
    float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
    vec3 radiance = pointLightIntensity * attenuation;
    // Diffuse.
    //diffuse = Diffuse(Kd, radiance, N, vsLightDir);
    diffuse = Diffuse(applyKd, radiance, N, vsLightDir);
    // Specular.
    vR = 2 * dot(N, vsLightDir) * N - vsLightDir;
    vR = normalize(vR);
    specular = Specular(Ks, radiance, viewDir, vR, Ns);
    vec3 pointLight = diffuse + specular;
    // -------------------------------------------------------------
    // Spot light.
    tmpPos = vec4(spotLightPos, 1.0);
    vsLightPos = tmpPos.xyz / tmpPos.w;
    vsLightDir = normalize(vsLightPos - iPosWorld);
    distSurfaceToLight = distance(vsLightPos, iPosWorld);
    float theta = dot(spotLightDirection, vsLightDir);
    float epsilon = spotLightTotalWidthInDegree - spotLightCutoffStartInDegree;
    attenuation = clamp((theta-spotLightCutoffStartInDegree) / epsilon,0.0, 1.0) / (distSurfaceToLight * distSurfaceToLight);
    vec3 Intensity = attenuation *  spotLightIntensity;
    //diffuse.
    //diffuse = Diffuse(Kd, Intensity, N, vsLightDir) ;
    diffuse = Diffuse(applyKd, Intensity, N, vsLightDir) ;
    //specular
    vR = 2 * dot(N, vsLightDir) * N - vsLightDir;
    specular = Specular(Ks, Intensity, viewDir, vR, Ns);
    vec3 spotLight = diffuse + specular;

    vec3 LightingColor = ambient + dirLight + pointLight + spotLight;
    //FragColor = vec4(N, 1.0);
    FragColor = vec4(LightingColor, 1.0);
}
