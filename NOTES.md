# OpenGl jegyzet

OpenGl maga egy state machine.

Mindig az aktuális állapot szerint renderel amit a context határoz meg.

A renderelést úgy tudjuk befolyásolni hogy a contextet módosítjuk (pl beállítunk egy változót hogy háromszögek helyett vonalakat rajzoljon)

## Objects

Részhalmaza az OpenGl state machine beállításainak

```c
struct object_name {
    float  option1;
    int    option2;
    char[] name;
};
```

```c
// create object
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH,  800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

## GLFW

Operéciós rendszer függetlenül ad funkciókat.

OpenGl próbál platform független maradni viszont van egy csomó dolog ami oprendszer függő, pl. input beolvasás, window kreálás stb.

Ezeket a glfw könyvtár elrejti.

## GLAD

A videó kártya fejlesztők különböző verziójú OpenGl-t támogatnak és ez nem derül ki compile time, ezáltal minden egyes OpenGl hívásnál le kellene ellenőrizni a támogatott verziót, majd csak ez után lehívni a function pointert.

A GLAD ebben tud segíteni.

## Shader

Egy shader egy apró program, ami a GPU-n fut.

A shadereket pipelineokba szervezzük. Ezek egymás outputjait kapják inputnak.

A shadereket GLSL (OpenGL Shading Language) nyelven írjuk.

A shaderek legyenek minél kisebbek és lehessen őket párhuzamosan futtatni.

Ábra: ![Ábra](https://learnopengl.com/img/getting-started/pipeline.png)

Renderelés menete:
 1. átadunk 3D-s pontokat a pipeline-nak (ezek a vertex-ek vagy vertices)
    vertex: információ egy 3D-s pontról. Ezek az információk lehetnek bármik, de a legfontosabb az x, y, z koordináta és a színe.
	OpenGl primitívek: ezek határozzák meg hogy mit szeretnénk kirenderelni. Lehetséges értékei: GL_POINTS, GL_TRIANGLES, GL_LINE_STRIP.
 2. A vertex shader lesz az első shader ami megkapja a pontokat. A vertex shader legfontosabb feladata hogy a bejövő adatokat 3D-s koordinátákká alakítsa. (pl.: [1, 2, 3] -> x: 1, y: 2, z: 3)
 3. (opcionális) A geometry shader a következő. Ez inputként a vertex shader kimeneti vertexeit kapja és átalakíthatja azt (pl.: egy háromszöget 2db háromszöggé alakíthat). Ezt úgy teszi hogy további vertexeket adhat a meglévőkhöz.
 4. primitive assembly stage: minden korábbi vertexet összeszed és belerak egy (vagy több) primitív alakba
 5. ez után a reszterizáció következik, ami az előző lépésben lévő primitív alakokat pixelekké fordítja. Ennek outputja egy fragment lesz, ami a fragment shader inputja. Egy fragment tartalmaz minden adatot, ami ahhoz kell hogy az OpenGl ki tudjon renderelni egy pixelt.
 6. A fragment shader legfontosabb feladata, hogy színt adjon egy-egy pixelnek.
    A fragment shader tartalmaz ehhez minden adatot (pl világítás, árnyék, világítás színe stb.)
 7. Alpha test és blending stage.
    Mindezek után az adott objektumon ellenőrzésre kerül az alpha érték és a mélység, azaz hogy melyik objektum helyezkedik el egy másik előtt. Így érhető el az ablak vagy átlátszóság de az is hogy a térben hátrébb elhelyezkedő objektumok kirenderelésre kerüljenek (pl player fegyvere, amikor belemegy a falba kell hogy látszódjon)
	
## VBO (vertex buffer object)

A GPU-n lefoglalt memória ami a vertexekről tárolja az adatot.

A vertex shader ebből olvas ki annyi adatot amennyit megmondunk neki.

## VAO (vertex array object)

A VBO-khoz hozzá lehet bindolni vertex attribútumokat.

```c
// 0. átmásoljuk a vertexeinket a VBO-ba, amit OpenGl használni fog renderelésnél
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 1. beállítjuk az attribútum pointereket (ezek a shader forráskódjában lévő változók lesznek)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);  
// 2. kiválasztjuk a shader programot amivel renderelni szeretnénk
glUseProgram(shaderProgram);
// 3. kirajzoljuk az alakot, mondjuk háromszög módban
glDrawArrays(GL_TRIANGLES, 0, 3); 
```

Ezt minden esetben meg kellene tennünk, ha csak vbo-kat szeretnénk használni, ami elég idegesítő feladat, nem beszélve arról ha a shadereknek mondjuk 4-5 attribútuma is van.

A VAO ebben hivatott segíteni. 

A VAO-hoz hozzárendelhetünk attribútum pointereket, amik innentől kezdve egy gyűjtője lehet a rendereléshez szükséges közös dolgoknak.

A VAO-hoz a VBO-t is hozzá tudjuk rendelni, ami a kirenderelendő objektumot jelenti.

A jó dolog ebben, hogy egy VAO-hoz hozzárendelhetünk egy másik VBO-t és egy VBO-hoz hozzárendelhetünk különböző VAO-kat is.

Ábra: ![Ábra](https://learnopengl.com/img/getting-started/vertex_array_objects.png)

## Indexed drawing

Az indexelt rajzolás egy olyan technika amivel optimalizálhatjuk a renderelést.

Pl.: egy négyszöget úgy rajzolhatunk ki hogy 2 háromszög vertexei segítségével kirajzolunk 2 háromszöget, aminek az eredménye egy téglalap lesz. Itt viszont a háromszögek pontjai közül 2 meg fog egyezni.

```c
float vertices[] = {
    // first triangle
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f,  0.5f, 0.0f,  // top left 
    // second triangle
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
}; 
```

Ezen segít az Element Buffer Object (EBO), aminek VBO-jában megadhatjuk a unique vertexeket, majd egy másik array-ben azt, hogy milyen sorrendben rajzolja ki az OpenGL a háromszögeket.

```c
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};  
```

![ábra](https://learnopengl.com/img/getting-started/vertex_array_objects_ebo.png)

## Wireframe mode

Hibakeresés során hasznos lehet.

```c
// draw the polygon's front and back side with a line
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// draw the polygon's front and back side and fill the shape
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
```

## GLSL

A GLSL az opengl egyedi programozási nyelve, amivel a GPU-ra tudunk programot írni.

Ezeknek a programoknak csak inputjaik és outputjaik vannak és a programok csak ezeken keresztül kommunikálhatnak.

### GLSL típusok

GLSL számos beépített típust kínál fel:

Vektorok:
  - vecn: alapértelmezett vektor N darab float értékkel
  - bvecn: vektor N darab bool értékkel
  - ivecn: vektor N darab integer értékkel
  - uvecn: nektor N darab unsigned integer értékkel
  - dvecn: vektor N darab double értékkel

A vektorok kompnenseit az x, y, z, w paramétereikkel érhetjük el, pl. vector.x

A vektorok elérésére elég fura szintaxisok is vannak:

```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
```

### Linking

A shaderek inputjait és outputjait linkeléssel kötjük össze.

Ha egy shader output változójának típusa és neve megegyezik az őt követő shader input változójának típusával és nevével, akkor ezek a linkelés során összekötésre kerülnek és az első shader outputja bekerül az őt követő shader inputjába.

Pl.:

Vertex shader:
```glsl
#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
  
out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}
```

Fragment shader:
```glsl
#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = vertexColor;
} 
```

### Uniform változók

A uniform változók egy másik módja, hogy adatot küldjünk a GPU-nak.

Hasonlítanak a vertex attribútumokhoz, annyi különbséggel, hogy a uniform változók globálisak per shader object.

```glsl
#version 330 core
out vec4 FragColor;
  
uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
    FragColor = ourColor;
}
```

Érdemes fejben tartani, hogy ha a glsl kódunkban nem hivatkozunk a uniform változóra, akkor a fordítás során a compiler ki fogja onnan törölni.

## Textúrázás

A textúrázás az, amikor egy alakra egy képet illesztünk.

Ezt úgy tesszük, hogy megadunk egy alakot (pl egy háromszög) és minden egyes vertex-ét megfeleltetjük a textúrának egy-egy korrdinátájával.

![ábra](https://learnopengl.com/img/getting-started/tex_coords.png)

Az ábrán látható háromszög 3 vertexét hozzá kell rendelni a textúra 3 pontjához. A többi fragment pixel színét az interpoláció végzi.