// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
// ����SOIL��
#include <SOIL/SOIL.h>
// ����GLM��
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"
// �������������
#include "texture.h"
// ����ģ�͵���
#include "model.h"
// ��հ�Χ����
#include "skybox.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// �������ƶ�
void do_movement();

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
glm::vec3 lampPos(-1.0f, -1.0f, -1.0f);
float speed_land_z = 0.15, speed_land_y = 0.02;
int main(int argc, char** argv)
{

	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading model with AssImp", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);
	// ע������¼��ص�����
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// ע���������¼��ص�����
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// ��겶�� ͣ���ڳ�����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Section1 ����ģ������
	Model objModel,objModel_1;
	if (!objModel_1.loadModel("../../resources/models/SmallTropicalIsland/SmallTropicalIsland.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	if (!objModel.loadModel("../../resources/models/millenium-falcon/millenium-falcon.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	/*std::ifstream modelPath("modelPath.txt");
	if (!modelPath)
	{
		std::cerr << "Error::could not read model path file." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	std::string modelFilePath;
	std::getline(modelPath, modelFilePath);
	if (modelFilePath.empty())
	{
		std::cerr << "Error::model path empty." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	if (!objModel.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}*/
	std::cout << "sad \n";
	// Section2 ����Skybox
	std::vector<const char*> faces;
	faces.push_back("../../resources/skyboxes/sky/sky_rt.jpg");
	faces.push_back("../../resources/skyboxes/sky/sky_lf.jpg");
	faces.push_back("../../resources/skyboxes/sky/sky_up.jpg");
	faces.push_back("../../resources/skyboxes/sky/sky_dn.jpg");
	faces.push_back("../../resources/skyboxes/sky/sky_bk.jpg");
	faces.push_back("../../resources/skyboxes/sky/sky_ft.jpg");
	SkyBox skybox;
	skybox.init(faces);

	// Section3 ׼����ɫ������
	Shader shader("scene.vertex", "scene.frag");
	Shader skyBoxShader("skybox.vertex", "skybox.frag");
	Shader falconShader("falcon.vertex", "falcon.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		// ����colorBuffer��ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		// ���colorBuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// �Ȼ��Ƴ���
		shader.use();
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f); // ͶӰ����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
		static float trans_x = 0.35, trans_y = -15, trans_z = -100.0;

		model = glm::translate(model, glm::vec3(trans_x, trans_y, trans_z)); // �ʵ��µ�λ��
		//if (trans_y < -0.64)
		//{
		//	//trans_y += speed_land_y;
		//	camera.handleKeyPress(BOTTOM, deltaTime);
		//}
		//if (trans_z < -1.6)
		//{
		//	camera.handleKeyPress(FORWARD, deltaTime);
		//	//trans_z += speed_land_z;
		//}
		//std::cout << "z = " << trans_z << std::endl;
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f)); // �ʵ���Сģ��
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		// ע���Ѿ�����diffuse specular reflection map 
		// ����Ӧ������Ϊ���ĸ�����Ԫ
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextId());
		glUniform1i(glGetUniformLocation(shader.programId, "envText"), 3);
		glUniform3f(glGetUniformLocation(shader.programId, "cameraPos"),
			camera.position.x, camera.position.y, camera.position.z); // ע�����ù۲���λ��
		objModel.draw(shader);	// ��������


		//�ɴ�
		falconShader.use();
		projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 1000.0f); // ͶӰ����
		view = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model_falcon;
		static float fa_x = 0.0, fa_y = 0.0, fa_z = 0.0;

		model_falcon = glm::translate(model_falcon, glm::vec3(fa_x, fa_y, fa_z)); // �ʵ��µ�λ��
		//model_falcon = glm::scale(model_falcon, glm::vec3(0.03f, 0.03f, 0.03f)); // �ʵ���Сģ��
		glUniformMatrix4fv(glGetUniformLocation(falconShader.programId, "model_falcon"),
			1, GL_FALSE, glm::value_ptr(model_falcon));
		objModel_1.draw(falconShader);

		// Ȼ����ư�Χ��
		skyBoxShader.use();
		view = glm::mat4(glm::mat3(camera.getViewMatrix())); // �ӱ任���� �Ƴ�translate����
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextId());
		glUniform1i(glGetUniformLocation(skyBoxShader.programId, "skybox"), 0);
		skybox.draw(skyBoxShader);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // �״�����ƶ�
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}
// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// ����������ദ����̿���
void do_movement()
{

	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
}