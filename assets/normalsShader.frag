#version 120

// Input textures
uniform sampler2D normalsTexture;
uniform sampler2D lightTexture;
uniform vec2 textureSize;
uniform vec2 lightSize;

// Light properties
uniform vec3 lightPosition;
uniform vec3 lightColor;

void main()
{
	// Sample our normals map
	vec2 coord = gl_TexCoord[0].xy;
	vec4 normalsColor = texture2D(normalsTexture, coord);
	
	// Get normal value from sample
	vec3 normals = normalize(normalsColor.rgb * 2.0 - 1.0);
	
	// Compute the light vector
	vec2 lightVector = lightPosition.xy - gl_FragCoord.xy;
	vec3 lightDir = vec3(lightVector / textureSize, lightPosition.z);
	lightDir = normalize(lightDir);
	
	vec2 lightCoord = lightVector / lightSize + 0.5;
	float lightPower = texture2D(lightTexture, lightCoord).r;
	
	// Compute the color intensity based on normals
	float colourIntensity = max(dot(normals, lightDir), 0.0);
	vec4 diffuse = vec4(lightColor * vec3(colourIntensity), lightPower * normalsColor.a);
	
	gl_FragColor = clamp(diffuse, 0.0, 1.0);
}

