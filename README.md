# Slider

Slider is a 3D anatomical or other mesh browser allowing slidable elements to be stretched 
and placed onto surfaces as a way of marking up areas to be transformed; the marked areas can then
be cut, peeled, split or act as hinges or constraints for exploders etc. Additionally, elements can 
act as handles on meshes for moving them out of the way; parts can be transformed in a free-form fashion, 
constrained like beads on a string, or exploded along a constrained axis, etc. The goal is to develop an 
intuitive and immediate exploration system using slidable elements (in the shape of superquadrics) that 
lends itself to visual affordance (i.e. users can easily and intuitively combine the elements such that 
they can perform various actions). It is primarily created for users who have little to no prior 3D modeling 
experience and just want to break apart a complicated system for the goal of better understanding relationships 
between parts in the 3D system. It can be used by medical professionals on anatomical data or even be extended 
for use as an immediate rigging tool on other polygonal meshes.

These pages document the use of the application.

#### Important Links (dependencies)

Written using C++11 (Requires Visual Studio 2010 or higher) <br />
OpenSceneGraph 3.2.0 (Rendering) [http://www.openscenegraph.org/] (http://www.openscenegraph.org/) <br />
Qt 4.85 (GUI), [http://qt-project.org/](http://qt-project.org/) <br />
Carve CSG 1.40 (Boolean operations), [http://carve-csg.com/](http://carve-csg.com/) <br />
Graphics card with GLSL (OpenGL 2.1 or higher)
