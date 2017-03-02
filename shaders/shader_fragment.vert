#version 330
in layout(location=0) vec3 position;
in layout(location=1) vec2 m_texture;
in layout(location=2) vec3 i_normal;
in layout(location=3) vec3 tangent;
//in layout(location=4) vec3 bitangent;
uniform vec3 position_vector;
uniform vec3 normal_vector;
uniform mat4 u_viewMat;
uniform mat4 u_normalMat;
uniform mat4 u_projMat;
uniform mat4 u_modelMat;
uniform vec3 u_lightPosition;
uniform vec3 eye_position;
uniform mat4 u_depth_matrix;
// attributes
out vec3 o_normal;
out vec3 o_toLight;
out vec3 o_toCamera;
out vec2 Texcoord;
out vec3 o_toLightPuntual;
out float dist;
//PARA CUBEMAPS
out vec3 Normal_model;
out vec3 Position_model;
out mat3 TBN;
out vec4 DepthCoord;
out vec3 viewDir;
void main(void)
{
	vec3 T = normalize(vec3(u_modelMat * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(u_modelMat * vec4(i_normal, 0.0)));
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

   Texcoord = m_texture;   
   vec4 worldPosition = ( u_modelMat * vec4(position,1.0));
   //Normal
	o_normal = normalize( vec3( u_modelMat * vec4(i_normal, 0.0)) );
	//direction to light	
	o_toLight = normalize( u_lightPosition  ); 
   //direction to camera
   vec3 VP;
   VP = vec3( u_lightPosition - worldPosition.xyz );
   dist = length(VP);
   o_toLightPuntual = normalize(VP);

   o_toCamera = normalize( eye_position );
   viewDir = normalize(o_toCamera - worldPosition.xyz);
   DepthCoord = u_depth_matrix * worldPosition;
   //CUBE MAP
   Normal_model = mat3(transpose(inverse(u_modelMat))) * i_normal;
   Position_model = worldPosition.xyz;

   //screen space coordinates of the vertex
   gl_Position = u_projMat * u_viewMat * worldPosition;
} 


