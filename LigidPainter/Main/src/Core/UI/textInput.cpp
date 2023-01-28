#include<iostream>

#include <string>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"

bool UserInterface::textInput(int key, int action,bool caps,std::string &text,int threshold,GLFWwindow* window,int activeChar){
    bool valueChanged = false;
	bool add = false;

	if(activeChar == 0){
		add = true;
	}

    if(key >= 320 && key <=329){
		//Numpad Optimization
		key -= 272;
	}
	if(action == 0 || action == 2){ //Take input

		if(text.size() < threshold){
			if(key == 32){
				//Space
				text.insert(text.end()+ activeChar,' ');
                valueChanged = true;
			}
			else if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS  || glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS){

			}
			else if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS  || glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS){
				if(key == 61){
					text.insert(text.end()+ activeChar,'_');
					valueChanged = true;
				}
			}
			if(key != GLFW_KEY_RIGHT_SHIFT && key != GLFW_KEY_LEFT_SHIFT && key != GLFW_KEY_LEFT_CONTROL && key != GLFW_KEY_RIGHT_CONTROL && key != GLFW_KEY_LEFT_ALT && key != GLFW_KEY_RIGHT_ALT && key != GLFW_KEY_TAB && key != GLFW_KEY_ENTER){
				if(key == 333 || key == 61){
					text.insert(text.end()+ activeChar,'-');
					valueChanged = true;
				}
				else if(isalpha((char)key)){
						if(!caps){
							text.insert(text.end()+ activeChar,(char)key+32);
            	        	valueChanged = true;
						}
						else{
							text.insert(text.end()+ activeChar,(char)key);
            	        	valueChanged = true;
						}
					}

				else if(isdigit((char)key)){
					text.insert(text.end()+ activeChar,(char)key);
            	    valueChanged = true;
				}
			}
		}

		if(key == 259){
			//Backspace
			if(text.size() != 0){
				text.erase(text.end()+ activeChar-1);
                valueChanged = true;
			}
		}
	}
    return valueChanged;
}

bool UserInterface::textInputHex(int key, int action, std::string &text, int &activeChar){
    bool valueChanged = false;
    if(action == 0 || action == 2){ 
        if(isdigit((char)key)){
	    	//Add decimal numbers
	    	text[activeChar] = (char)(key);
            valueChanged = true;
	    	activeChar++;
	    }
	    else{
	    	//Add hexadecimal numbers
	    	const char* chars = "ABCDEF";
	    	for (int i = 0; i < 6; i++)
	    	{
	    		if(key == chars[i]){
	    			text[activeChar] = (char)(chars[i]+32);
                    valueChanged = true;
	    			activeChar++;
	    		}
	    	}
	    }
        if(key == 259 && activeChar != 0 && text[activeChar-1] != '#'){
	    	//Backspace
	    	text[activeChar] = '0';
            valueChanged = true;
            activeChar--;
	    }
    }
    return valueChanged;
}