# The Marvelous Mandrill Maze
 A small 3D maze game created using OpenGL for *CSE 4200 - Computer Graphics*.


**Constructed with the help of tutorials from:**     
https://learnopengl.com/  
http://www.opengl-tutorial.org/  

Intended for computers that can support OpenGL 3.3 and above. This code will compile on an x86 Windows 10 workstation.

**Libraries/APIs/Headers used:**   
**OpenGL** (Rendering)  
**GLEW** (Extension loading)  
**SDL2** (Keyboard input and window logic)  
**SDL2_MIXER** (Audio)  
**FREETYPE** (Font rendering)  
**GLM** (Matrix and vector math)  
**stb_image** (Texture loading)  

Skybox generated using:
http://wwwtyro.github.io/space-3d/


Mandrill Maze inspiration:  
https://www.youtube.com/watch?v=pPB39fvq8m0&t=351s  
https://www.youtube.com/watch?v=sMc4Lcxmmso&t=105s  


## Controls and Interface

Aside from the game window, a console window is also present for debugging, but can be ignored by the user.

To win the game, the user must navigate throughout the maze and collect all monkey idols present in the game. These idols can be collected just by walking into them. 

**WASD keys:** Player/Camera movement  
**Mouse:** Camera movement  
**R key:** Restart the game and generate a new random maze  
**M key:** Mute the background music  
**P key:** Toggle cursor locking  
**SPACE key:** Jump  
**I key:** Toggle infinite jump mode  
**ESC key:** Exit out of the game at any time.  


**Players must gather all of these monkey idols to win the game (Amount needed shown in the bottom left):**
![image](https://user-images.githubusercontent.com/53513566/118372033-39710f80-b564-11eb-8d87-aed8513398c1.png)


 **After collecting all idols, the maze will disappear, and the player will be prompted to generate a new maze with the R key.**
![image](https://user-images.githubusercontent.com/53513566/118372028-35dd8880-b564-11eb-8e74-e86e9c64c02b.png)

**Aerial view of the maze**:
![image](https://user-images.githubusercontent.com/53513566/118372176-185cee80-b565-11eb-8590-7e461c965658.png)


