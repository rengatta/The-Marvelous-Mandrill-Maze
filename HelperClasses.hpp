#pragma once
class Camera;
class BoundingBox;
#define WAV_PATH "Roland-GR-1-Trumpet-C5.wav"
#define MUS_PATH "HR2_Friska.ogg"

int volume = MIX_MAX_VOLUME /40;
bool muted = false;

void PlayMP3(std::string songPath) {
	
	int result = 0;
	int flags = MIX_INIT_MP3;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Failed to init SDL\n");
		exit(1);
	}

	if (flags != (result = Mix_Init(flags))) {
		printf("Could not initialize mixer (result: %d).\n", result);
		printf("Mix_Init: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);

	Mix_Music* music = Mix_LoadMUS(songPath.c_str());
	Mix_PlayMusic(music, -1);


	Mix_VolumeMusic(volume);

}

void ToggleMute() {
	muted = !muted;

	if (muted) {
		Mix_VolumeMusic(0);
	}
	else {
		Mix_VolumeMusic(volume);
	}

}

struct VBOs {
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	GLuint elementBuffer;

	VBOs() {

	}

	void GenerateBuffers() {
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &uvBuffer);
		glGenBuffers(1, &normalBuffer);
		glGenBuffers(1, &elementBuffer);
	}
};


class BoundingBox {

public:
	glm::vec3 size;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 previousMin;


	BoundingBox() {
		

	}
	

	BoundingBox(glm::vec3 min, glm::vec3 size) {
		this->size = size;
		this->min = min;
		this->previousMin = min;
	

		max.x = min.x + size.x;
		max.y = min.y + size.y;
		max.z = min.z + size.z;

	}

	glm::vec3 GetPreviousMax() {
		return (previousMin + size);

	}

	void ChangePosition(glm::vec3 position) {
		this->previousMin = min;
		this->min = position;
	


		max.x = min.x + size.x;
		max.y = min.y + size.y;
		max.z = min.z + size.z;

	}

	void ChangePosition2(glm::vec3 position) {

		this->min = position;
		max.x = min.x + size.x;
		max.y = min.y + size.y;
		max.z = min.z + size.z;

	}

};

void PrintVector(glm::vec3& a) {
	std::cout << "<" << a.x << ", " << a.y << ", " << a.z << ">" << std::endl;

}

std::string GetVectorString(glm::vec3& a) {
	return "<" + std::to_string(a.x) +  ", " + std::to_string(a.y) + ", " + std::to_string(a.z) + ">";

}

//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
bool Intersect(BoundingBox& a, BoundingBox& b) {
	return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool PushBackIfIntersecting(BoundingBox& a, BoundingBox& b, glm::vec3 &newPosition, bool &onFloor) {
	if (Intersect(a, b)) {
	
		glm::vec3 aPreviousMax = a.GetPreviousMax();
		float pushBack = 0.0001f;
		

		if (a.previousMin.y > b.max.y) {

			//bottom of a is higher than top of b
			newPosition =  glm::vec3(0.0f, (b.max.y + pushBack)-a.min.y, 0.0f);
			onFloor = true;
			return true;

		}
		else if (a.previousMin.x > b.max.x) {
			//left of a is to the right of the right of b

			newPosition = glm::vec3((b.max.x + pushBack)-a.min.x, 0.0f, 0.0f);
			return true;
		}
		else if (a.previousMin.z > b.max.z) {
			//z

			newPosition = glm::vec3(0.0f, 0.0f, (b.max.z + pushBack)-a.min.z);
			return true;
		} else if (aPreviousMax.x < b.min.x) {
			//right of a is to the left of the left of b
			newPosition = glm::vec3(((b.min.x - pushBack) - a.size.x)-a.min.x, 0.0f, 0.0f);
			return true;
		}
		else if (aPreviousMax.z < b.min.z) {
			//other z
			newPosition = glm::vec3(0.0f, 0.0f, ((b.min.z - pushBack) - a.size.z)-a.min.z);
			return true;
		}

		else if (aPreviousMax.y < b.min.y) {
			//top of a is lower than bottom of b
			newPosition = glm::vec3(0.0f, ((b.min.y - pushBack) - a.size.y)-a.min.y, 0.0f);
			return true;
		}

	} 

	return false;

}
//https://learnopengl.com/Getting-started/Camera
class Camera {

	public:
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
		float yaw = -90.0f;
		float pitch = 0.0f;

	Camera() {
		position = glm::vec3(4, 3, 3);
		forward = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - position);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void CalculateCameraVectors() {
		right = cross(forward, up);
		right = glm::normalize(right);
	}


	void mouse(float xoffset, float yoffset) {

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch -= yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(direction);

		CalculateCameraVectors();
	}

};

class Player {
public:

	glm::vec3 position;
	glm::vec3 cameraLocalPosition;
	Camera camera;
	BoundingBox collider;

	Player() {
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
		this->cameraLocalPosition = glm::vec3(0.5f, 1.5f, 0.5f);
		this->camera.position = this->position + cameraLocalPosition;
		this->collider = BoundingBox(this->position, glm::vec3(1.0f, 2.0f, 1.0f));
	}

	void ChangePosition(glm::vec3 position) {
		this->position = position;
		this->camera.position = this->position + cameraLocalPosition;
		this->collider.ChangePosition(position);

	}

	void ChangePosition2(glm::vec3 position) {
		this->position = position;
		this->camera.position = this->position + cameraLocalPosition;
		this->collider.ChangePosition2(position);

	}

};

class Object {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 scale;

	float rotateAngle = 0.0f;

	GLuint textureID;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	BoundingBox collider;
	bool colliderEnabled = true;

	int pointLightIndex;

	void GenerateBoundingBox() {
		glm::vec3 min(INFINITY, INFINITY, INFINITY);
		glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);


		for (int i = 0; i < indexed_vertices.size(); i++) {
			if(indexed_vertices[i].x < min.x) 
				min.x = indexed_vertices[i].x;
			else if(indexed_vertices[i].x > max.x)
				max.x = indexed_vertices[i].x;

			if (indexed_vertices[i].y < min.y)
				min.y = indexed_vertices[i].y;
			else if (indexed_vertices[i].y > max.y)
				max.y = indexed_vertices[i].y;

			if (indexed_vertices[i].z < min.z)
				min.z = indexed_vertices[i].z;
			else if (indexed_vertices[i].z > max.z)
				max.z = indexed_vertices[i].z;
		}

		glm::vec3 sizeDifference = ((max - min) * (scale - glm::vec3(1.0f, 1.0, 1.0f))) / 2.0f;
		//glm::vec3 sizeDifference2 = (max - min) * (scale);

	
		min += position;
		max += position;

		min -= sizeDifference;
		max += sizeDifference;
		

		collider = BoundingBox(min, max - min);

	}

	Object(const Object& copy) {

		this->collider = copy.collider;

		this->vertices = copy.vertices;
		this->uvs = copy.uvs;
		this->normals = copy.normals;
		this->textureID = copy.textureID;
		this->indices = copy.indices;
		this->indexed_vertices = copy.indexed_vertices;
		this->indexed_uvs = copy.indexed_uvs;
		this->indexed_normals = copy.indexed_normals;

		this->position = copy.position;
		this->scale = copy.scale;
		this->model = copy.model;

	
		this->colliderEnabled = copy.colliderEnabled;
		
	}

	void RotateHorizontally(float rotateModifier) {

		model *= glm::rotate(rotateModifier, glm::vec3(0.0f, 1.0, 0.0));
		
	}

	Object(std::string filename, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) {

		bool res = loadOBJ(filename.c_str(), vertices, uvs, normals);
		this->textureID = LoadTexture("defaultTexture.png");
		this->color = color;

		this->position = position;
		this->scale = scale;
		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		model = glm::mat4(1.0f);
		model *= glm::translate(position);
		model *= glm::scale(glm::vec3(scale.x, scale.y, scale.z));

		GenerateBoundingBox();
	}

	Object(std::string filename, std::string textureFilename, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f)) {

		bool res = loadOBJ(filename.c_str(), vertices, uvs, normals);
		this->textureID = LoadTexture(textureFilename.c_str());
		this->position = position;
		this->scale = scale;
		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		model = glm::mat4(1.0f);
		model *= glm::translate(position);
		model *= glm::scale(glm::vec3(scale.x, scale.y, scale.z));
		
		GenerateBoundingBox();
	}

	Object(Object &templateObject, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) {
		
		vertices = templateObject.vertices;
		uvs = templateObject.uvs;
		normals = templateObject.normals;
		textureID = templateObject.textureID;
		color = templateObject.color;
		indices = templateObject.indices;
		indexed_vertices = templateObject.indexed_vertices;
		indexed_uvs = templateObject.indexed_uvs;
		indexed_normals = templateObject.indexed_normals;

		this->position = position;
		this->scale = scale;
		model = glm::mat4(1.0f);
		model *= glm::translate(position);
		model *= glm::scale(glm::vec3(scale.x, scale.y, scale.z));
		
		GenerateBoundingBox();

	}

	Object(std::string filename, GLuint texture, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) {

		bool res = loadOBJ(filename.c_str(), vertices, uvs, normals);
		this->textureID = texture;
		this->position = position;
		this->scale = scale;
		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		model = glm::mat4(1.0f);
		model *= glm::scale(glm::vec3(scale.x, scale.y, scale.z));
		model *= glm::translate(position);

		GenerateBoundingBox();
	}

	Object() {

	}

};