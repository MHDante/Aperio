# <img src="https://raw2.github.com/eternallite/MeshIllustrator/superquadrics/about_orig.png" valign="bottom" />Aperio  

Aperio is a 3D anatomical or other mesh browser allowing slidable superquadrics to be stretched 
and placed onto surfaces as a way of marking up areas to be illustratively manipulated 
(transformed or deformed in a manner for the purpose of illustration and understanding). The marked areas can 
then be cut, peeled, split, deformed or act as hinges or constraints for exploders etc. Additionally, elements can 
act as handles on meshes for moving them out of the way; parts can also be transformed in a free-form fashion, 
constrained like beads on a string, or exploded along a constrained axis, etc. The goal is to develop an 
intuitive and immediate exploration system using slidable elements (in the shape of superquadrics) that 
lends itself to visual affordance (i.e. users can easily and intuitively combine the elements such that 
they can perform various actions). It is primarily created for users who have little to no prior 3D modeling 
experience and just want to break apart a complicated system for the goal of better understanding relationships 
between parts in the 3D system. It can be used by medical professionals on anatomical data or even be extended 
for use as an immediate rigging tool on other polygonal meshes.

These pages document the use of the application.



#### Important Links (dependencies)

Written using C++11 (Requires Visual Studio 2013 or higher) <br />
Visualization Toolkit 6.1.0 (algorithms and rendering), [http://www.vtk.org/](http://www.vtk.org/) <br />
Qt 5.3.0 (GUI), [http://qt-project.org/](http://qt-project.org/) <br />
Carve CSG (Latest) (Boolean operations), [http://carve-csg.com/](http://carve-csg.com/) <br />
Graphics card with GLSL shader support (OpenGL 2.1 or higher)
