# VulkanRenderer

## Intro
In this repository I will be making 2 renderers using the Vulkan API, one 3D and one 2D.  

As vulkan is not easy to start with without help, i used this tutorial to get me started:  
https://vulkan-tutorial.com.  
Later I also bought and used the official Vulkan programming guide from OpenGL for a deeper understanding.

## Motivation
While studying Game Development at Digital Arts and Entertainment in Kortrijk, Belgium, we made a DirectX renderer in our Graphics Programming class, this made me look into more APIs for rendering and landed on Vulkan to make my own Renderer with.  
This renderer is alsointegrated into my self made engine, as my engine project grew, this project became more of a testing ground for vulkan.  

## Inital goals
I wanted to deepen my understanding of graphics programming and in the long run build a small engine that i built myself from the ground up.  
I wanted to create a renderer completely by myself (with the help of tutorials but no starting project) as a challenge to see how far i could get.  

## Curren capabilities
### Model loading
3D model can be loaded from .obj files and displayed on screen.  

### Texture loading
Textures can be loaded from a variety of file types. Textures can be used in materials to be sent to shaders so the user can use it however they want. Currently diffuse, gloss, specular and normal maps are supported.  

### Global light
Currently there is a single global directional light that is used for shading in any way the user wants to. The light has a direction, color and intensity that can be customized.  

### Camera movement
A simple camera movement is implemented so that the user can see their objects from different angles. 

### GUI
ImGui has been implemented and can be used for debugging purposes or anything else the user wants to.  

### Skybox rendering
The renderer can render a simple skybox when given 6 images to form a cubemap.  

## Future goals
### Shadow mapping
I am currently working on adding shadow mapping into the renderer.

### Particles  
Particles are often an essential part of making a game look finished and giving the player feedback on his actions. They have always fascinated me and i have wondered for a long time how they work, so that's why i would love to get into implementing them myself and seeing how they work.  

### Compute shaders  
I have not read a lot about compute shaders, but what i do know about them makes them seem extremely powerful and useful so i would love to learn all about them.  

### Instanced rendering
Instanced rendering has always seemed like a very interesting concept to me and i would love to deepen my understanding of it and how to apply it in my renderer.  

### More lights
Currently my renderer only supports a single directional light, this is of course not ideal for any 3D application.  
I would like to add support for multiple lights at the same time.

### More light types
There are many different types of light in graphics, i would like to implement as many of them as i can.  
Currently the lights i would like to add are:  
                    -Point light  
                    -Spotlight  
                    -Sky light  

## Libraries 
### Window handling and input
For window handling I chose GLFW, in class we always used OpenGl and SDK and i wanted to explore other options, the Vulkan Tutorial also adviced using GLFW.  

### GUI
In class we learned about Dear ImGui, but i feel we didn't explore it to it's full potential so i chose to explore it further on my own and implement it into my renderer.  

### Math
For math support, GLM i chose glm as it was the library i was most familiar with, i usually enjoy experimenting and learning new libraries but a math library didn't seem interesting enough to explore for me. GLM also seems to be the best in communicating with Vulkan.  

### Json file reading
I had worked with Json files before in C# but never in C++, so I had to learn an look for a library that could fit my requirements and I ended up choosing RapidJSON.
