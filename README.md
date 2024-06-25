# VulkanRenderer

## Intro
In this repository I will be making 2 renderers using the Vulkan API, one 3D and one 2D.  

As vulkan is not easy to start with without help, i used this tutorial to get me started:  
https://vulkan-tutorial.com
Later I also bought and used the official Vulkan programming guide from OpenGL for a deeper understanding.

## Motivation
While studying Game Development at Digital Arts and Entertainment in Kortrijk, Belgium, we made a DirectX renderer in our Graphics Programming class, this made me look into more APIs for rendering and landed on Vulkan to make my own Renderer with.  
This renderer is alsointegrated into my self made engine, as my engine project grew, this project became more of a testing ground for vulkan.  

## Inital goals
I wanted to deepen my understanding of graphics programming and in the long run build a small engine that i built myself from the ground up.  
I wanted to create a renderer completely on myself (with the help of tutorials but no starting project) as a challenge to see how far i could get.  

## Curren capabilities
### Model loading
3D model can be loaded from .obj files and displayed on screen.  

### Texture loading
Textures can be loaded from a variety of file types. Textures can be used in materials to be sent to shaders so the user can use it however they want. Currently diffuse, gloss, specular and normal maps are supported.  

### Camera movement
A simple camera movement is implemented so that the user can see their objects from different angles.  

## Future goals
### Shadow mapping
I am currently working on adding shadow mapping into the renderer.

### Particles  
In the future i would like to learn how Compute shaders work and how to use them to create particles in Vulkan.  

### Instanced rendering
Instanced rendering has always seemed like a very interesting concept to me and i would love to deepen my understanding of it and how to apply it in my renderer.  


## Libraries 
# Window handling and input
Windows handling and input are both handles by GLFW with the use of the Vulkan extension 