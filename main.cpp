#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
using namespace std;

// ���ڴ�С��
const GLuint screenWidth = 800, screenHeight = 800;

// ���ӵ�λ�á�
glm::vec3 boxPositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f),
	glm::vec3( 20.5f,  5.0f, -15.0f),
	glm::vec3(-10.5f, -2.2f, -2.5f),
	
	glm::vec3( 20.4f, -0.4f, -30.5f),
	glm::vec3(-10.7f,  3.0f, -30.5f),
	glm::vec3( 10.3f, -5.0f, -5.5f),

	glm::vec3( 10.5f,  20.0f, -2.5f),
	glm::vec3( 1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  10.0f, -1.5f),

	glm::vec3( 10.5f,  2.0f, -1.5f),
	glm::vec3( 10.5f,  5.2f, -10.5f),
	glm::vec3(-3.8f, -15.0f, -12.3f)
};

// ������ԡ�
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ���̻ص���
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode) {
	// ���û�����ESC��,��������window���ڵ�WindowShouldClose����Ϊtrue���ر�Ӧ�ó���
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		 glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;	
    }
}

// ��껬���ص���
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// �����ֻص���
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// ���̿�������ƶ���
void moveCamera () {
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if(keys[GLFW_KEY_C])
        camera.ProcessKeyboard(DOWN, deltaTime);
	if(keys[GLFW_KEY_SPACE])
        camera.ProcessKeyboard(UP, deltaTime);
	
	boxPositions[0] = camera.Position + camera.Front + camera.Front + camera.Front;
}

GLuint loadCubeMap (vector<const GLchar*> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int imageWidth, imageHeight;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++) {
        image = SOIL_load_image(faces[i], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

	// ����������ˡ�
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Ʒ�ʽ��
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

GLuint loadTexture (GLchar* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int imageWidth, imageHeight;
    unsigned char* image = SOIL_load_image(path, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);	

    // ���������Ʒ�ʽ����
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// ����������ˡ�
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// �ͷ���Դ��
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

int main () {
	// ��ʼ�� GLFW
	glfwInit();
	// ��OpenGL�����汾��(Major)�ʹΰ汾��(Minor)����Ϊ3��ģʽΪ����ģʽ�����ڴ�С���ɵ���
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// �������ڡ�
	GLFWwindow* window = glfwCreateWindow(800, 800, "2015130257_T-ming_final", nullptr, nullptr);
	if (window == nullptr) {
		cout << "���ڴ���ʧ��" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// ���ûص���
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	// ���غ�ץȡ��꣬�ṩ��������޵Ĺ���ƶ���
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��GLEW֪��ʹ���ִ���������������ָ�����չ��
	glewExperimental = GL_TRUE;
	// ��ʼ��GLEW������OpenGL����ָ�롣
	glewInit();
	
	// ����Ȳ��ԡ�
	glEnable(GL_DEPTH_TEST);

	// ���ô��ڵ�ά�ȡ�
	glViewport(0, 0, screenWidth, screenHeight);

	// �������ꡣ
	float boxVertex[] = {
		// λ��                  // ����      // ������
		// ����
		-0.25f, -0.25f, -0.25f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.25f, -0.25f, -0.25f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		// ǰ��
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.25f,  0.25f,  0.25f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		// ����
		-0.25f,  0.25f,  0.25f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.25f,  0.25f, -0.25f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.25f,  0.25f,  0.25f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		// ����
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		// ����
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f, -0.25f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 0.25f, -0.25f,  0.25f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.25f,  0.25f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.25f, -0.25f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		// ����
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f, -0.25f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 0.25f,  0.25f,  0.25f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.25f,  0.25f,  0.25f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.25f,  0.25f, -0.25f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
	};
	GLfloat skyBoxVertex[] = {
        // λ��         
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

	// ��պ�
	// ��������ʼ������������󡢶��㻺�����
	GLuint skyBoxVAO, skyBoxVBO;
    glGenVertexArrays(1, &skyBoxVAO);
    glGenBuffers(1, &skyBoxVBO);
	// ��skyBoxVAO��
    glBindVertexArray(skyBoxVAO);
	// ���ƶ������鵽���㻺���й�OpenGLʹ�á�
    glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertex), &skyBoxVertex, GL_STATIC_DRAW);
	// ���ö���λ������ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ���skyBoxVAO��
    glBindVertexArray(0);

	// ����
	// ��������ʼ������������󡢶��㻺�����
	GLuint boxVAO, boxVBO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	// ��boxVAO��
	glBindVertexArray(boxVAO);
	// ���ƶ������鵽���㻺���й�OpenGLʹ�á�
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertex), boxVertex, GL_STATIC_DRAW);
	// ���ö���λ������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
    // ��������λ������ָ��
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
	// // ���÷���������ָ��
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
	// ���boxVAO��
	glBindVertexArray(0);

	// ������պ�����
	vector<const GLchar*> faces;
    faces.push_back("./textures/skyBox/right.jpg");
    faces.push_back("./textures/skyBox/left.jpg");
    faces.push_back("./textures/skyBox/top.jpg");
    faces.push_back("./textures/skyBox/bottom.jpg");
    faces.push_back("./textures/skyBox/back.jpg");
    faces.push_back("./textures/skyBox/front.jpg");
    GLuint skyBoxTexture = loadCubeMap(faces);

    // ������������
	GLuint mainBoxTexture = loadTexture("./textures/surprisedFace.png");
    GLuint boxTexture = loadTexture("./textures/box.png");

    // ������������ɫ����
	Shader boxShader("./shaders/box.vs", "./shaders/box.fs");
	Shader skyBoxShader("./shaders/skyBox.vs", "./shaders/skyBox.fs");

	// ��Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		// ����¼���
		glfwPollEvents();
		moveCamera();

		// ��Ⱦָ�
		// �����ڱ���ɫ��Ϊǳ��ɫ��
		glClearColor(0.12f, 0.63f, 1.0f, 1.0f);
		// �����ɫ��������Ȼ�������
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ��պ�
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		skyBoxShader.Use();

		// �����۲����ͶӰ����
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		// ���۲����ͶӰ��������ɫ����	
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyBoxVAO);
		glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(boxShader.Program, "skyBox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

		// ����
		boxShader.Use();

		// �����۲����ͶӰ����
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		// ���۲����ͶӰ��������ɫ����		
		glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// �����λ�ô�����ɫ����
		glUniform3f(glGetUniformLocation(boxShader.Program, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);

		glBindVertexArray(boxVAO);

		for (GLuint i = 0; i < 12; ++i) {
			if (i == 0) {
				glBindTexture(GL_TEXTURE_2D, mainBoxTexture);
			} else {
				glBindTexture(GL_TEXTURE_2D, boxTexture);
			}
			// ����ģ�;���
			glm::mat4 model;
			model = glm::translate(model, boxPositions[i]); 
			model = glm::rotate(model, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.5f, 1.0f, 0.0f));
			// ��ģ�;�������ɫ����
			glUniformMatrix4fv(glGetUniformLocation(boxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// �������塣
		glfwSwapBuffers(window);
	}

	// �ͷ���Դ��
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &boxVBO);
	// �ͷ�GLFW������ڴ档
	glfwTerminate();
	return 0;
}