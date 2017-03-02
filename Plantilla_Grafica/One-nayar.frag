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
uniform sampler2D texture1;

in vec3 o_normal;
in vec3 o_toLight;
in vec3 o_toCamera;
in vec2 Texcoord;


out vec4 resultingColor;

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
vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V)
{
   float specularTerm = 0;

   // calculate specular reflection only if
   // the surface is oriented to the light source


   if(dot(N, L) > 0)
   {
	  vec3 reflectDir = reflect(-L, N);
	  float specAngle = max(dot(reflectDir,V), 0.0);
	  specularTerm = pow(specAngle,u_matShininess/4.0);      
   }

   return specularTerm * u_matSpecularReflectances * u_lightSpecularIntensitys;
}

void main(void)
{
   // normalize vectors after interpolation
   vec3 L = normalize(o_toLight);  // deberia ser normalize(u_toLightDirection); porque son todas los vectores que inciden sobre el vertice
   vec3 N = normalize(o_normal);
   vec3 V = normalize(o_toCamera);
   vec3 R = normalize(reflect(L, N));
   const float PI = 3.14159;


    // calculate intermediary values
    float NdotL = dot(N, L);
    float NdotV = dot(N, o_toCamera); 

    float angleVN = acos(NdotV);
    float angleLN = acos(NdotL);
    
    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(o_toCamera - N * dot(o_toCamera, N), L - N * dot(L, N));
    
    float roughnessSquared = roughness * roughness;
    float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));
	
	 // calculate C1, C2 and C3
    float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
    float C2 = 0.45 * roughnessSquared9;
    
    if(gamma >= 0.0)
    {
        C2 *= sin(alpha);
    }
    else
    {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }
 
    float powValue = (4.0 * alpha * beta) / (PI * PI);
    float C3  = 0.125 * roughnessSquared9 * powValue * powValue;
 
    // now calculate both main parts of the formula
    float A = gamma * C2 * tan(beta);
    float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);
 
    // put it all together
    float L1 = max(0.0, NdotL) * (C1 + A + B);
    
    // also calculate interreflection
    float twoBetaPi = 2.0 * beta / PI;
	//TODO: p is squared in this case... how to separate this?
    float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);
    
    // get the final color by summing both components
    vec3 finalValue = diffuseLighting(N, L) * (L1 + L2);

	resultingColor = vec4(finalValue, 1.0);


   // get Blinn-Phong reflectance components
   //vec3 Iamb =  ambientLighting();
   //vec3 Idif = diffuseLighting(N, L);
   //vec3 Ispe = specularLighting(N, L, V);

   // combination of all components and diffuse color of the object
   //resultingColor.xyz = Iamb + Idif + Ispe;
   //resultingColor.a = 1.0;
	//resultingColor = vec4(Texcoord,0.0,1.0);
   //resultingColor =  texture(texture1, Texcoord);





} 