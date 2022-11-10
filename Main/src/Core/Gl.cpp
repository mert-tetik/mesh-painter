#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "RigidPainter.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"

void GlSet::drawArrays(std::vector<float> &vertices,bool isLine) {
	if (!isLine) {
		glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size() * sizeof(float), &vertices[0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0,vertices.size() * sizeof(float), &vertices[0]);
		glDrawArrays(GL_LINES, 0, vertices.size() / 8);
	}
}
void GlSet::uiDataToShaders(glm::vec3 color) {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 1);
	int uiColorLoc = glGetUniformLocation(cmnd.program, "uiColor");
	glUniform3f(uiColorLoc, color.x, color.y, color.z);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 1);
}
void GlSet::meshDataToShaders() {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void GlSet::axisPointerDataToShaders() {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 1);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void GlSet::bindFramebuffer(unsigned int FBO) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void GlSet::genFramebuffers(unsigned int &FBO) {
	glGenFramebuffers(1, &FBO);
}
void GlSet::deleteFramebuffers(unsigned int& FBO) {
	glDeleteFramebuffers(1, &FBO);
}
void GlSet::setVertexAtribPointer() {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
void GlSet::bufferData(std::vector<float> &vertices) {
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
}
void GlSet::activeTexture(unsigned int texture) {
	glActiveTexture(texture);
}
void GlSet::bindTexture(unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GlSet::genTextures(unsigned int &texture) {
	glGenTextures(1, &texture);
}
void GlSet::uniform1i(unsigned int program, const char* target, int intValue){
	glUniform1i(glGetUniformLocation(program, target), intValue);
}
void GlSet::uniform1f(unsigned int program, const char* target, float floatValue) {
	glUniform1f(glGetUniformLocation(program, target), floatValue);
}
void GlSet::uniform3fv(unsigned int program, const char* target, glm::vec3 &vectorValue) {
	glUniform3fv(glGetUniformLocation(program, target),1, &vectorValue[0]);
}
void GlSet::uniform3f(unsigned int program, const char* target, float f1, float f2, float f3) {
	glUniform3f(glGetUniformLocation(program, target), f1,f2,f3);
}
void GlSet::uniformMatrix4fv(unsigned int program, const char* target, glm::mat4x4 matrixVal) {
	glUniformMatrix4fv(glGetUniformLocation(program, target), 1,GL_FALSE, glm::value_ptr(matrixVal));
}
void GlSet::enable(unsigned int target) {
	glEnable(target);
}
void GlSet::disable(unsigned int target) {
	glDisable(target);
}
void GlSet::cullFace(unsigned int target) {
	glCullFace(target);
}
void GlSet::viewport(int width,int height) {
	glViewport(0, 0, width, height);
}
void GlSet::blendFunc(unsigned int sfactor, unsigned int dfactor) {
	glBlendFunc(sfactor, dfactor);
}
void GlSet::texImage(GLubyte* data,int width,int height,unsigned int channels) {
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
}
void GlSet::generateMipmap() {
	glGenerateMipmap(GL_TEXTURE_2D);
}
void GlSet::genRenderbuffers(unsigned int &RBO) {
	glGenRenderbuffers(1, &RBO);
}
void GlSet::bindRenderBuffer(unsigned int RBO) {
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
}
void GlSet::getProgram() {//Prepare shader program | Usen once
	CommonData cmnd;
	Utilities utilities;
	std::string fshader = utilities.readFile("fragmentShaderSource.glsl");
	fshader.pop_back();
	const char* fragmentShaderSource = fshader.c_str();
	std::string vshader = utilities.readFile("vertexShaderSource.glsl");
	vshader.pop_back();
	const char* vertexShaderSource = vshader.c_str();
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
#pragma region vertex_compilation_check
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

#pragma region fragment_compilation_check
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion

	//Program
	cmnd.program = glCreateProgram();
	glAttachShader(cmnd.program, vertexShader);
	glAttachShader(cmnd.program, fragmentShader);
	glLinkProgram(cmnd.program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#pragma region program_link_check
	glGetProgramiv(cmnd.program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(cmnd.program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
#pragma endregion
}
int a = 0;
void drawBrushSizeIndicator(float distanceX,float screenWidth,float screenHeight,float mouseXpos,float mouseYpos,glm::vec3 color) {
	CommonData commonData;
	GlSet glset;
	std::vector<float> paintingSquare{
		// first triangle
		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // top right
		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom right
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // top left 

		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom right
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0  // top left
	};
	glset.uiDataToShaders(color/glm::vec3(255.0f));
	glset.uniform1f(commonData.program, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(commonData.program, "uiOpacity", 0.5f);
}

	float lastColorBoxPickerValue_x = 7;
	float lastColorBoxPickerValue_y = 3;
	float lastColorBoxRangeValue = 3;
	glm::vec3 colorBoxVal = glm::vec3(0);

void GlSet::render(RenderData renderData, std::vector<float>& vertices, unsigned int FBO, PanelData panelData,bool cameraPosChanged, std::vector<float> &axisPointer, float colorBoxPickerValue_x, float colorBoxPickerValue_y,unsigned int paintingSqFBO,float colorBoxColorRangeBarValue, const char* exportFolder, bool jpgFormatChecked, bool pngFormatChecked, bool jpgCheckboxHover, bool pngCheckboxHover, bool doPainting,ExportData exportData, unsigned int depthTexture, unsigned int FBOSq) {
	GlSet gls;

	UserInterface ui;
	CommonData commonData;
	ColorData colorData;
	Utilities util;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(renderData.window, &screenSizeX, &screenSizeY);
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(renderData.window, &mouseXpos, &mouseYpos);


	bool colorBoxValChanged = true;

	if (lastColorBoxPickerValue_x != colorBoxPickerValue_x || lastColorBoxPickerValue_y != colorBoxPickerValue_y || lastColorBoxRangeValue != colorBoxColorRangeBarValue) {
		colorBoxValChanged = true;
	}
	else {
		colorBoxValChanged = false;
	}
	lastColorBoxPickerValue_x = colorBoxPickerValue_x;
	lastColorBoxPickerValue_y = colorBoxPickerValue_y;
	lastColorBoxRangeValue = colorBoxColorRangeBarValue;

	//Axis Pointer
	axisPointerDataToShaders();
	blendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	drawArrays(axisPointer, true);
	blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (renderData.backfaceCulling) {
		enable(GL_CULL_FACE);
		cullFace(GL_BACK);
	}

	//Get Screen Size

	//

	meshDataToShaders();
	std::vector<float> renderVertices = {
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};
	glm::vec3 colorBoxColor = glm::vec3(1.0f, 0.0f, 0.0f);

	std::vector<float> colorBox = {
		// first triangle
		 0.0f,  1.0f, 0.0f,1,1,1,1,1,  // top right
		 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 1.0f,  1.0f, 0.0f,0,1,colorBoxColor.r,colorBoxColor.g + 0.2f,colorBoxColor.b + 0.2f,  // top left 
		// second triangle	  ,0,0,0,
		 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 1.0f,  1.0f, 0.0f,0,1,colorBoxColor.r,colorBoxColor.g + 0.2f,colorBoxColor.b + 0.2f   // top left
	};

	int pickedColor = 0;
	if (cameraPosChanged || exportData.exportImage) {
		Texture txtr;
		viewport(1920, 1080);
		uniform1i(commonData.program, "isRenderTextureMode", 1);
		bindFramebuffer(FBOSq);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uniform1i(commonData.program, "isRenderTextureModeV", 1);
		drawArrays(vertices, false);
		if(!exportData.exportImage)
			drawArrays(renderVertices, false);
		GLubyte* renderedImage = txtr.getTextureFromProgram(GL_TEXTURE10,1080, 1080, 3);

		activeTexture(GL_TEXTURE0);
		texImage(renderedImage,1080,1080, GL_RGB);
		generateMipmap();
		if (exportData.exportImage) {
			if (jpgFormatChecked) {
				txtr.downloadTexture(exportData.path, "rigidPainter Export", 0, 1080, 1080, renderedImage, 3);
			}
			else if (pngFormatChecked) {
				txtr.downloadTexture(exportData.path, "rigidPainter Export", 1, 1080, 1080, renderedImage, 3);
			}
		}
		delete(renderedImage);
		bindFramebuffer(FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uniform1i(commonData.program, "isRenderTextureModeV", 0);
		uniform1i(commonData.program, "depthTexture", 9);
		uniform1i(commonData.program, "renderDepth", 1);
		drawArrays(vertices, false);
		GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
		activeTexture(GL_TEXTURE9);
		texImage(screen, 1920, 1080, GL_RGB);
		generateMipmap();
		uniform1i(commonData.program, "renderDepth", 0);
		delete(screen);

		uniform1i(commonData.program, "isRenderTextureMode", 0);
		bindFramebuffer(0);
		viewport(screenSizeX, screenSizeY);
	}
	if (colorBoxValChanged) {
		
		viewport(1920, 1080);
		uniform1i(commonData.program, "isRenderTextureModeV", 1);
		uniform1i(commonData.program, "isRenderTextureMode", 1);
		bindFramebuffer(FBO);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Deleted

		uniform1i(commonData.program, "isColorBox", 1);
		drawArrays(colorBox, false); //Render Model
		uniform1i(commonData.program, "isColorBox", 0);
		glReadPixels((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080, (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
		uniform3f(commonData.program, "drawColor", colorBoxPixel[0] / 255.0f, colorBoxPixel[1] / 255.0f, colorBoxPixel[2] / 255.0f);

		uniform1i(commonData.program, "isRenderTextureMode", 0);
		uniform1i(commonData.program, "isRenderTextureModeV", 0);
		bindFramebuffer(0);
		viewport(screenSizeX, screenSizeY);
		colorBoxVal.r = colorBoxPixel[0];
		colorBoxVal.g = colorBoxPixel[1];
		colorBoxVal.b = colorBoxPixel[2];
		delete(colorBoxPixel);
	}
	uniform1i(commonData.program, "isRenderTextureModeV", 1);
	uniform1i(commonData.program, "isRenderTextureMode", 1);

	drawArrays(vertices, false); //Render Model
	uniform1i(commonData.program, "isRenderTextureModeV", 0);
	uniform1i(commonData.program, "isRenderTextureMode", 0);
	drawArrays(vertices, false); //Render Model

	disable(GL_CULL_FACE);

	//UI
	//ui.renderMenubar(renderData.window);
	ui.panel(renderData.panelLoc, 0);

	glm::mat4 projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	uniformMatrix4fv(commonData.program, "TextProjection", projection);
	ui.panelChangeButton(renderData.panelLoc,0.8f);//Model Panel
	ui.panelChangeButton(renderData.panelLoc, 0.72f);//Texture Panel
	ui.panelChangeButton(renderData.panelLoc, 0.64f);//Painting Panel
	ui.panelChangeButton(renderData.panelLoc, 0.56f);//Painting Panel
	if(panelData.paintingPanelActive)
		ui.box(0.25f, 0.22f, renderData.panelLoc+ 0.25f, -0.3f, "", colorData.textBoxColor, 0.075f, false, false,0.1f);



	float centerDivider;
	float centerSum;
	uniform1f(commonData.program,"uiOpacity",0.5f);
	if (!panelData.movePanel) {
		centerDivider = 2.0f;
		centerSum = 0;
		 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		uniformMatrix4fv(commonData.program, "TextProjection", projection);
	}
	else {
		centerDivider = 1.0f;
		centerSum = 0.15f;
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		uniformMatrix4fv(commonData.program, "TextProjection", projection);
	}
	if (panelData.modelPanelActive) {

		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider+ centerSum, 0.6f, renderData.modelLoadFilePath, colorData.textBoxColor, 0, true, false,0.9f);
		ui.renderText(commonData.program, "File Path", renderData.panelLoc / centerDivider+ centerSum - 0.05f, 0.64f, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));

		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider+ centerSum, 0.4f, "Load", colorData.buttonColor, 0.022f, false, false, 0.9f);
		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider+ centerSum, 0.0f, "Add Panel", colorData.buttonColor, 0.045f, false, false, 0.9f);
		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider+ centerSum, -0.1f, "Add Sphere", colorData.buttonColor, 0.047f, false, false, 0.9f);

		ui.checkBox(renderData.panelLoc / centerDivider+ centerSum - 0.08f, 0.3f,"Auto triangulate",colorData.checkBoxColor, renderData.isAutoTriangulateHover, renderData.isAutoTriangulateChecked);
		ui.checkBox(renderData.panelLoc / centerDivider+ centerSum - 0.08f, 0.2f, "Backface culling", colorData.checkBoxColor, renderData.isbackfaceCullingHover, renderData.isbackfaceCullingChecked);
	}
	if (panelData.texturePanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.8f, "+ Add Image", colorData.buttonColor, 0.05f, false, false, 0.9f);
	}

	if (panelData.paintingPanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.8f, "Add Mask Texture", colorData.buttonColor, 0.075f, false,false, 0.9f);
		ui.box(0.14f, 0.28f, renderData.panelLoc / centerDivider + centerSum, 0.4f, "", colorData.buttonColor, 0.075f, false,true, 0.9f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, 0.05f, renderData.brushSizeRangeBarValue);
		ui.renderText(commonData.program, "Brush Size", renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.01f, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));

		ui.renderText(commonData.program, util.rgbToHexGenerator(colorBoxVal), renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.56f, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));

		uniform1f(commonData.program, "uiOpacity", 1.0f);
		ui.box(0.012f, 0.025f, renderData.panelLoc / centerDivider + centerSum -0.1f, -0.55f, "", colorBoxVal/glm::vec3(255), 0.075f, false, false, 0.9f);
		uniform1f(commonData.program, "uiOpacity", 0.5f);
		ui.decorationSquare(renderData.panelLoc / centerDivider + centerSum - 0.1f, -0.54f);

		ui.colorBox(renderData.panelLoc / centerDivider + centerSum - 0.02f, -0.3f, colorBoxPickerValue_x, colorBoxPickerValue_y);
		ui.colorRect(renderData.panelLoc / centerDivider + centerSum + 0.1f, -0.3f, colorBoxColorRangeBarValue,FBO,renderData.window);
	}

	if (panelData.exportPanelActive) {
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum, 0.6f,exportFolder, colorData.textBoxColor, 0, true, false, 0.9f); //Path
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - 0.11f, 0.5f, "JPEG", colorData.checkBoxColor, jpgCheckboxHover, jpgFormatChecked);
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum + 0.05f, 0.5f, "PNG", colorData.checkBoxColor, pngCheckboxHover, pngFormatChecked);
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.3f, "Download", colorData.buttonColor, 0.045f, false, false, 0.9f);
	}
	//UI
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	uniformMatrix4fv(commonData.program, "TextProjection", projection);
	if(doPainting)
		drawBrushSizeIndicator(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorBoxVal);
}
GLFWwindow* GlSet::getWindow() {
	glfwInit();
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Mesh Painter", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	return window;
}
void GlSet::setMatrices() {
	CommonData cmnd;

	glm::mat4 textProjection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	uniformMatrix4fv(cmnd.program, "TextProjection", textProjection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	uniformMatrix4fv(cmnd.program, "renderTextureProjection", renderTextureProjection);

	glm::mat4 model = glm::mat4(1.0f);
	uniformMatrix4fv(cmnd.program, "model", model);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	uniformMatrix4fv(cmnd.program, "projection",projection);

	ProjectionData pd;
	pd.modelMat = model;
	pd.projMat = projection;
	//pd.viewMat = view;
}
void GlSet::updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos) {
	CommonData cmnd;
	glm::mat4 view;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0));
	uniformMatrix4fv(cmnd.program, "view", view);
}
void GlSet::getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos) { //Not used
	/*GlSet glset;
	CommonData cmnd;
	ProjectionData pd = glset.setMatrices(cameraPos, originPos);
	return glm::project(vPos, pd.modelMat * pd.viewMat, pd.projMat, glm::vec4(0, 0, 1920, 1080));*/
}

void GlSet::drawLightObject(glm::vec3 lightPos) {
	/*CommonData cmnd;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	int lightColorLoc = glGetUniformLocation(cmnd.program, "lightColor");
	glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 1);
	int modelLoc = glGetUniformLocation(cmnd.program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);*/
}
unsigned int GlSet::createScreenFrameBufferObject() {
	activeTexture(GL_TEXTURE5);
	unsigned int FBO;
	genFramebuffers(FBO);
	bindFramebuffer(FBO);
	unsigned int textureColorbuffer;
	genTextures(textureColorbuffer);
	bindTexture(textureColorbuffer);
	texImage(NULL, 1920,1080,GL_RGB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	unsigned int RBO;
	genRenderbuffers(RBO);
	bindRenderBuffer(RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return FBO;
}
unsigned int GlSet::createScreenFrameBufferObjectSq() {
	activeTexture(GL_TEXTURE10);
	unsigned int FBO;
	genFramebuffers(FBO);
	bindFramebuffer(FBO);
	unsigned int textureColorbuffer;
	genTextures(textureColorbuffer);
	bindTexture(textureColorbuffer);
	texImage(NULL, 1080, 1080, GL_RGB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	unsigned int RBO;
	genRenderbuffers(RBO);
	bindRenderBuffer(RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1080, 1080);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return FBO;
}