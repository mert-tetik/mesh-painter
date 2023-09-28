/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef NODE_HPP
#define NODE_HPP

//OpenGL & Window
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//GLM - Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string> //std::string
#include <iostream> //input & outputs file streams
#include <vector> //std::vector

#include "UTIL/Util.hpp"

#include "GUI/Elements/Elements.hpp"

//Forward declarations
struct Element;
struct NodePanel;


/// @brief Represents a single node connection
struct NodeConnection{
    //Both inputs & outputs of the node use that structure.
    //std::vector<NodeConnection> connections from the NodeIO class
    //Example :
    //NodeIO connectedIO = meshNodeScene[nodeIndex].IOs[inputIndex];

    //Which node the connection is connected
    int nodeIndex; 
    //Which input the connection is connected
    int inputIndex;

    //Constructor for the connection
    NodeConnection(int nodeIndex,int inputIndex);
};


//!------------------------------NODEIO-----------------------------


class NodeIO
{
private:
    /* data */
public:
    std::vector<NodeConnection> connections;
    std::string title;
    Element element; 
    Button IOCircle;
    glm::vec4 color; 
    unsigned int result; 
    int state;

    //Constructor
    NodeIO(std::string title ,Element element,glm::vec4 color,int state);
};

//!------------------------------NODE-----------------------------

#define MATERIAL_MASK_NODE 3
#define MATERIAL_ID_NODE 2
#define MATERIAL_NODE 1
#define MESH_NODE 0


class Node
{
private:
    //-------------------- UTILITY FUNCTIONS --------------------

    bool renderBarriers(Panel &nodeEditorPanel);
    void drawLine(glm::vec2 src, glm::vec2 dest,Panel nodeEditorPanel, int direction);
    void clearConnections(int nodeIParam,int IOIParam);
    bool doHaveConnection(int nodeIParam,int IOIParam);
    void addVectors(std::vector<NodeIO>& orgVec, std::vector<NodeIO>& addedVec);
    void createPanelUsingIOs();
    void getHoveredInputs(int &nodeIParam, int &IOIndexParam);
    int getStateData(int nodeI, int outI);
    void getTheIOConnectedToTheInput(int &nodeIParam, int &IOIParam,int currentNodeI, int currentIOI);

public:
    void createConnection(int nodeIParam,int IOIParam,int currentNodeI,int currentIOI);
    
    /// @brief Inputs & outputs of the node
    std::vector<NodeIO> IOs;

    /// @brief if the nodeIndex is @ref MATERIAL_NODE indicates the belonged material's unique ID 
    int materialID;
    
    /// @brief indicates the form of the node.
    /// Is MATERIAL_NODE or MESH_NODE
    int nodeIndex; 
    
    /// @brief the node is rendered using that tpanel
    Panel nodePanel;  
    
    /// @brief the button on top of the node (used to move the node and displays the title of the node)
    Button barButton; 
    
    /// @brief scale of the node
    /// ! (is not used! Use the nodePanel.scale instead)
    glm::vec2 scale = glm::vec2(10,20); 
    
    /// @brief position of the node
    /// ! (is not used! Use the nodePanel.pos instead)
    glm::vec3 pos = glm::vec3(50,50,0.8f); 
    
    /// @brief true if the cursor on the barriers.
    /// Barriers are 3 squares rendered outside of the node scene panel.
    /// Covers all the screen but the node scene panel and prevents rendering the node outside of the node scene panel.
    /// Barriers are rendered for each node.
    /// if cursorOnBarriers == false than nodePanel.hover must be true 
    bool cursorOnBarriers;

    /// @brief Default constructor
    Node();
    
    /*!
    * @brief Create the node class
    * @param nodeIndex is @ref MATERIAL_NODE or @ref MESH_NODE. Indicates what form does the node has.
    * @param materialID used if the nodeIndex is : @ref MATERIAL_NODE indicates the unique ID value of the material used by the material node 
    */
    Node(int nodeIndex, int materialID);

    /// @brief Render the node & manage inputs & outputs 
    /// @param currentNodeIndex which node is rendered (index of the nodeScene)
    void render(Timer &timer,Panel nodeEditorPanel,int currentNodeIndex, NodePanel& nodePanel, int textureRes, bool doMouseTracking);

    /// @brief Replaces the IOs vector with the new inputs & outputs (+ generates the node panel)  
    void uploadNewIOs(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs);
    
    /// @brief Replaces the IOs vector with the new IOs generated with meshes of the 3D model (used for the mesh node) (+ generates the node panel)  
    void uploadNewIOs();
};

namespace NodeScene{

    /// @brief Renders the node list 
    void render(Timer &timer,  Panel nodeEditorPanel, NodePanel& nodePanel, bool doMouseTracking);
    
    /// @brief Adds the given node to the scene 
    void addNode(const Node node);

    /// @brief Returns the node pointer at the corresponding index from the node scene
    Node* getNode(int index);

    /// @brief Deletes all the nodes in the nodes cene
    void clearArray();

    /// @brief Returns the count of the nodes in the node scene
    int getArraySize();

    /// @brief remove the connections with invalid indices 
    ///        (if the node is connected to a nonexistent node or an input)
    ///        (for example : connection node index = 5 & the meshNodeScene size : 4)
    void updateAllTheNodeConnections();
    
    /// @brief deletes the node with the given index from meshNodeScene and updates all the nodes
    /// @param index remove the meshNodeScene[index]
    void deleteNode(int index);

    Mesh processNode(Node &node, Mesh& mesh, int textureRes);

    /// @brief Updates the result textures of the every input of the mesh node
    void updateNodeResults( int textureRes, int updateNodeI);

    std::vector<Node>* getNodeArrayPointer();
};

#endif