# CSCI 5229 Computer Graphics Summer 2021
## Final Project - Cabin in the Woods
Submitted By : Madhukar Arora


## About the Project

For the final project, I have created a three-dimensional walk through of a cabin located in a forest.  


### Source Files Included
1. CSCIx229.h
2. errcheck.c
3. fatal.c
4. loadobj.c
5. loadtexbmp.c
6. print.c
7. projection.c
8. main.c (source code - application entry point)
9. Makefile 

OS Requirements : Ubuntu 20.04 with Open GL installed and gcc C compiler.

### Compilation Instructions
1. Extract the zipped folder submitted as part of this submission.
2. Change into the new directory created on unzip containing the source file and the Makefile for automated builds.
3. Open terminal in this directory.
4. Make commands 

   ***make*** : This will generate an executable called final******.
   
   ***make clean*** : This will remove all executables and files generated as part of compilation.
5. To run the application : type in your terminal ***./final*** .

### Working with the application

***ESC*** : exit the application

***x*** or ***X*** : Turn OFF/ON axes.

***M*** or ***m*** : toggle between Orthogonal, Perspective and First-Person Mode


***l*** or ***L*** : Turn OFF/ON light.

***p*** or ***P*** : Toggle between automatic light control or manual light control.


***<*** or ***>*** : Move light source in either direction in Manual mode.


***[*** or ***]*** : Decrease/Increase the height of the light source.


***a***: Decrease ambient light levels.

***A***: Increase ambient light levels.

***d***: Decrease diffuse levels.

***D***: Increase diffuse levels.

***v***: Decrease specular levels.

***V***: Increase specular levels.

***e***: Decrease emission levels.

***E***: Increase Emission levels.

***b***: Decrease shininess levels.

***B***: Increase shininess levels.

***UP Arrow*** or ***DOWN Arrow*** : Look down or up using these keys in Orthogonal/Perspective Mode. Move forward or backward in First Person View.

***LEFT Arrow*** or ***RIGHT Arrow*** : Look left or right using these keys. 

***PAGE UP*** or ***PAGE DOWN*** : Increase/Decrease the dimension of the orthogonal view box.

***F1***: Toggle between Smooth Mode.

***F2***: Toggle between Local Mode.

***F3***: Change light distance.

***F8***: Toggle ball increment.

***+*** : Increase the Field of View in Orthogonal/Perspective Mode  or Increase the window dimension in First Person Mode.

***-*** : Decrease the Field of View in Orthogonal/Perspective Mode  or Decrease the window dimension in First Person Mode.




### Highlights 

1. The project has been implemented using OpenGl and written entirely in C.

2. There is a cabin placed between a forest which one can go inside. The inside includes a floor bed, chair and a ceiling lamp to give a rustic look inside.

3. The scene includes a bench that gives views overlooking the lake and the mountains.

4. The two sides of the lake are connected using a bridge and serves as another view point of the entire scenery. I've also added effects to make the water in the lake look more realistic.


### Learnings from the course

Coming from a background with no experience in Computer Graphics, I've learnt a lot in the past four weeks from the course. I've developed a thought process of how to create basic shapes, how to apply lighting and textures to them and how to create scenes using these. 


