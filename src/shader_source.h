#pragma once
#include <string>

namespace ShaderSource {
	const std::string lambertVertexShader =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPosition;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"

		"out vec3 fPosition;\n"
		"out vec3 fNormal;\n"
		"out vec2 fTexCoord;\n"

		"uniform mat4 projection;\n"
		"uniform mat4 view;\n"
		"uniform mat4 model;\n"

		"void main() {\n"
		"    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
		"    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
		"    fTexCoord = aTexCoord;\n"
		"    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
		"}\n";
	const std::string lambertFragmentShader =
		"#version 330 core\n"
		"in vec3 fPosition;\n"
		"in vec3 fNormal;\n"
		"in vec2 fTexCoord;\n"
		"out vec4 color;\n"

		"struct DirectionalLight {\n"
		"    vec3 direction;\n"
		"    vec3 color;\n"
		"    float intensity;\n"
		"};\n"

		"struct Material {\n"
		"    vec3 kds[2];\n"
		"    float blend;\n"
		"	 float ka;\n"
		"    float kd;\n"
		"    float ks;\n"
		"};\n"

		"uniform Material material;\n"
		"uniform DirectionalLight light;\n"
		"uniform sampler2D mapKds[2];\n"

		"vec3 calcDirectionalLight(vec3 normal, vec3 kds) {\n"
		"    vec3 lightDir = normalize(-light.direction);\n"
		"    vec3 diffuse = max(dot(lightDir, normal), 0.0f);\n"
		"    return light.color * light.intensity * kds * diffuse;\n"
		"}\n"

		"void main() {\n"
		"    vec3 color1 = texture(mapKds[0], fTexCoord).rgb * material.kds[0];\n"
		"    vec3 color2 = texture(mapKds[1], fTexCoord).rgb * material.kds[1];\n"
		"    vec3 localkds = mix(color1,color2,material.blend);\n"
		"    vec3 normal = normalize(fNormal);\n"
		"    vec3 diffuse = calcDirectionalLight(normal, localkds);\n"
		"    color = vec4(diffuse , 1.0f);\n"
		"}\n";
	const std::string& Blinn_PhongVertexShader = lambertVertexShader;
	const std::string& Blinn_PhongFragmentShader = R"(
	#version 330 core
	in vec3 fPosition;
	in vec3 fNormal;
	in vec2 fTexCoord;
	out vec4 color;

	struct DirectionalLight {
		vec3 direction;
		vec3 color;
		float intensity;
	};

	struct Material {
		vec3 kds[2];
		float blend;
		vec3 ka;
		vec3 kd;
		vec3 ks;
	};

	uniform Material material;
	uniform DirectionalLight light;
	uniform sampler2D mapKds[2];
	uniform vec3 viewPos;

	vec3 calcAmbientLight(vec3 kds) {
		return material.ka * kds;
	}
	vec3 calcLambertLight(vec3 normal, vec3 kds) {
		vec3 lightDir = normalize(-light.direction);
		vec3 diffuse = light.color * max(dot(lightDir, normal), 0.0f);
		return light.intensity * kds * diffuse;
	}

	vec3 calcSpecularLight(vec3 normal, vec3 viewDir, vec3 kds, vec3 ks) {
		vec3 lightDir = normalize(-light.direction);
		vec3 halfwayDir = normalize(lightDir + normalize(-viewDir));
		float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), 32.0f);
		vec3 specular = light.color * spec;
		return light.intensity * ks * specular;
	}

	void main() {
		vec3 color1 = texture(mapKds[0], fTexCoord).rgb * material.kds[0];
		vec3 color2 = texture(mapKds[1], fTexCoord).rgb * material.kds[1];
		vec3 localkds = mix(color1,color2,material.blend);
		vec3 normal = normalize(fNormal);
		vec3 diffuse = calcLambertLight(normal, localkds);
		vec3 diffuse2 = calcSpecularLight(normal, normalize(viewPos - fPosition), localkds, material.ks);
		vec3 diffuse3 = calcAmbientLight(localkds);
		color = vec4(diffuse3, 1.0f) + vec4(diffuse, 1.0f) + vec4(diffuse2, 1.0f);
	})";
}