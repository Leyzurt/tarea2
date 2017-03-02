#version 330
// parameters of the light and possible values
uniform vec3 u_lightAmbientIntensitys;
uniform vec3 u_lightDiffuseIntensitys;
uniform vec3 u_lightSpecularIntensitys;
// parameters of the material and possible values
uniform vec3 u_matAmbientReflectances;
uniform vec3 u_matDiffuseReflectances;
uniform vec3 u_matSpecularReflectances;
uniform float u_matShininess;
uniform float u_roughness;
uniform float u_fresnel;
uniform float u_albedo;
//ACTIVES
uniform float u_specular;
uniform float u_difuse;
uniform float u_texture_active;
//FIN ACTIVES
uniform float u_atConstant;
uniform float u_atLineal;
uniform float u_atCuadratic;
uniform float u_exponent;
uniform float u_spotcos;
uniform sampler2D texture1;
uniform sampler2D texture_n;
uniform sampler2D shadowMap;
uniform vec3 u_lightDir;
uniform vec3 u_type_light;
uniform mat4 u_depth_matrix;
uniform float u_activeReflect;
uniform float u_texture_active_normal;

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 m_texture;
in layout(location = 2) vec3 i_normal;
in layout(location = 3) vec3 tangent;

uniform vec3 position_vector;
uniform vec3 normal_vector;
uniform mat4 u_viewMat;
uniform mat4 u_normalMat;
uniform mat4 u_projMat;
uniform mat4 u_modelMat;
uniform vec3 u_lightPosition;
uniform vec3 eye_position;
//UNIFORM FOR CUBE MAP -- ALL FOR CUBEMAP
uniform samplerCube cubeMap;
in vec3 Normal_model;
in vec3 Position_model;
uniform float u_ratioRefract;
uniform float u_activeRefract;
//-----------------------------------------

in vec3 o_normal;
in vec3 o_toLight;
in vec3 o_toLightPuntual;
in float dist;
in vec3 o_toCamera;
in vec2 Texcoord;

out vec4 color;

// returns intensity of reflected ambient lighting
vec3 ambientLighting()
{
	return u_matAmbientReflectances * u_lightAmbientIntensitys;
}
// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 N, in vec3 L)
{
	// calculation as for Lambertian reflection
	return max(dot(N, L), 0.0) * u_matDiffuseReflectances * u_lightDiffuseIntensitys;
}
// returns intensity of specular reflection
vec3 specularLighting_phong(in vec3 N, in vec3 L, in vec3 V)
{
	float specularTerm = 0.0;
	vec3 reflectDir = reflect(-L, N);
	float specAngle = max(dot(reflectDir, V), 0.0);
	specularTerm = pow(specAngle, u_matShininess);
	return specularTerm * u_matSpecularReflectances * u_lightSpecularIntensitys;


}

// returns intensity of specular reflection
vec3 specularLighting_blinn(in vec3 N, in vec3 L, in vec3 V)
{
	float specularTerm = 0.0;
	vec3 H = normalize(L + V);
	specularTerm = max(pow(dot(N, H), u_matShininess), 0.0);
	return specularTerm * u_matSpecularReflectances * u_lightSpecularIntensitys;

}

vec3 one_nayar(in vec3 N, in vec3 L, in vec3 V)
{
	// calculate intermediary values
	float NdotL = dot(N, L);
	float NdotV = dot(N, V);
	float angleVN = acos(NdotV);
	float angleLN = acos(NdotL);
	float alpha = max(angleVN, angleLN);
	float beta = min(angleVN, angleLN);
	float gamma = dot(V - N * dot(V, N), L - N * dot(L, N));
	float roughnessSquared = u_roughness * u_roughness;
	float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));
	// calculate C1, C2 and C3
	float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
	float C2 = 0.45 * roughnessSquared9;
	const float PI = 3.14159;
	if (gamma >= 0.0)
	{
		C2 *= sin(alpha);
	}
	else
	{
		C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
	}
	float powValue = (4.0 * alpha * beta) / (PI * PI);
	float C3 = 0.125 * roughnessSquared9 * powValue * powValue;

	// now calculate both main parts of the formula
	float A = gamma * C2 * tan(beta);
	float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

	// put it all together
	float L1 = max(0.0, NdotL) * (C1 + A + B);

	// also calculate interreflection
	float twoBetaPi = 2.0 * beta / PI;
	//TODO: p is squared in this case... how to separate this?
	float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);
	return u_albedo * u_lightDiffuseIntensitys * (L1 + L2);
}

vec3 cook_torrance(in vec3 N, in vec3 L, in vec3 V)
{
	float NdotL = max(dot(N, L), 0.0);
	float specular = 0.0;
	float F0 = u_fresnel;
	if (NdotL > 0.0)
	{
		vec3 eyeDir = normalize(V);

		// calculate intermediary values
		vec3 halfVector = normalize(L + V);
		float NdotH = max(dot(N, halfVector), 0.0);
		float NdotV = max(dot(N, V), 0.0); // note: this could also be NdotL, which is the same value
		float VdotH = max(dot(V, halfVector), 0.0);
		float mSquared = u_roughness * u_roughness;

		// geometric attenuation
		float NH2 = 2.0 * NdotH;
		float g1 = (NH2 * NdotV) / VdotH;
		float g2 = (NH2 * NdotL) / VdotH;
		float geoAtt = min(1.0, min(g1, g2));

		// roughness (or: microfacet distribution function)
		// beckmann distribution function
		float r1 = 1.0 / (4.0 * mSquared * pow(NdotH, 4.0));
		float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
		float roughness = r1 * exp(r2);

		// fresnel
		// Schlick approximation
		float fresnel = pow(1.0 - VdotH, 5.0);
		fresnel *= (1.0 - F0);
		fresnel += F0;

		specular = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
	}
	//u_matSpecularReflectances * u_lightSpecularIntensitys;
	return u_lightSpecularIntensitys * NdotL * (0.2 + specular * (1.0 - 0.2));

}

vec3 difuse(in vec3 N, in vec3 L, in vec3 V) {
	if (u_difuse == 0.0) {
		return diffuseLighting(N, L); //LAMBERT
	}
	else {
		return one_nayar(N, L, V);
	}
}

vec3 specular(in vec3 N, in vec3 L, in vec3 V) {
	if (u_specular == 0.0) {
		return cook_torrance(N, L, V);
	}
	else if (u_specular == 1.0) {
		return specularLighting_phong(N, L, V);
	}
	else {
		return specularLighting_blinn(N, L, V);
	}
}

/*vec2 ParallaxMapping(){
    float height =  texture(shadowMap, Texcoord).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * 100.0);
    return Texcoord - p; 
}*/


void main(void)
{
	vec3 T = normalize(vec3(u_modelMat * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(u_modelMat * vec4(i_normal, 0.0)));
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	

   vec4 worldPosition = u_modelMat * vec4(position,1.0);
   vec4 DepthCoord = u_depth_matrix * worldPosition;
   //Normal
   vec3 o_normal = normalize( vec3( u_modelMat * vec4(i_normal, 0.0)) );
	//direction to light	
   vec3 o_toLight = normalize( u_lightPosition  ); 
   //direction to camera
   vec3 VP;
   VP = vec3( u_lightPosition - worldPosition.xyz );
   float dist = length(VP);
   vec3 o_toLightPuntual = normalize(VP);

   vec3 o_toCamera = normalize( eye_position );
   //screen space coordinates of the vertex
  

   // normalize vectors after interpolation
   vec3 L = normalize(o_toLight);  // deberia ser normalize(u_toLightDirection); porque son todas los vectores que inciden sobre el vertice

   vec2 texcoord = Texcoord;
   if (u_texture_active_normal == 1.0) {
      // vec2 texcoord = ParallaxMapping();
	   N = normalize(((2.0 * (texture(texture_n, texcoord).rgb) - vec3(1.0, 1.0, 1.0))));
      // N = normalize(o_normal);
   }
   else {
	   N = normalize(o_normal);
   }
   vec3 V = normalize(o_toCamera); 
   

   vec3 finalValue = vec3(0.0, 0.0, 0.0);

   if (u_type_light.x == 1.0) { //DIRECCIONAL
	   L = vec3(u_lightDir.x, u_lightDir.y, u_lightDir.z);
	   float NdotL = max(dot(N, L), 0.0);
	   if (NdotL > 0.0) {
		   finalValue += difuse(N, L, V);
		   finalValue += specular(N, L, V);
		   finalValue += ambientLighting();
	   }

   }
   else if (u_type_light.y == 1.0) { //PUNTUAL
	   L = normalize(o_toLightPuntual);
	   float NdotL = max(dot(N, L), 0.0);
	   if (NdotL > 0.0) {
		   float at = 1.0 / (u_atConstant + u_atLineal*dist + u_atCuadratic * dist * dist);
		   finalValue += at * (difuse(N, L, V));
		   finalValue += at * (specular(N, L, V));
		   finalValue += ambientLighting();
	   }

   }
   else {						//SPOT
	   L = normalize(o_toLightPuntual);
	   float NdotL = max(dot(N, L), 0.0);
	   if (NdotL > 0.0) {
		   float spotEffect = dot(normalize(u_lightDir), normalize(-L));
		   if (spotEffect > u_spotcos) {
			   spotEffect = pow(spotEffect, u_exponent);
			   float at = spotEffect / (u_atConstant + u_atLineal*dist + u_atCuadratic * dist * dist);
			   finalValue += at * (difuse(N, L, V));
			   finalValue += at * (specular(N, L, V));
			   finalValue += ambientLighting();
		   }
	   }
   }

   if (u_texture_active == 1.0) {
	   finalValue *= texture(texture1, texcoord).rgb;
   }


    if(u_activeRefract == 1.0f){
   //FOR CUBE MAP REFLECTION:
	   vec3 I = normalize(normalize(Position_model) - o_toCamera);
  	   vec3 R = refract(I, normalize(Normal_model), 1.0 / u_ratioRefract);
	   finalValue = texture(cubeMap, R).rgb;
   }
   else if (u_activeReflect == 1.0f) {
	   vec3 I = normalize(normalize(Position_model) - o_toCamera);
	   vec3 R = reflect(I, normalize(Normal_model));
	   finalValue = texture(cubeMap, R).rgb;
   }


 //  resultingColor = texture( shadowMap, DepthCoord.xy );
  //finalValue = N;

color = vec4( finalValue, 1.0);
float bias = 0.005;

float shadow = 1.0;
vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
for(int x = -1; x <= 1; x++)
{
    for(int y = -1; y <= 1; y++)
    {
        float pcfDepth = texture(shadowMap, DepthCoord.xy + vec2(x, y) * texelSize).r; 
        if ( pcfDepth  <  DepthCoord.z-bias ){
            shadow-=0.1;
        }         
    }    
}
//shadow /= 9.0;
	color = color * shadow;
   //color = finalValue;
    gl_Position = u_projMat * u_viewMat *  worldPosition;
} 








