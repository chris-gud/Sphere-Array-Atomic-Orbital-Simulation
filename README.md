# HYDROGEN ATOM ORBITAL SIMULATOR
Atomic orbital simulator using a sphere-array discretization to depict wavefunctions in 3D. Focuses on orbital geometry, internal structure, and symmetry with numerical sampling.

BY CHRIS GUDMUNDSEN  
DEC. 2024  

## Summary

&nbsp;&nbsp;This program is meant to calculate and display the probability density functions of an electron in 
the hydrogen atom. Once the user enters their desired quantum numbers, a window is created with 3 axes
and hundreds of spheres arranged in a cube. The electron probability density at several points in 3D space is represented by the
radius of the sphere at that location. The origin of the axis represents the nucleus of the
hydrogen atom. The maximum radius for each orbital is arbitrary - it has
been manually fitted to be visually appealing and clear (this means that it is not recommended to
draw conclusions from radius size when comparing orbitals stemming from different combinations of
quantum numbers). In addition, the scale of the axes change from orbital to orbital. They have been
manually set to display the most interesting parts of each orbital clearly. The scale of the axes for
each orbital is printed to terminal for the viewer's reference. The viewer can assume
that the probability density decreases to 0 outside of the axis volume. 
 
&nbsp;&nbsp;How is this simulation different from the rest? Most depictions of the orbitals of the hydrogen
atom depict a surface called an isosurface, where points of constant probability are bridged to form
a surface However, this misplaces a lot of information; the viewer cannot visualize the probability
densities within the surface. A naive viewer may be led to believe that the probability density is thus
uniform within the surface. This is not the case. This program is meant to prove it.
  
  
## CONTROLS:   

  W : move forwards into the plane of the screen  
  S : move backwards away from the plane of the screen  
  A : move left upon the plane of the screen  
  D : move right upon the plane of the screen  
  CNTRL : down (-y)
  SPACE : up (+y)
  Mouse : click and drag to change camera angle    


## EXAMPLE IMAGES:     

Quantum numbers: n = 1, l = 0, ml = 0
<img width="950" alt="Image" src="https://github.com/user-attachments/assets/909b11bd-faba-46ad-b458-c6f81b00f0c8" />

Quantum numbers: n = 2, l = 1, ml = 1
<img width="880" height="728" alt="Image" src="https://github.com/user-attachments/assets/ac7840ce-d2d2-4c5a-939d-f0ccb4a9fcce" />

Quantum numbers: n = 3, l = 1, ml = 0
<img width="880" height="728" alt="Image" src="https://github.com/user-attachments/assets/de8e54a9-34a6-4d12-85ba-435ce8d0a77a" />

Quantum numbers: n = 3, l = 2, ml = 0
<img width="880" height="728" alt="Image" src="https://github.com/user-attachments/assets/3942e503-e934-4943-803b-1823562c68ec" />

Quantum numbers: n = 3, l = 2, ml = 1
<img width="880" height="728" alt="Image" src="https://github.com/user-attachments/assets/b4d7442f-2b7c-413c-8ec7-f66e7a34a726" />


## ACKNOWLEDGEMENTS
 
OpenGL single sphere indices and vertices generation from "OpenGL Sphere Tutorial" by Song Ho Ahn  
https://www.songho.ca/opengl/gl_sphere.html    

Texture, Camera, EBO, VAO, VBO, and shaderClass .cpp and .h (basic OpenGL 3D display and movement)
from Youtube "OpenGL Course - Create 3D and 2D Graphics With C++" by freeCodeCamp.org  
(Github VictorGordan/opengl-tutorials)  
https://www.youtube.com/watch?v=45MIykWJ-C4  
https://github.com/VictorGordan/opengl-tutorials    

Glad library     

---

<a href="mailto:chrisgudmundsen@gmail.com">Let me know if you found this interesting!</a>  

