#pragma once
//https://learnopengl.com/Advanced-OpenGL/Cubemaps
unsigned int LoadCubemap(std::vector<std::string> faces);

//generated with http://wwwtyro.github.io/space-3d/#animationSpeed=1&fov=80&nebulae=true&pointStars=true&resolution=1024&seed=3ri5hb5d1eo0&stars=true&sun=true
std::vector<std::string> textures_faces
{
	"skybox/right.png",
	"skybox/left.png",
	"skybox/top.png",
	"skybox/bottom.png",
	"skybox/front.png",
	"skybox/back.png"
};
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

struct SkyBoxShader {

	GLuint shaderProgramID;


	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
	}
	void setVec3(const std::string& name, glm::vec3 vec) const
	{
		glUniform3f(glGetUniformLocation(shaderProgramID, name.c_str()), (GLfloat)vec.x, (GLfloat)vec.y, (GLfloat)vec.z);

	}
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void GetVec3(const std::string& name) {
		float return_vec[3];
		glGetUniformfv(shaderProgramID, glGetUniformLocation(shaderProgramID, name.c_str()), &return_vec[0]);
		std::cout << return_vec[0] << " : " << return_vec[1] << " : " << return_vec[2] << std::endl;
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
	}

	void Use() {
		glUseProgram(shaderProgramID);
	}


	SkyBoxShader(std::string vertexShaderPath, std::string fragmentShaderPath) {
		shaderProgramID = LoadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		glUseProgram(shaderProgramID);
	}

	SkyBoxShader() {

	}

};

unsigned int skyboxVAO, skyboxVBO;
GLuint cubemapTextureID;
SkyBoxShader skyBoxShader;

void InitSkyBox() {

	cubemapTextureID = LoadCubemap(textures_faces);

	skyBoxShader = SkyBoxShader("skybox.vert", "skybox.frag");
	skyBoxShader.setInt("skybox", 0);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

unsigned int LoadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed tod load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	std::cout << "Cubemap loaded." << std::endl;
	return textureID;
}

void DrawSkyBox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	
	// change depth function so depth test passes when values are equal to depth buffer's content
	skyBoxShader.Use();

	skyBoxShader.setMat4("view", glm::mat4(glm::mat3(viewMatrix)));
	skyBoxShader.setMat4("projection", projectionMatrix);

	glBindVertexArray(skyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}