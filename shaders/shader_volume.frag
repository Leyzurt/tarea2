#version 330

in vec3 EntryPoint;
in vec4 ExitPointCoord;

uniform sampler1D texture1D;
uniform sampler2D texture2D;
uniform sampler3D texture3D;
uniform float StepSize;
uniform vec2 ScreenSize;
uniform vec3 eye_position;
out vec4 resultingColor;
//uniform sampler2D ExitPoints;
//uniform sampler3D VolumeTex;
//uniform sampler1D TransferFunc;


void main() {
	//resultingColor = vec4(1.0, 0.0, 1.0, 1.0);

	vec3 exitPoint = texture(texture2D, gl_FragCoord.st / ScreenSize).xyz;
	if (EntryPoint == exitPoint)
		//background need no raycasting
		discard;
	vec3 dir = exitPoint - EntryPoint;
	//vec3 dir = eye_position - EntryPoint;
	float len = length(dir); // the length from front to back is calculated and used to terminate the ray
	vec3 deltaDir = normalize(dir) * StepSize;
	float deltaDirLen = length(deltaDir);
	vec3 voxelCoord = EntryPoint;
	vec4 colorAcum = vec4(0.0); // The dest color
	float alphaAcum = 0.0;                // The  dest alpha for blending
										 
	float intensity;
	float lengthAcum = 0.0;
	vec4 colorSample; // The src color 
	float alphaSample; // The src alpha
					   // backgroundColor
	vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.2);

	for (int i = 0; i < 1600; i++)
	{
		intensity = texture(texture3D, voxelCoord).x;		
		colorSample = texture(texture1D, intensity);
		//colorSample = vec4(intensity, intensity, intensity, 1.0f); //PRUEBA
		// modulate the value of colorSample.a
		// front-to-back integration
		if (colorSample.a > 0.0) {
			// accomodate for variable sampling rates (base interval defined by mod_compositing.frag)
			colorSample.a = 1.0 - pow(1.0 - colorSample.a, StepSize*200.0f);
			colorAcum.rgb += (1.0 - colorAcum.a) * colorSample.rgb * colorSample.a;
			colorAcum.a += (1.0 - colorAcum.a) * colorSample.a;
		}
		voxelCoord += deltaDir;
		lengthAcum += deltaDirLen;
		if (lengthAcum >= len)
		{
			colorAcum.rgb = colorAcum.rgb*colorAcum.a + (1 - colorAcum.a)*bgColor.rgb;
			break;  // terminate if opacity > 1 or the ray is outside the volume	
		}
		else if (colorAcum.a > 1.0)
		{
			colorAcum.a = 1.0;
			break;
		}
	}
	resultingColor = colorAcum;
}
