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
#include "LigidPainter.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include <dirent.h>


Programs glPrograms;

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
	int isAxisPointerLoc = glGetUniformLocation(glPrograms.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(glPrograms.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 1);
	int uiColorLoc = glGetUniformLocation(glPrograms.program, "uiColor");
	glUniform3f(uiColorLoc, color.x, color.y, color.z);
	int is2DLoc = glGetUniformLocation(glPrograms.program, "is2D");
	glUniform1i(is2DLoc, 1);
}
void GlSet::meshDataToShaders() {
	int isAxisPointerLoc = glGetUniformLocation(glPrograms.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(glPrograms.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(glPrograms.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(glPrograms.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void GlSet::axisPointerDataToShaders() {
	int isAxisPointerLoc = glGetUniformLocation(glPrograms.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 1);
	int isTwoDimensionalLoc = glGetUniformLocation(glPrograms.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(glPrograms.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(glPrograms.program, "isLightSource");
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

//------------Shaders------------
void testShader(unsigned int shader){
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
}
unsigned int createShader(const char* path,unsigned int shaderType){
	Utilities utilities;
	std::string shaderText = utilities.readFile(path);
	const char* shaderSource = shaderText.c_str();

	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	testShader(shader);
	return shader;
}
Programs GlSet::getProgram() {//Prepare shader program | Usen once
	Utilities utilities;

	//Main Program
	unsigned int vertexShader = createShader("LigidPainter/Resources/Shaders/vertexShaderSource.glsl",GL_VERTEX_SHADER);
	unsigned int fragmentShader = createShader("LigidPainter/Resources/Shaders/fragmentShaderSource.glsl",GL_FRAGMENT_SHADER); 

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//Skybox program
	unsigned int skyboxVert = createShader("LigidPainter/Resources/Shaders/skybox.vert",GL_VERTEX_SHADER);
	unsigned int skyboxFrag = createShader("LigidPainter/Resources/Shaders/skybox.frag",GL_FRAGMENT_SHADER); 

	unsigned int skyboxProgram = glCreateProgram();
	
	glAttachShader(skyboxProgram, skyboxVert);
	glAttachShader(skyboxProgram, skyboxFrag);
	glLinkProgram(skyboxProgram);

	glDeleteShader(skyboxVert);
	glDeleteShader(skyboxFrag);


	//Blur program
	unsigned int blurVert = createShader("LigidPainter/Resources/Shaders/blur.vert",GL_VERTEX_SHADER);
	unsigned int blurFrag = createShader("LigidPainter/Resources/Shaders/blur.frag",GL_FRAGMENT_SHADER); 

	unsigned int blurProgram = glCreateProgram();
	
	glAttachShader(blurProgram, blurVert);
	glAttachShader(blurProgram, blurFrag);
	glLinkProgram(blurProgram);

	glDeleteShader(blurVert);
	glDeleteShader(blurFrag);


	//Icons program
	unsigned int iconsVert = createShader("LigidPainter/Resources/Shaders/icons.vert",GL_VERTEX_SHADER);
	unsigned int iconsFrag = createShader("LigidPainter/Resources/Shaders/icons.frag",GL_FRAGMENT_SHADER); 

	unsigned int iconsProgram = glCreateProgram();
	
	glAttachShader(iconsProgram, iconsVert);
	glAttachShader(iconsProgram, iconsFrag);
	glLinkProgram(iconsProgram);

	glDeleteShader(iconsVert);
	glDeleteShader(iconsFrag);



	//Skybox Blur program
	unsigned int skyboxblurVert = createShader("LigidPainter/Resources/Shaders/skyboxblur.vert",GL_VERTEX_SHADER);
	unsigned int skyboxblurFrag = createShader("LigidPainter/Resources/Shaders/skyboxblur.frag",GL_FRAGMENT_SHADER); 

	unsigned int skyboxblurProgram = glCreateProgram();
	
	glAttachShader(skyboxblurProgram, skyboxblurVert);
	glAttachShader(skyboxblurProgram, skyboxblurFrag);
	glLinkProgram(skyboxblurProgram);

	glDeleteShader(skyboxblurVert);
	glDeleteShader(skyboxblurFrag);



	//PBR program
	unsigned int PBRVert = createShader("LigidPainter/Resources/Shaders/PBR.vert",GL_VERTEX_SHADER);
	unsigned int PBRFrag = createShader("LigidPainter/Resources/Shaders/PBR.frag",GL_FRAGMENT_SHADER); 

	unsigned int PBRProgram = glCreateProgram();
	
	glAttachShader(PBRProgram, PBRVert);
	glAttachShader(PBRProgram, PBRFrag);
	glLinkProgram(PBRProgram);

	glDeleteShader(PBRFrag);
	glDeleteShader(PBRVert);



	// program
	unsigned int saturationValBoxVert = createShader("LigidPainter/Resources/Shaders/saturationValBox.vert",GL_VERTEX_SHADER);
	unsigned int saturationValBoxFrag = createShader("LigidPainter/Resources/Shaders/saturationValBox.frag",GL_FRAGMENT_SHADER); 

	unsigned int saturationValBoxProgram = glCreateProgram();
	
	glAttachShader(saturationValBoxProgram, saturationValBoxVert);
	glAttachShader(saturationValBoxProgram, saturationValBoxFrag);
	glLinkProgram(saturationValBoxProgram);

	glDeleteShader(saturationValBoxFrag);
	glDeleteShader(saturationValBoxVert);


	glPrograms.blurProgram = blurProgram;
	glPrograms.iconsProgram = iconsProgram;
	glPrograms.skyboxblurProgram = skyboxblurProgram;
	glPrograms.program = program;
	glPrograms.skyboxProgram = skyboxProgram;
	glPrograms.PBRProgram = PBRProgram;
	glPrograms.saturationValBoxProgram = saturationValBoxProgram;

	return glPrograms;
}
//------------Shaders------------
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
WindowData GlSet::getWindow() {
	glfwInit();
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);
	GLFWwindow* window = glfwCreateWindow(3845,2165, "LigidPainter", NULL, NULL);
	glfwMakeContextCurrent(window);

	int windowMaxWidth;
	int windowMaxHeight;
	glfwGetWindowSize(window,&windowMaxWidth,&windowMaxHeight);

	GLFWimage images[1];
	images[0].pixels = stbi_load("LigidPainter/Resources/Icons/logo-1080x.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	gladLoadGL();
	WindowData windowData;
	windowData.window = window;
	windowData.windowMaxWidth = windowMaxWidth;
	windowData.windowMaxHeight = windowMaxHeight;
	return windowData;
}
Cubemaps GlSet::loadCubemaps(){
	Texture txtr;
	std::vector<std::string> faces
	{
	    "LigidPainter/Resources/Cubemap/Skybox/px.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nx.png",
	    "LigidPainter/Resources/Cubemap/Skybox/ny.png",
	    "LigidPainter/Resources/Cubemap/Skybox/py.png",
	    "LigidPainter/Resources/Cubemap/Skybox/pz.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nz.png"
	};
	unsigned int cubemapTexture = txtr.loadCubemap(faces,GL_TEXTURE13);  
	std::vector<std::string> bluryfaces
	{
	    "LigidPainter/Resources/Cubemap/Skybox/pxblur.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nxblur.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nyblur.png",
	    "LigidPainter/Resources/Cubemap/Skybox/pyblur.png",
	    "LigidPainter/Resources/Cubemap/Skybox/pzblur.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nzblur.png"
	};
	unsigned int cubemapTextureBlury = txtr.loadCubemap(bluryfaces,GL_TEXTURE16); //TODO : Avoid using texture slot for blury cubemap 

	Cubemaps cubemaps;
	cubemaps.blurycubemap = cubemapTextureBlury;
	cubemaps.cubemap = cubemapTexture;

	return cubemaps;
}

BrushMaskTextures GlSet::loadBrushMaskTextures(){
	GlSet glset;
	Texture txtr;

	std::vector<unsigned int> maskTextures;
	std::vector<std::string> maskTextureNames;

	struct dirent *d;
    DIR *dr;
    dr = opendir("./LigidPainter/Resources/Textures");
    if(dr!=NULL)
    {
        for(d=readdir(dr); d!=NULL; d=readdir(dr))
        {
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			std::string fileName =d->d_name;
			if(fileName.size() > 3){
				if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
					maskTextures.push_back(txtr.getTexture("./LigidPainter/Resources/Textures/" + fileName,0,0,false));
					maskTextureNames.push_back(fileName);
				}
			}		
        }
        closedir(dr);
    }
    else
        std::cout<<"\nError Occurred Using Dirent.h!";

	BrushMaskTextures brushMasks;
	brushMasks.names = maskTextureNames;
	brushMasks.textures = maskTextures;

	return brushMasks;
}

LigidCursors GlSet::loadCursors(){

	GLFWimage images[1];
	stbi_set_flip_vertically_on_load(false);
	images[0].pixels = stbi_load("LigidPainter/Resources/Icons/PointerIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	GLFWcursor* pointerCursor = glfwCreateCursor(images,15,0);
	stbi_image_free(images[0].pixels);

	images[0].pixels = stbi_load("LigidPainter/Resources/Icons/DefaultIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	GLFWcursor* defaultCursor = glfwCreateCursor(images,7,0);
	stbi_image_free(images[0].pixels);

	images[0].pixels = stbi_load("LigidPainter/Resources/Icons/DropperCursor.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	GLFWcursor* dropperCursor = glfwCreateCursor(images,0,30);
	stbi_image_free(images[0].pixels);

	LigidCursors cursors;
	cursors.pointerCursor = pointerCursor;
	cursors.defaultCursor = defaultCursor;
	cursors.dropperCursor = dropperCursor;

	return cursors;
}

void GlSet::usePBRShader(unsigned int program,PBRShaderData data){
	glUseProgram(program);

	//Vert
	uniformMatrix4fv(program,"view",data.view);
	uniformMatrix4fv(program,"mirroredView",data.mirroredView);
	uniformMatrix4fv(program,"projection",data.projection);

	//Frag
	uniform1i(program,"screenMaskTexture",data.screenMaskTexture);
	uniform1i(program,"mirroredScreenMaskTexture",data.mirroredScreenMaskTexture);
	uniform1i(program,"useMirror",data.useMirror);
	uniform3fv(program,"drawColor",data.drawColor);
	uniform1i(program,"depthTexture",data.depthTexture);
	uniform1i(program,"mirroredDepthTexture",data.mirroredDepthTexture);
	uniform3fv(program,"viewPos",data.viewPos);
	uniform3fv(program,"mirroredViewPos",data.mirroredViewPos);
	uniform1i(program,"bluryskybox",data.bluryskybox);
	uniform1i(program,"material.diffuse",data.materialDiffuse);
}

void GlSet::useSkyBoxShader(unsigned int program, SkyBoxShaderData data){
	glUseProgram(program);

	//Vert
	uniformMatrix4fv(program,"view",data.view);
	uniformMatrix4fv(program,"projection",data.projection);

	//Frag
	uniform1i(program,"skybox",data.skybox);
}

void GlSet::useBlurShader(unsigned int program, BlurShaderData data){
	glUseProgram(program);

	//Vert
	uniform1f(program,"brushBlurVal",data.brushBlurVal);
	uniform1i(program,"isRenderVerticalBlur",data.isRenderVerticalBlur);
	uniformMatrix4fv(program,"renderTextureProjection",data.renderTextureProjection);

	//Frag
	uniform1i(program,"inputTexture",data.inputTexture);
}

void GlSet::useSaturationValBoxShader(unsigned int program, SaturationValShaderData data){
	glUseProgram(program);

	//Vert
	uniformMatrix4fv(program,"renderTextureProjection",data.renderTextureProjection);

	//Frag
	uniform3fv(program,"boxColor",data.boxColor);
}