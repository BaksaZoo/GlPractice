# OpenGl jegyzet

OpenGl maga egy state machine.
Mindig az aktuális állapot szerint renderel amit a context határoz meg.
A renderelést úgy tudjuk befolyásolni hogy a contextet módosítjuk (pl beállítunk egy változót hogy háromszögek helyett vonalakat rajzoljon)

## Objects
Részhalmaza az OpenGl state machine beállításainak

struct object_name {
    float  option1;
    int    option2;
    char[] name;
};

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
Ábra: https://learnopengl.com/img/getting-started/pipeline.png
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