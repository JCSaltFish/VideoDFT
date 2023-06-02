#include <iostream>
#include <string>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "shaders.h"

#include "ScreenCapture.h"

GLFWwindow* window;
GLint wWindow = 1440;
GLint hWindow = 900;
const std::string wTitle = "DFT";

GLuint quadProg = 0;
GLuint frameTex = 0;
GLuint quadVao = 0;

std::atomic<bool> imgbufferchanged = false;
std::vector<SL::Screen_Capture::Monitor> monitors;
SL::Screen_Capture::Monitor monitor;
std::unique_ptr<unsigned char[]> imgbuffer;
std::atomic<int> imgbuffersize;
std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> framgrabber;

GLuint fbo0 = 0;
GLuint fboTex0 = 0;
GLuint fbo1 = 0;
GLuint fboTex1 = 0;
GLuint fbo2 = 0;
GLuint fboTex2 = 0;
GLuint fbo3 = 0;
GLuint fboTex3 = 0;

int size = 1024;
const int MODE_DFT = 1;
const int MODE_IFT = 2;
int mode = MODE_DFT;

int monitorId = 0;

void InitScreenCapture();

void DrawGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	/*ImGui::Begin("Source");
	ImGui::Image((void*)fboTex0, ImVec2(640, 360));
	ImGui::End();

	ImGui::Begin("Input");
	ImGui::Image((void*)fboTex1, ImVec2(640, 360));
	ImGui::End();

	ImGui::Begin("DFT");
	ImGui::Image((void*)fboTex2, ImVec2(640, 360));
	ImGui::End();

	ImGui::Begin("IFT");
	ImGui::Image((void*)fboTex3, ImVec2(640, 360));
	ImGui::End();*/

	ImGui::SetNextWindowSize(ImVec2(400, 60), ImGuiCond_Once);
	ImGui::Begin("Menu");

	std::string strMonitor = "Monitor " + std::to_string(monitorId);
	ImGui::SetNextItemWidth(120);
	if (ImGui::BeginCombo("Monitor", strMonitor.c_str()))
	{
		auto monitors = SL::Screen_Capture::GetMonitors();
		for (int i = 0; i < monitors.size(); i++)
		{
			bool isSelected = (i == monitorId);
			std::string str = "Monitor " + std::to_string(i);
			if (ImGui::Selectable(str.c_str(), isSelected))
			{
				monitorId = i;
				InitScreenCapture();
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(120);
	const char* modes[] = { "DFT", "IFT" };
	int modeId = mode - 1;
	if (ImGui::Combo("Mode", &modeId, modes, IM_ARRAYSIZE(modes)))
		mode = modeId + 1;

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Display()
{
	/*glViewport(0, 0, wWindow, hWindow);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTex);
	glUniform1i(0, 0);
	glUniform1i(1, 0);
	glUniform1i(2, size);
	glUniform1i(3, 0);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);*/

	glViewport(0, 0, size, size);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTex);
	glUniform1i(0, 0);
	glUniform1i(1, 0);
	glUniform1i(2, size);
	glUniform1i(3, mode);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glViewport(0, 0, size, size);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProg);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, fboTex1);
	glUniform1i(0, 1);
	glUniform1i(1, 1);
	glUniform1i(2, size);
	glUniform1i(3, mode);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	/*glViewport(0, 0, size, size);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo3);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProg);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, fboTex2);
	glUniform1i(0, 2);
	glUniform1i(1, 1);
	glUniform1i(2, size);
	glUniform1i(3, MODE_IFT);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);*/

	glViewport(0, 0, wWindow, hWindow);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProg);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, fboTex2);
	glUniform1i(0, 3);
	glUniform1i(1, 1);
	glUniform1i(2, size);
	glUniform1i(3, 0);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	DrawGui();

	glfwSwapBuffers(window);
}

void Idle()
{
	if (imgbufferchanged)
	{
		glBindTexture(GL_TEXTURE_2D, frameTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, monitor.Width, monitor.Height,
			GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer.get());
		imgbufferchanged = false;
	}

	Display();
}

void Reshape(GLFWwindow* window, int w, int h)
{
	wWindow = w;
	hWindow = h;
	Display();
}

void Close(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void InitializeProgram(GLuint* program, const std::string& vert, const std::string& frag)
{
	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, vert));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, frag));
	*program = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

int InitializeGL(GLFWwindow*& window)
{
	if (!glfwInit())
		return -1;
	
	window = glfwCreateWindow(wWindow, hWindow, wTitle.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, Reshape);
	glfwSetWindowCloseCallback(window, Close);

	glfwMakeContextCurrent(window);

	glewInit();

	InitializeProgram(&quadProg, vQuad, fQuad);
	//InitializeProgram(&quadProg, vQuad, LoadShader("../FFT/shaders/fquad.glsl"));
	glGenVertexArrays(1, &quadVao);
	glGenTextures(1, &frameTex);

	glGenTextures(1, &fboTex0);
	glBindTexture(GL_TEXTURE_2D, fboTex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, wWindow, hWindow, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &fbo0);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glGenTextures(1, &fboTex1);
	glBindTexture(GL_TEXTURE_2D, fboTex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &fbo1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex1, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &fboTex2);
	glBindTexture(GL_TEXTURE_2D, fboTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &fbo2);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &fboTex3);
	glBindTexture(GL_TEXTURE_2D, fboTex3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &fbo3);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex3, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}

void InitializeImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	ImGui::GetIO().IniFilename = 0;
}

void GlfwLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		Idle();
		glfwPollEvents();
	}
}

void ExtractAndConvertToRGBA(const SL::Screen_Capture::Image& img, unsigned char* dst, size_t dst_size)
{
	assert(dst_size >= static_cast<size_t>
		(
			SL::Screen_Capture::Width(img) *
			SL::Screen_Capture::Height(img) *
			sizeof(SL::Screen_Capture::ImageBGRA)
		));
	auto imgsrc = StartSrc(img);
	auto imgdist = dst;
	if (img.isContiguous)
		memcpy(imgdist, imgsrc, dst_size);
	else
	{
		for (auto h = 0; h < Height(img); h++)
		{
			auto startimgsrc = imgsrc;
			for (auto w = 0; w < Width(img); w++)
			{
				*imgdist++ = imgsrc->R;
				*imgdist++ = imgsrc->G;
				*imgdist++ = imgsrc->B;
				*imgdist++ = 0; // alpha should be zero
				imgsrc++;
			}
			imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
		}
	}
}

void InitScreenCapture()
{
	if (framgrabber)
		framgrabber.reset();
	std::vector<SL::Screen_Capture::Monitor>().swap(monitors);
	if (imgbuffer)
		imgbuffer.reset();

	glBindTexture(GL_TEXTURE_2D, frameTex);

	// Set texture wrap parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering paremeters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture
	monitors = SL::Screen_Capture::GetMonitors();
	monitor = monitors[monitorId];
	monitors[0] = monitor;
	monitors.resize(1);
	imgbuffersize = monitor.Width * monitor.Height * sizeof(SL::Screen_Capture::ImageBGRA);
	imgbuffer = std::make_unique<unsigned char[]>(imgbuffersize);
	memset(imgbuffer.get(), 0, imgbuffersize); // create a black image to start with

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, monitor.Width, monitor.Height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer.get());
	glBindTexture(GL_TEXTURE_2D, 0);

	framgrabber = SL::Screen_Capture::CreateCaptureConfiguration([&]()
	{
		return monitors;
	})->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor)
	{
		imgbufferchanged = true;
		ExtractAndConvertToRGBA(img, imgbuffer.get(), imgbuffersize);
	})->start_capturing();
	framgrabber->setFrameChangeInterval(std::chrono::milliseconds(10));
}

void OnExit()
{
	if (quadProg)
		glDeleteProgram(quadProg);
	if (frameTex)
		glDeleteTextures(1, &frameTex);
	if (quadVao)
		glDeleteVertexArrays(1, &quadVao);
	if (fbo0)
		glDeleteFramebuffers(1, &fbo0);
	if (fboTex0)
		glDeleteTextures(1, &fboTex0);
	if (fbo1)
		glDeleteFramebuffers(1, &fbo1);
	if (fboTex1)
		glDeleteTextures(1, &fboTex1);
	if (fbo2)
		glDeleteFramebuffers(1, &fbo2);
	if (fboTex2)
		glDeleteTextures(1, &fboTex2);
	if (fbo3)
		glDeleteFramebuffers(1, &fbo3);
	if (fboTex3)
		glDeleteTextures(1, &fboTex3);
}

int main(int argc, char** argv)
{
	int initRes = InitializeGL(window);
	if (initRes)
		return initRes;

	InitializeImGui();
	
	InitScreenCapture();

	GlfwLoop();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	OnExit();
	return 0;
}
