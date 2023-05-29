#ifndef LGDTEXTRENDERER_HPP
#define LGDTEXTRENDERER_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Font.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class TextRenderer
{
private:
    /* data */
    glm::vec4 rndrTxt(Shader shader,std::string text,float x,float y,float z,float maxX,bool multipleLines,float scale,float mostLeft,int index,bool render){
		glm::vec4 data;
		data.w = 0.f;
		//.x = Returns the text's starting position int x axis
		//.y = Returns the text's ending position int x axis
		//.z = hitTheBoundaries
		//.w = position of the char in the given index


		//Text is rendered using button shader
		//Shader is the shader class of the button shader
		//X Y Z : Is the location of the text, z = depth value
		//MaxX is the location value in the x axis where you want text to end or get to new line (buttons right side's x coordinate)
		//If mutliple line is set to true, multiple lines will be enabled
		//Scale : scale of the text. 1 is default and pretty big. 
		//Most left is the location value in the x axis where text can't go further left of that value (buttons left side's x coordinate)

		float lastXText = x;
        
        glActiveTexture(GL_TEXTURE0);
    
	    std::string::const_iterator c;
        
        shader.setInt("txtr",0);
	    shader.setInt("renderText",1);

		//Get the location of the text's last char
	    float overallX = 0.f;
		for (std::string::const_iterator aC = text.begin(); aC != text.end(); aC++){
	    	character ch = font.characters[*aC];//Get the current char
            
			//To the right
	    	overallX += (ch.Advance >> 6) * scale / 1.2f;
		}
		
		x-=overallX/2.f; //Allign the text in the middle

		//Check if the text's left side is out of the boundaries
	    float overallX2 = 0.f;
		bool hitTheBoundaires = false;
		for (std::string::const_iterator aC = text.begin(); aC != text.end(); aC++){
	    	character ch = font.characters[*aC];//Get the current char
            
			if(maxX < x + overallX2) {
				hitTheBoundaires = true;
				break;
			}
			
			//To the right
	    	overallX2 += (ch.Advance >> 6) * scale / 1.2f;
		}

		if(hitTheBoundaires) //If the text's left side is out of the boundaries align the text on the left side of the button
			x = mostLeft;

		data.x = x;

		//Render all the chars in the text parameter
	    int counter = 0;
	    for (c = text.begin(); c != text.end(); c++)
	    {
	    	character ch = font.characters[*c];//Get the current char
	    	if(*c == '\n'){//New line if the char is \n
                if(!multipleLines)//Break the for loop & stop rendering the text if multiple lines are not allowed 
                    break;

	    		x=lastXText; //Go to the start
	    		y-=0.03f; //New line
	    	}
	    	else{
	    		if(maxX < x){ //Go to new line if the text is out of it's boundaries
	    			x=lastXText;
	    			y-=(ch.Size.y) * scale * 1.3f;
	    			if(!multipleLines) //Break the for loop & stop rendering the text if multiple lines are not allowed 
	    				break;
	    		}

                //Calculate the position of the char
	    		float xpos = x + ch.Bearing.x * scale;
	    		float ypos = y + (ch.Size.y - ch.Bearing.y ) * scale;

                //Calculate the size of the char
	    		float w = ch.Size.x * scale * 0.8f;
	    		float h = ch.Size.y * scale;

                //Set the transform values
	    		shader.setVec2("scale",glm::vec2(w/1.7,h/1.7));
	    		shader.setVec3("pos",glm::vec3(xpos + w/1.7,ypos,z));

                //Draw the char
	    		glBindTexture(GL_TEXTURE_2D,ch.TextureID);
				if(render)
	    			glDrawArrays(GL_TRIANGLES, 0, 6);

				if(counter == index)
					data.w = x;

                //To the right
	    		x += (ch.Advance >> 6) * scale / 1.2f;


	    		counter++;
	    	}
	    }
	    shader.setInt("renderText",0);

		data.y = x;
		data.z = hitTheBoundaires;

		return data;
	}
public:
    Font font;
	
	//Current key state
	bool keyInput = false;
	bool caps = false;
	char key = 0;
	int mods = 0;

    TextRenderer(/* args */){}
    TextRenderer(Font font){
        this->font = font;
    }

	

    glm::vec3 renderText(Shader shader,std::string text,float x,float y,float z,float maxX,bool multipleLines,float scale,float mostLeft){
		return rndrTxt(shader,text,x,y,z,maxX,multipleLines,scale,mostLeft,0,true);
	}
    glm::vec3 renderText(Shader shader,std::string text,float x,float y,float z,float maxX,bool multipleLines,float scale,float mostLeft,bool active,int &activeChar,int &activeChar2,Timer &timer){
		//If the active char is not equal to the active char 2 the chars between them will be selected

		//Render the text and get the position of the chars from the text
		glm::vec4 result = rndrTxt(shader,text,x,y,z,maxX,multipleLines,scale,mostLeft,activeChar,true);
		
		//Get the position of the chars from the text (w axis will contain the position of the char at the index of activeChar2)
		glm::vec4 resultX = rndrTxt(shader,text,x,y,z,maxX,multipleLines,scale,mostLeft,activeChar2,false);
		
		//Render the insertion point cursor
		if(active){
			//Set the transform values
        	shader.setFloat("radius",     0    );
        	shader.setInt("outline" ,     false      ); 
        	shader.setInt("outlineExtra" ,     false     ); 

			if(activeChar == text.size()){ //Render at the end of the text
				shader.setVec2("scale",glm::vec2(5 * scale,35 * scale));
	    		shader.setVec3("pos",glm::vec3(result.y + 5 * scale,y,z));
			}
			else{ //Render near the selected char
				shader.setVec2("scale",glm::vec2(5 * scale,35 * scale));
	    		shader.setVec3("pos",glm::vec3(result.w + 5 * scale,y,z));
			}

			//TODO Special timer for the text renderer
			if(timer.seconds % 2 == 0) //Hide and show the insertion point cursor every second
	    		glDrawArrays(GL_TRIANGLES, 0, 6);


			//Multiselection 
			if(activeChar != activeChar2){ 
				if(activeChar == text.size()){
					shader.setVec2("scale",glm::vec2((result.y - resultX.w)/2.f,35 * scale));
	    			shader.setVec3("pos",glm::vec3(result.y - ((result.y - resultX.w)/2.f),y,z));
				}
				else{
					shader.setVec2("scale",glm::vec2((result.w - resultX.w)/2.f,35 * scale));
	    			shader.setVec3("pos",glm::vec3(result.w - ((result.w - resultX.w)/2.f),y,z));
				}

	    		glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		return result;
	}

	void processTextInput(std::string &text,int &activeChar,int &activeChar2){
		if(keyInput){
			//Delete
			if(key == GLFW_KEY_BACKSPACE-256 && activeChar != 0){ //Multiselected
				if(activeChar2 != activeChar){
					if(activeChar < activeChar2)
						text.erase(text.begin()+activeChar,text.begin()+activeChar2);
					else
						text.erase(text.begin()+activeChar2,text.begin()+activeChar);

					if(activeChar < 0)
						activeChar = 0;
					if(activeChar > text.size())
						activeChar = text.size();
					
					activeChar2 = activeChar;
				}
				else{ //Single
					text.erase(text.begin()+activeChar-1);
					activeChar--;
					activeChar2 = activeChar;
				}

			}
			else if(key == GLFW_KEY_LEFT-256){
				if(mods == 1){//Shift pressed 
					if(activeChar2 > 0)
						activeChar2--;
				}
				else{
					if(activeChar > 0)
						activeChar--;
					activeChar2 = activeChar;
				}
			}
			else if(key == GLFW_KEY_RIGHT-256){
				if(mods == 1){//Shift pressed 
					if(activeChar2 < text.size())
						activeChar2++;
				}
				else{
					if(activeChar < text.size())
						activeChar++;
					activeChar2 = activeChar;
				}
			}
			else if(key == GLFW_KEY_CAPS_LOCK-256){
                this->caps = !this->caps;
			}
			else if(mods == 0){
				if(!this->caps && isalpha(key))
					key+=32;
					
				text.insert(text.begin() + activeChar,key);
				activeChar++;
				activeChar2 = activeChar;
			}
		}
	}
};

#endif