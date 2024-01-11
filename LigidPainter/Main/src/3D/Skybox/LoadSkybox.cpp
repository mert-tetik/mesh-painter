/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"


#include "../../thirdparty/stb_image.h"


void Skybox::load(std::string path) //Path to the folder
{
	std::vector<std::string> faces //Each face of the skybox 
	{
		path + "/px.png", 
		path + "/nx.png",
		path + "/ny.png",
		path + "/py.png",
		path + "/pz.png",
		path + "/nz.png"
	};

	int width, height, nrChannels;//Of the face of the skybox
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		Texture txtr;
		unsigned char *data = txtr.getTextureDataViaPath(faces[i].c_str(),width,height,nrChannels,3,true);

		if (data) //Loaded
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
			
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 
							0, 
							GL_RGB8, 
							width, 
							height,
							0, 
							GL_RGB, 
							GL_UNSIGNED_BYTE, 
							data
			);
			
			LGDLOG::start<< "Loaded " << faces[i] << LGDLOG::end;
			stbi_image_free(data);
		}
		else //An error occured while loading
		{
			LGDLOG::start<< "Cubemap texture failed to load at path: " << faces[i] << LGDLOG::end;
			stbi_image_free(data);
		}
	}
}