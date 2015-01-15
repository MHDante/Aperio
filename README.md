# <img src="https://raw.githubusercontent.com/eternallite/Aperio/master/about_orig.png" valign="bottom" />Aperio  


Demo Video: [www.youtube.com/watch?v=wdhQHhCtf2M](https://www.youtube.com/watch?v=wdhQHhCtf2M)

Project site: [eternallite.github.io/Aperio](http://eternallite.github.io/Aperio)

<img src="https://raw.githubusercontent.com/eternallite/Aperio/master/screenshot_small.png" width="227px;" />
<img src="https://raw.githubusercontent.com/eternallite/Aperio/master/screenshot_small2.png" width="227px;" />
<img src="https://raw.githubusercontent.com/eternallite/Aperio/master/screenshot_small3.png" width="227px;" />

Aperio is a 3D surface mesh browser using slidable superquadric-shaped tools that can be stretched and planted onto surfaces as a way of marking up areas to be illustratively manipulated (transformed or deformed in a manner for the purpose of illustration and understanding).

The marked areas can be cut, peeled or the tools themselves can act as the explosion paths. Superquadrics are defined by both implicit and parametric equations and are easily deformable (bendable, twistable, etc.) making them ideal for creating flexible shapes and paths.

The goal is to develop an intuitive and immediate exploration system using slidable tools (in the form of superquadrics) that lends itself to visual affordance (i.e. users can easily and intuitively combine tools such that they can perform various actions). It is primarily created for users who have little to no prior 3D modeling experience and just want to break apart a complicated system for the goal of better understanding relationships between parts of the 3D system.

It can be used by medical professionals on anatomical data or for visualizing on any other geometric models. These pages document the use of the application.

## Important Links (dependencies)

* Written using C++11 (Requires Visual Studio 2013 or higher) <br />
* Visualization Toolkit 6.1.0 (algorithms and rendering), [www.vtk.org](http://www.vtk.org/) <br />
* Qt 5.3.2 (GUI), [qt-project.org](http://qt-project.org/) <br />
* Open Asset Import Library (Mesh Loading), [assimp.sourceforge.net](http://assimp.sourceforge.net/) <br />
* Carve CSG (Latest) (Boolean operations), [carve-csg.com](http://carve-csg.com/) <br />
* Graphics card with GLSL Shaders/FBOs support (OpenGL 3.2 or higher), [glew.sourceforge.net](http://glew.sourceforge.net/)
