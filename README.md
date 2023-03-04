# Stella Toy Renderer

This is a tiny renderer project, mostly for learning C++/ OpenGL programing and some graphics techniques . Currently I name it Stella.

Several graphics algorithms are implemented in a rather simple way: Physically based material (Cook-Torrance BRDF), Deferred Shading, Shadow Mapping (Cascaded and PCF filter), Screen Space Reflection (with Hi-Z buffer accelerated), Image-based lighting, simple Reflective Shadow Map, Temporal AA (with jittering and velocity buffer). 

Also, there are features like perspective camera, normal mapping, simple HDR, Directional light(Point light and shadow not yet implemented).

Features I would like to try in the feature: high-quality bloom, forward+ pipeline, area light, VXGI.



## Render Demos

<center> <h4>Physically based material + Image-based Lighting</h4> </center>

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/01.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/02.png)



<center> <h4>Cascaded Shadow Mapping</h4> </center>

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/12.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/13.png)

<center> <h4>Sponza With IBL/Screen Space Refelction</h4> </center>

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/03.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/06.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/05.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/04.png)

<center> <h4>Temporal AA</h4> </center>

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/10.png)

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/09.png)

<center> <h4>RSM</h4> </center>

![](https://github.com/LiogiShiki/Stella/blob/master/Demos/RSM.png)



### Made with
* OpenGL 4.5
* GLFW3
* ASSIMP (gLTF)
* stbImage



