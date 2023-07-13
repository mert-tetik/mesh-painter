/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Text renderer constructors and text data loading

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"


//Forward declerations for the util functions
void updateInsertionPointCursor(Timer &timer);
void deletion(int& activeChar, int& activeChar2, std::string &text);
void leftArrow(int mods, int &activeChar, int &activeChar2, std::string &text);
void rightArrow(int mods, int &activeChar, int &activeChar2, std::string &text);
void charInput(int &key, bool &caps, std::string& text, int& activeChar, int& activeChar2);
void modsControl(int &key, int &activeChar, int &activeChar2, std::string &text, LigidWindow window);





void TextRenderer::processTextInput(std::string &text,LigidWindow window,int &activeChar,int &activeChar2, int &textPosCharIndex){
	//If pressed to a key
    if(keyInput){


        //Show the insertion point cursor in case if invisible
        updateInsertionPointCursor(timer);
		
		//Delete
		if(key == LIGIDGL_KEY_BACKSPACE){
            deletion(activeChar,activeChar2,text);
		}

        //Left arrow
		else if(key == LIGIDGL_KEY_LEFT){
            leftArrow(mods,activeChar,activeChar2,text);
		}

        //Right arrow
		else if(key == LIGIDGL_KEY_RIGHT){
            rightArrow(mods,activeChar,activeChar2,text);
		}

        //Caps lock
		else if(key == LIGIDGL_KEY_CAPS_LOCK){
			this->caps = !this->caps;
		}

        //Basic char input
		else if(mods == 0){ //If shift, control & alt keys are released
            
            if(activeChar2 != activeChar)
                deletion(activeChar,activeChar2,text);
            
            charInput(key,caps,text,activeChar,activeChar2);
		
        }
        else if(mods == 2){ //Control pressed
            modsControl(key, activeChar, activeChar2, text, window);
        }
	}
}






//---------------- UTIL FUNCTIONS -----------------

void updateInsertionPointCursor(Timer &timer){
	timer.seconds = 2;
	timer.lastTimeT = LigidGL::getTime();
}

void deletion(int& activeChar, int& activeChar2, std::string &text){
    
    //If selected multiple characters
    if(activeChar2 != activeChar){
        
        //If selected thru right delete the selected chars between activeChar & activeChar2
        if(activeChar < activeChar2)
            text.erase(text.begin() + activeChar + 1,text.begin()+activeChar2 + 1);
        
        //If selected thru left delete the selected chars between activeChar & activeChar2
        else
            text.erase(text.begin() + activeChar2 + 1,text.begin()+activeChar + 1);

        if(activeChar < -1)
            activeChar = -1;
        
        if(activeChar > text.size()-1)
            activeChar = text.size()-1;
        
        activeChar2 = activeChar;
    }

    //If selected a single character
    else if (activeChar != -1){ 
        text.erase(text.begin() + activeChar);
        activeChar--;
        activeChar2 = activeChar;
    }
}


void charInput(int &key, bool &caps, std::string& text, int& activeChar, int& activeChar2){
    //If char is a letter & capslock is false lowercase the char
    if(key < 127)
        if(!caps && isalpha(key))
            key+=32;

    if(key >= 320 && key <=329){
		//Numpad Optimization
		key -= 272;
	}

    text.insert(text.begin() + (activeChar + 1), key);

    activeChar++;
    activeChar2 = activeChar;
}

//direction 0 is left and 1 is right
int getTheIndexOfTheSpace(int currentIndex, std::string text, int direction){

    if(currentIndex < 1)
        return -1;

    if(direction == 0){
        int counter = 0;
        int i = currentIndex;

        while(text[i] != ' ' || counter == 0){
            i--;

            if(i < 0)
                return -1;

            counter++;
        }
        return i;
    }
    
    if(direction == 1){
        int counter = 0;
        int i = currentIndex;
        
        while(text[i] != ' ' || counter == 0){
            i++;

            if(i > text.size()-1)
                return text.size()-1;
        
            counter++;
        }
        return i;
    }
}

void leftArrow(int mods, int &activeChar, int &activeChar2, std::string &text){
	//Shift pressed (multiselected)
    if(mods == 1){
		if(activeChar2 > -1)
			activeChar2--;
	}

	//Control pressed or control and shift (pass to the end)
    else if(mods == 2 || mods == 3){
        //If multiselected
        if(activeChar2 != activeChar)
            activeChar2 = getTheIndexOfTheSpace(activeChar,text,0);
        
        //If not multiselected
        else{
            activeChar = getTheIndexOfTheSpace(activeChar,text,0);
            activeChar2 = activeChar;
        }
    }
	
    //Shift nor control is not pressed
    else{
		if(activeChar > -1)
			activeChar--;
		activeChar2 = activeChar;
	}
}

void rightArrow(int mods, int &activeChar, int &activeChar2, std::string &text){
	//Shift pressed (multiselected)
    if(mods == 1){
		if(activeChar2 < text.size()-1)
			activeChar2++;
	}
    
    //Control pressed (pass to the end)
    else if(mods == 2){
        //If multiselected
        if(activeChar2 != activeChar)
            activeChar2 = getTheIndexOfTheSpace(activeChar,text,1);
        
        //If not multiselected
        else{
            activeChar = getTheIndexOfTheSpace(activeChar,text,1);
            activeChar2 = activeChar;
        }
    }

    //Shift nor control is not pressed
	else{
		if(activeChar < (int)text.size()-1)
			activeChar++;
            
		activeChar2 = activeChar;
	}
}

void modsControl(int &key, int &activeChar, int &activeChar2, std::string &text, LigidWindow window){
    //Control + V
    if(key == LIGIDGL_KEY_V){
        if(activeChar2 != activeChar)
            deletion(activeChar,activeChar2,text);
        
        std::string clipText = LigidGL::getClipboardText();
        
        text.insert( (activeChar + 1) , clipText );
            
        activeChar += clipText.size();  
        activeChar2 = activeChar; 
    }

    //Control + C
    if(key == LIGIDGL_KEY_C){
        std::string clipText;
        
        if(activeChar2 != activeChar){
            if(activeChar > activeChar2)
                clipText = text.substr(activeChar2 + 1, activeChar - activeChar2);
            else
                clipText = text.substr(activeChar + 1, activeChar2 - activeChar);
        }

        else{
            clipText = text;
        }

        LigidGL::setClipboardText(clipText.c_str());

    }
}