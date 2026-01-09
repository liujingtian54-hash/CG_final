#pragma once
#include <string>

namespace ShaderSource {
	const std::string& Blinn_PhongVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	fPosition = vec3(model * vec4(aPosition, 1.0f));
	fNormal = mat3(transpose(inverse(model))) * aNormal;
	fTexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
}
)";
	const std::string& Blinn_PhongFragmentShader = R"(
	#version 330 core
	in vec3 fPosition;
	in vec3 fNormal;
	in vec2 fTexCoord;
	out vec4 color;

	struct Light {
		vec3 direction;
		vec3 Diffusecolor;
		vec3 Ambientcolor;
		vec3 Specularcolor;
		float intensity;
	};

	struct Material {
		vec3 kds[2];
		float blend;
		vec3 ks;
		float N;
	};

	uniform Material material;
	uniform Light light;
	uniform sampler2D mapKds[2];
	uniform vec3 viewPos;

	vec3 calcAmbientLight(vec3 kds) {
		return light.Ambientcolor * kds;
	}
	vec3 calcLambertLight(vec3 normal, vec3 kds) {
		vec3 lightDir = normalize(-light.direction);
		vec3 diffuse = light.Diffusecolor * max(dot(lightDir, normal), 0.0f);
		return light.intensity * kds * diffuse;
	}

	vec3 calcSpecularLight(vec3 normal, vec3 viewDir, vec3 kds, vec3 ks) {
		vec3 lightDir = normalize(-light.direction);
		vec3 halfwayDir = normalize(lightDir + normalize(-viewDir));
		float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.N);
		vec3 specular = light.Specularcolor * spec;
		return light.intensity * ks * specular;
	}

	void main() {
		vec3 color1 = texture(mapKds[0], fTexCoord).rgb * material.kds[0];
		vec3 color2 = texture(mapKds[1], fTexCoord).rgb * material.kds[1];
		vec3 material_kd = mix(color1,color2,material.blend);
		vec3 normal = normalize(fNormal);
		vec3 diffuse = calcLambertLight(normal, material_kd);
		vec3 diffuse2 = calcAmbientLight(material_kd);
		vec3 diffuse3 = calcSpecularLight(normal, normalize(viewPos - fPosition), material_kd, material.ks);
		vec3 temp = diffuse + diffuse2 + diffuse3;
		color = vec4(temp, 1.0f);
	})";
	const std::string& depthVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
	gl_Position = lightSpaceMatrix * model * vec4(aPosition, 1.0);
}
)";

	const std::string& depthFragmentShader = R"(
#version 330 core

void main(){}
)";
	const std::string& ShadowPoweredBlinnPhongVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
	fPosition = vec3(model * vec4(aPosition, 1.0f));
	fNormal = mat3(transpose(inverse(model))) * aNormal;
	fTexCoord = aTexCoord;
	fPosLightSpace = lightSpaceMatrix * model * vec4(aPosition, 1.0f);
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
})";
	const std::string& ShadowPoweredBlinnPhongFragmentShader = R"(
#version 330 core
	in vec3 fPosition;
	in vec3 fNormal;
	in vec2 fTexCoord;
	in vec4 fPosLightSpace;
	out vec4 color;
	struct Light {
		vec3 direction;
		vec3 Diffusecolor;
		vec3 Ambientcolor;
		vec3 Specularcolor;
		float intensity;
	};
	struct Material {
		vec3 kds[2];
		float blend;
		vec3 ks;
		float N;
	};
	uniform Material material;
	uniform Light light;
	uniform sampler2D mapKds[2];
	uniform sampler2D shadowMap;
	uniform vec3 viewPos;
	vec3 calcAmbientLight(vec3 kds) {
		return light.Ambientcolor * kds;
	}
	vec3 calcLambertLight(vec3 normal, vec3 kds) {
		vec3 lightDir = normalize(-light.direction);
		vec3 diffuse = light.Diffusecolor * max(dot(lightDir, normal), 0.0f);
		return light.intensity * kds * diffuse;
	}
	vec3 calcSpecularLight(vec3 normal, vec3 viewDir, vec3 kds, vec3 ks) {
		vec3 lightDir = normalize(-light.direction);
		vec3 halfwayDir = normalize(lightDir + normalize(-viewDir));
		float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.N);
		vec3 specular = light.Specularcolor * spec;
		return light.intensity * ks * specular;
	}
	float calculateShadow(vec4 posLightSpace, vec3 normal) {
		vec3 lightDir = normalize(-light.direction);
		vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(shadowMap, projCoords.xy).r;
		float currentDepth = projCoords.z;
		float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
		float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
		if(projCoords.z > 1.0)
			shadow = 0.0;
		return shadow;
	}
	void main() {
		vec3 color1 = texture(mapKds[0], fTexCoord).rgb * material.kds[0];
		vec3 color2 = texture(mapKds[1], fTexCoord).rgb * material.kds[1];
		vec3 material_kd = mix(color1,color2,material.blend);
		vec3 normal = normalize(fNormal);
		vec3 diffuse = calcAmbientLight(material_kd);
		vec3 diffuse2 = calcLambertLight(normal, material_kd);
		vec3 diffuse3 = calcSpecularLight(normal, normalize(viewPos - fPosition), material_kd, material.ks);
		float shadow = calculateShadow(fPosLightSpace, normal);
		color = vec4((diffuse + (1.0 - shadow) * (diffuse2 + diffuse3)), 1.0f);
	}
)";
	const std::string DebugQuadVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoord;
out vec2 fTexCoord;
void main() {
	fTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0f);
}
)";
	const std::string DebugQuadFragmentShader = R"(
#version 330 core
in vec2 fTexCoord;
out vec4 color;
uniform sampler2D debugTexture;
void main() {
	float depthValue = texture(debugTexture, fTexCoord).r;
	color = vec4(vec3(depthValue), 1.0f);
}
)";
	const std::string bloomVertexShader = R"(
#version 330 core
in vec3 aPosition;
in vec2 aTexCoord;
out vec2 fTexCoord;
void main() {
	fTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0f);
}
)";
	const std::string bloomFragmentShader = R"(
#version 330 core
in vec2 fTexCoord;
out vec4 color;
uniform sampler2D scene;
void main() {
	vec3 hdrColor = texture(scene, fTexCoord).rgb;
	float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		color = vec4(hdrColor, 1.0);
	else
		color = vec4(0.0, 0.0, 0.0, 1.0);
}
)";
}