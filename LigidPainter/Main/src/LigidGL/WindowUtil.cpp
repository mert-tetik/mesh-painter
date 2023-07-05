/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    See the ./README.md file for the description

*/

#include <iostream>

#include "LigidGL/LigidGL.hpp"


#if defined(_WIN32) || defined(_WIN64)
//User has Windows
#include <Windows.h>

#elif defined(__APPLE__)
//User has MacOS
#import <Cocoa/Cocoa.h>


#elif defined(__linux__)
//User has Linux
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <utility>

#endif


bool LigidWindow::shouldClose(){

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment
    
    /* 
        The GetMessage function retrieves a message from the calling thread's message queue.
        If there are no messages, it blocks until a message arrives or the application is terminated.
        Here, we are checking if GetMessage returns 0, which indicates there are no more messages in the queue.
        The message structure (msg) is filled with the message details if GetMessage retrieves a message.
        The hWnd parameter specifies the handle of the window whose messages are to be retrieved.
        The next two parameters (wMsgFilterMin and wMsgFilterMax) filter the messages to be retrieved.
        In this case, they are set to 0, which means all messages are retrieved.

        Returns false if there are no messages which means user is trying to close the window
    */
    return !GetMessage(
                        &this->msg, 
                        this->window, 
                        0, 
                        0
                    );
#endif

}


void LigidWindow::pollEvents(){
#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    // The TranslateMessage function translates virtual-key messages into character messages.
    // It is typically called to process keyboard input before dispatching it to the appropriate window procedure.
    // This function is necessary for translating virtual-key messages (such as WM_KEYDOWN and WM_KEYUP)
    // into character messages (such as WM_CHAR) for proper handling of keyboard input.
    TranslateMessage(&this->msg);

    // The DispatchMessage function dispatches a message to a window procedure.
    // It sends the message to the window procedure that the message's hWnd and message type specify.
    // The window procedure handles the message and performs appropriate actions based on the message type.
    DispatchMessage(&this->msg);


    //--- Call the callback functions ---

    // If received a mouse position change message
    if(this->msg.message == WM_MOUSEMOVE){

        // Call the mouse position callback function set by the user using message data
        this->mousePosCallback(
                                LOWORD(msg.lParam), //Received mouse x pos  
                                HIWORD(msg.lParam)  //Received mouse y pos
                            );
    
    }

#endif
}


void LigidWindow::close(){
#if defined(_WIN32) || defined(_WIN64)

    //* User in windows environment
    
    // Closes / destroys the hwnd window
    DestroyWindow(this->window);
    
#endif
} 


void LigidWindow::makeContextCurrent(){

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    HDC hdc = GetDC(this->window);    // Obtain the device context handle for the window

    // Create and activate the OpenGL rendering context
    this->openGLContext = wglCreateContext(hdc);    // Create an OpenGL rendering context
    wglMakeCurrent(hdc, this->openGLContext);       // Make the created context current for the given device context


#elif(__APPLE__)


    //* User in MacOS environment
    
    // Make the OpenGL context current
    this->openGLContext->makeCurrentContext();


#elif(__linux__)


    //TODO Ask for a existing display 

    // Open the X11 display connection
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        // Failed to open X11 display
        // Handle the error here
    }

    // Get the default screen
    int screen = DefaultScreen(display);

    // Make the GLX context current
    if (!glXMakeCurrent(display, this->window, this->glxContext)) {
        // Failed to make context current
        std::cout << "ERROR : Can't make the context current!" << std::endl;
    }

#endif

}