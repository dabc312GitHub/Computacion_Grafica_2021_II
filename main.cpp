#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <queue>
#include <vector>

#include "lib/stb_image.h"
#include "lib/shader.h"
#include "lib/solver.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, bool flagCambioCamara);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


void cargarTexturas(GLuint& textureID, std::string& texturePath)
{
    // generamos size cantidad ed texturas
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        exit(1); // force exit
    }
    stbi_image_free(data); // free image data
}


// usa 26 shaders para cada cubo
static GLfloat vertices[] =
        {
                // extra black color for drawing borders
                // UP face - white color
                -0.5f,  0.5f, -0.5f, 0.6f, 1.0f, // 0 - 0
                0.5f,  0.5f, -0.5f,  0.8f, 1.0f,// 1 - 1
                -0.5f,  0.5f,  0.5f, 0.6f, 0.0f,// 2 - 2
                0.5f,  0.5f,  0.5f,  0.8f, 0.0f,// 3 - 3

                // LEFT face - orange color
                -0.5f,  0.5f, -0.5f, 0.501f, 1.0f,// 4 - 0
                -0.5f,  0.5f,  0.5f, 0.668f, 1.0f,// 5 - 2
                -0.5f, -0.5f, -0.5f, 0.501f, 0.0f,// 6 - 4
                -0.5f, -0.5f,  0.5f, 0.668f, 0.0f,// 7 - 5

                // FRONT face - green color
                -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,// 8 - 2
                0.5f,  0.5f,  0.5f,  0.157f, 1.0f,// 9 - 3
                -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,// 10 - 5
                0.5f, -0.5f,  0.5f,  0.157f, 0.0f,// 11 - 6

                // RIGHT face - red color
                0.5f,  0.5f,  0.5f,  0.167f, 1.0f,// 12 - 3
                0.5f,  0.5f, -0.5f,  0.334f, 1.0f,// 13 - 1
                0.5f, -0.5f,  0.5f,  0.167f, 0.0f,// 14 - 6
                0.5f, -0.5f, -0.5f,  0.334f, 0.0f,// 15 - 7

                // BACK face - blue color
                -0.5f,  0.5f, -0.5f, 0.501f, 1.0f,// 16 - 0
                0.5f,  0.5f, -0.5f,  0.334f, 1.0f,// 17 - 1
                -0.5f, -0.5f, -0.5f, 0.501f, 0.0f,// 18 - 4
                0.5f, -0.5f, -0.5f,  0.334f, 0.0f,// 19 - 7

                // DOWN face - yellow color
                -0.5f, -0.5f, -0.5f, 0.8f, 1.0f,// 20 - 4
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f,// 21 - 7
                -0.5f, -0.5f,  0.5f, 0.8f, 0.0f,// 22 - 5
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f // 23 - 6
        };


static GLuint indices[] =
        {
                // UP face
                // Triangles
                0, 1, 3,
                0, 2, 3,
                // offset: 6

                // LEFT face
                // Triangles
                4, 5, 6,
                5, 6, 7,
                // offset: 12

                // FRONT face
                // Triangles
                8, 9, 11,
                8, 10, 11,
                // offset: 18

                // RIGHT face
                // Triangles
                12, 13, 15,
                12, 14, 15,
                // offset: 24

                // BACK face
                // Triangles
                16, 17, 18,
                17, 19, 18,
                // offset: 30

                // DOWN face
                // Triangles
                20, 21, 22,
                21, 23, 22,
                // offset: 36

                // LineLoop
                // UP face
                0, 1, 3, 2,
                // offset: 40

                // LEFT face
                4, 6, 7, 5,
                // offset: 44

                // FRONT face
                8, 9, 11, 10,
                // offset: 48

                // RIGHT face
                12, 13, 15, 14,
                // offset: 52

                // BACK face
                16, 17, 19, 18,
                // offset: 56

                // DOWN face
                20, 21, 23, 22
                // offset: 60

        };



class CuboUnidad
{
public:
    static const int NFACES = 6;
    char colors[NFACES];
    void chooseColor(Shader& program, GLint i){
        float timeValue = glfwGetTime();
        float cmabioRango = (sin(timeValue)+cos(timeValue) / 2.0f) + 0.5f;

        int vertexColorLocation = glGetUniformLocation(program.ID, "ourColorGlobal");
        program.use();
        switch (colors[i])
        {
            case 'W': // white color
                glUniform4f(vertexColorLocation,
                            1.0f*cmabioRango,
                            1.0f*cmabioRango,
                            1.0f*cmabioRango, 1.0f);
                break;
            case 'O': // orange color
                glUniform4f(vertexColorLocation,
                            0.972f*cmabioRango,
                            0.470f*cmabioRango,
                            0.227f*cmabioRango, 1.0f);
                break;
            case 'G': // green color
                glUniform4f(vertexColorLocation,
                            0.0f,
                            1.0f*cmabioRango,
                            0.0f, 1.0f);
                break;
            case 'R': // red color
                glUniform4f(vertexColorLocation,
                            1.0f*cmabioRango,
                            0.0f,
                            0.0f, 1.0f);
                break;
            case 'B': // blue color
                glUniform4f(vertexColorLocation,
                            0.0f,
                            0.0f,
                            1.0f*cmabioRango, 1.0f);
                break;
            case 'Y': // yellow color
                glUniform4f(vertexColorLocation,
                            1.0f*cmabioRango,
                            1.0f*cmabioRango,
                            0.0f, 1.0f);
                break;
            default: // black color by default
                program.setBool("fill", false); // no hay fill
                program.setVec3("changingColor", 0.0f, 0.0f, 0.0f);
                break;
        }
    }
    glm::vec3 pos;
    glm::mat4 model;
    CuboUnidad(){
        model = glm::mat4(1.0f);
        pos = glm::vec3(0.0f); // at the origin
        colors[0] = 'W'; // UP - White
        colors[1] = 'O'; // LEFT - Orange
        colors[2] = 'G'; // FRONT - Green
        colors[3] = 'R'; // RIGHT - Red
        colors[4] = 'B'; // BACK - Blue
        colors[5] = 'Y'; // DOWN - Yellow
    }
//    CuboUnidad(const CuboUnidad& another_cube){
//        pos = another_cube.pos;
//        model = another_cube.model;
//        colors[0] = 'W'; // UP - White
//        colors[1] = 'O'; // LEFT - Orange
//        colors[2] = 'G'; // FRONT - Green
//        colors[3] = 'R'; // RIGHT - Red
//        colors[4] = 'B'; // BACK - Blue
//        colors[5] = 'Y'; // DOWN - Yellow
//    }
    CuboUnidad(glm::mat4 model_, glm::vec3 pos_){
        model = model_;
        pos = pos_;
        colors[0] = 'W'; // UP - White
        colors[1] = 'O'; // LEFT - Orange
        colors[2] = 'G'; // FRONT - Green
        colors[3] = 'R'; // RIGHT - Red
        colors[4] = 'B'; // BACK - Blue
        colors[5] = 'Y'; // DOWN - Yellow
    }
    void setColor(GLint i, char c){
        colors[i] = c;
    }
    void draw(Shader& program){
        // ahora pintaremos cada cara del cubo con su respectivo color dependiendo del �ndice
        program.setBool("fill", true);

        for (GLint i = 0; i < NFACES; ++i)
        {
            switch (i)
            {
                case 0:// up face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
                    break;
                case 1:// left face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));
                    break;
                case 2:// front face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));
                    break;
                case 3:// right face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));
                    break;
                case 4:// back face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));
                    break;
                case 5:// down face
                    chooseColor(program, i);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));
                    break;
            }
        }

        // luego dibujamos las l�neas
        program.setBool("fill", false);
        program.setVec3("changingColor", 0.0f, 0.0f, 0.0f);
        // UP face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(36 * sizeof(GLuint)));
        // LEFT face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(40 * sizeof(GLuint)));
        // FRONT face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(44 * sizeof(GLuint)));
        // RIGHT face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(48 * sizeof(GLuint)));
        // BACK face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(52 * sizeof(GLuint)));
        // DOWN face
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(56 * sizeof(GLuint)));
    }
};
// END CUBE

class CuboCompleto
{
public:
    enum class STATE_ANIMATION { NONE, F, f, U, u, L, l, R, r, B, b, D, d, SOLVE };
    Shader program;
    static const int NroCubos = 27;
    std::string texturePath;
    GLuint textureID;

    GLuint RVAO; // associated VAO
    CuboUnidad cubes[NroCubos]; // 9(cuadrante) * 3(nivel) = 27 cubos en total
    STATE_ANIMATION state_animation = STATE_ANIMATION::NONE;

    std::string cubeString;
    std::stringstream ssCubeString;
    solver::Rubik rubikSolver;
    std::vector<char> solution;
    std::queue<STATE_ANIMATION> solutionStates;

    void HandleRubikMoves(char movement){
        static bool was_last_move = false;
        // un booleano para determinar si un movimiento fu� el �ltimo de la solucion del stack
        if (solutionStates.empty() && !was_last_move) {
            switch (movement)
            {
                case 'U':
                    rubikSolver.U(solution);
                    break;
                case 'u':
                    rubikSolver.u(solution);
                    break;
                case 'L':
                    rubikSolver.L(solution);
                    break;
                case 'l':
                    rubikSolver.l(solution);
                    break;
                case 'F':
                    rubikSolver.F(solution);
                    break;
                case 'f':
                    rubikSolver.f(solution);
                    break;
                case 'R':
                    rubikSolver.R(solution);
                    break;
                case 'r':
                    rubikSolver.r(solution);
                    break;
                case 'B':
                    rubikSolver.B(solution);
                    break;
                case 'b':
                    rubikSolver.b(solution);
                    break;
                case 'D':
                    rubikSolver.D(solution);
                    break;
                case 'd':
                    rubikSolver.d(solution);
                    break;
            }
        }
        if (was_last_move) was_last_move = false;
        if (!solutionStates.empty()) {
            solutionStates.pop();
            if (solutionStates.empty()) was_last_move = true;
        }
    }
    CuboCompleto(){}
    // dos listas de strings, vertex y fragment shaders para cada cubo con su respectivo �ndice
    CuboCompleto(
            const char* vertexPath,
            const char* fragmentPath,
            const char* texPath
    ){
        // no estamos resolviendo nada al inicio
        RVAO = 0; // valor por defecto del VAO
        program.ConfShaders(vertexPath, fragmentPath);

        // seting the Textures paths
        texturePath = texPath;
        // cargando las texturas y configurando las unit textures
        cargarTexturas(textureID, texturePath);
//        program.setInt("materialTexture", 0);


        // primer nivel
        cubes[0].pos = glm::vec3(-1, 1, -1);
        cubes[1].pos = glm::vec3(0, 1, -1);
        cubes[2].pos = glm::vec3(1, 1, -1);
        cubes[3].pos = glm::vec3(-1, 1, 0);
        cubes[4].pos = glm::vec3(0, 1, 0);
        cubes[5].pos = glm::vec3(1, 1, 0);
        cubes[6].pos = glm::vec3(-1, 1, 1);
        cubes[7].pos = glm::vec3(0, 1, 1);
        cubes[8].pos = glm::vec3(1, 1, 1);
        // segundo nivel
        cubes[9].pos = glm::vec3(-1, 0, -1);
        cubes[10].pos = glm::vec3(0, 0, -1);
        cubes[11].pos = glm::vec3(1, 0, -1);
        cubes[12].pos = glm::vec3(-1, 0, 0);
        // cubes[13].model = glm::vec3(0, 0, 0); omitimos el 13, porque es el centro y no lo necesitamos
        cubes[14].pos = glm::vec3(1, 0, 0);
        cubes[15].pos = glm::vec3(-1, 0, 1);
        cubes[16].pos = glm::vec3(0, 0, 1);
        cubes[17].pos = glm::vec3(1, 0, 1);
        // tercer nivel
        cubes[18].pos = glm::vec3(-1, -1, -1);
        cubes[19].pos = glm::vec3(0, -1, -1);
        cubes[20].pos = glm::vec3(1, -1, -1);
        cubes[21].pos = glm::vec3(-1, -1, 0);
        cubes[22].pos = glm::vec3(0, -1, 0);
        cubes[23].pos = glm::vec3(1, -1, 0);
        cubes[24].pos = glm::vec3(-1, -1, 1);
        cubes[25].pos = glm::vec3(0, -1, 1);
        cubes[26].pos = glm::vec3(1, -1, 1);

    }

    void ApplyTransformation(glm::mat4 glob_trans){
        // aplicando la transformaci�n global a todos los cubos
        for (int i = 0; i < NroCubos; ++i)
            cubes[i].model = cubes[i].model * glob_trans;
    }
    void DrawCube(glm::mat4& view, glm::mat4& projection){
        // render cube
        glBindVertexArray(RVAO);
        //// activando la textura con su respectiva unidad de textura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        for (int i = 0; i < NroCubos; ++i)
        {
            if (i == 13) continue; // no pintaremos el cubo porque es el interior
            // al principio debemos trasaladar cada cubo a su posici�n
            // pero luego debemos quitar tal traslaci�n para que no desaparezcan
            // en el infinito tales cubos
            cubes[i].model = glm::translate(cubes[i].model, cubes[i].pos);
            program.use();
            program.setMat4("projection", projection);
            program.setMat4("view", view);
            program.setMat4("model", cubes[i].model);

            cubes[i].draw(program);

            // aqu� quitamos la traslaci�n del cubo
            cubes[i].model = glm::translate(cubes[i].model, -cubes[i].pos);
        }
    }
    void HandleDrawing(
            glm::mat4& view,
            glm::mat4& projection,
            STATE_ANIMATION& some_state
    ){
        switch (some_state)
        {
            case STATE_ANIMATION::F:
            {
                // EL ERROR SE ENCUENTRA EN EL �LTIMO MOVIMIENTO EJECUTADO POR EL SOLVER
                if (F()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('F');
                }
                break;
            }
            case STATE_ANIMATION::f:
            {
                if (f()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('f');
                }
                break;
            }
            case STATE_ANIMATION::R:
            {
                if (R()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('R');
                }
                break;
            }
            case STATE_ANIMATION::r:
            {
                if (r()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('r');
                }
                break;
            }
            case STATE_ANIMATION::U:
            {
                if (U()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('U');
                }
                break;
            }
            case STATE_ANIMATION::u:
            {
                if (u()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('u');
                }
                break;
            }
            case STATE_ANIMATION::B:
            {
                if (B()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('B');
                }
                break;
            }
            case STATE_ANIMATION::b:
            {
                if (b()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('b');
                }
                break;
            }
            case STATE_ANIMATION::L:
            {
                if (L()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('L');
                }
                break;
            }
            case STATE_ANIMATION::l:
            {
                if (l()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('l');
                }
                break;
            }
            case STATE_ANIMATION::D:
            {
                if (D()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('D');
                }
                break;
            }
            case STATE_ANIMATION::d:
            {
                if (d()) {
                    some_state = solutionStates.empty() ? STATE_ANIMATION::NONE : solutionStates.front();
                    HandleRubikMoves('d');
                }
                break;
            }
        }
        DrawCube(view, projection);
    }

    void Solve(STATE_ANIMATION& some_state){
//        std::stringstream ss;
//        ss << rubikSolver;
//        std::cout << "CuboUnidad form to solve: \n";
//        std::cout << ss.str() << "\n";
        rubikSolver.solve(solution);
//        std::cout << "Solution: \n";
        for (int movement = 0; movement < solution.size(); ++movement) {
//            std::cout << solution[movement] << " ";
            if (solution[movement] == 'F')
                solutionStates.push(STATE_ANIMATION::F);
            else if (solution[movement] == 'f')
                solutionStates.push(STATE_ANIMATION::f);
            else if (solution[movement] == 'W')
                solutionStates.push(STATE_ANIMATION::B);
            else if (solution[movement] == 'w')
                solutionStates.push(STATE_ANIMATION::b);
            else if (solution[movement] == 'R')
                solutionStates.push(STATE_ANIMATION::R);
            else if (solution[movement] == 'r')
                solutionStates.push(STATE_ANIMATION::r);
            else if (solution[movement] == 'L')
                solutionStates.push(STATE_ANIMATION::L);
            else if (solution[movement] == 'l')
                solutionStates.push(STATE_ANIMATION::l);
            else if (solution[movement] == 'U')
                solutionStates.push(STATE_ANIMATION::U);
            else if (solution[movement] == 'u')
                solutionStates.push(STATE_ANIMATION::u);
            else if (solution[movement] == 'D')
                solutionStates.push(STATE_ANIMATION::D);
            else if (solution[movement] == 'd')
                solutionStates.push(STATE_ANIMATION::d);
        }
//        std::cout << "\n\n";
        solution.clear();
        // una vez contruida la soluci�n, se pone el primer estado
        some_state = solutionStates.front();
        solutionStates.pop(); // un pop porque ya consumimos un movimiento

    }
    void AssociateVAO(GLuint VAO){
        RVAO = VAO;
    }


    //// rota el FRONT del cubo en sentido horario
    bool F(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::mat4 I4(1.0f);
        cubes[6].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[6].model;
        cubes[7].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[7].model;
        cubes[8].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[8].model;
        cubes[15].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[15].model;
        cubes[16].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[16].model;
        cubes[17].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[17].model;
        cubes[24].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[24].model;
        cubes[25].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[25].model;
        cubes[26].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[26].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            // ahora debemos actualizar las coordenadas y modelos de los cubos que cambian por la rotaci�n
            CuboUnidad tmp_6 = cubes[6], tmp_7 = cubes[7], tmp_8 = cubes[8];
            CuboUnidad tmp_15 = cubes[15], tmp_17 = cubes[17];
            CuboUnidad tmp_24 = cubes[24], tmp_25 = cubes[25], tmp_26 = cubes[26];
            cubes[6] = tmp_24, cubes[7] = tmp_15, cubes[8] = tmp_6;
            cubes[15] = tmp_25, cubes[17] = tmp_7;
            cubes[24] = tmp_26, cubes[25] = tmp_17, cubes[26] = tmp_8;
            return true;
        }
        return false;
    }
    //// rota el FRONT del cubo en sentido antihorario
    bool f(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::mat4 I4(1.0f);
        cubes[6].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[6].model;
        cubes[7].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[7].model;
        cubes[8].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[8].model;
        cubes[15].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[15].model;
        cubes[16].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[16].model;
        cubes[17].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[17].model;
        cubes[24].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[24].model;
        cubes[25].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[25].model;
        cubes[26].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[26].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            // ahora debemos actualizar las coordenadas y modelos de los cubos que cambian por la rotaci�n
            CuboUnidad tmp_6 = cubes[6], tmp_7 = cubes[7], tmp_8 = cubes[8];
            CuboUnidad tmp_15 = cubes[15], tmp_17 = cubes[17];
            CuboUnidad tmp_24 = cubes[24], tmp_25 = cubes[25], tmp_26 = cubes[26];
            cubes[6] = tmp_8, cubes[7] = tmp_17, cubes[8] = tmp_26;
            cubes[15] = tmp_7, cubes[17] = tmp_25;
            cubes[24] = tmp_6, cubes[25] = tmp_15, cubes[26] = tmp_24;
            return true;
        }
        return false;
    }
    //// rota el RIGHT del cubo en sentido horario
    bool R(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::mat4 I4(1.0f);
        cubes[8].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[8].model;
        cubes[5].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[5].model;
        cubes[2].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[2].model;
        cubes[17].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[17].model;
        cubes[14].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[14].model;
        cubes[11].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[11].model;
        cubes[26].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[26].model;
        cubes[23].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[23].model;
        cubes[20].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[20].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_8 = cubes[8], tmp_5 = cubes[5], tmp_2 = cubes[2];
            CuboUnidad tmp_17 = cubes[17], tmp_11 = cubes[11];
            CuboUnidad tmp_26 = cubes[26], tmp_23 = cubes[23], tmp_20 = cubes[20];
            cubes[8] = tmp_26, cubes[5] = tmp_17, cubes[2] = tmp_8;
            cubes[17] = tmp_23, cubes[11] = tmp_5;
            cubes[26] = tmp_20, cubes[23] = tmp_11, cubes[20] = tmp_2;
            return true;
        }
        return false;
    }
    //// rota el RIGHT del cubo en sentido antihorario
    bool r(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::mat4 I4(1.0f);
        cubes[8].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[8].model;
        cubes[5].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[5].model;
        cubes[2].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[2].model;
        cubes[17].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[17].model;
        cubes[14].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[14].model;
        cubes[11].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[11].model;
        cubes[26].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[26].model;
        cubes[23].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[23].model;
        cubes[20].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[20].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_8 = cubes[8], tmp_5 = cubes[5], tmp_2 = cubes[2];
            CuboUnidad tmp_17 = cubes[17], tmp_11 = cubes[11];
            CuboUnidad tmp_26 = cubes[26], tmp_23 = cubes[23], tmp_20 = cubes[20];
            cubes[8] = tmp_2, cubes[5] = tmp_11, cubes[2] = tmp_20;
            cubes[17] = tmp_5, cubes[11] = tmp_23;
            cubes[26] = tmp_8, cubes[23] = tmp_17, cubes[20] = tmp_26;
            return true;
        }
        return false;
    }
    //// rota el UP del cubo en sentido horario
    bool U(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 I4(1.0f);
        cubes[0].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[0].model;
        cubes[1].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[1].model;
        cubes[2].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[2].model;
        cubes[3].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[3].model;
        cubes[4].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[4].model;
        cubes[5].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[5].model;
        cubes[6].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[6].model;
        cubes[7].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[7].model;
        cubes[8].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[8].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_0 = cubes[0], tmp_1 = cubes[1], tmp_2 = cubes[2];
            CuboUnidad tmp_3 = cubes[3], tmp_5 = cubes[5];
            CuboUnidad tmp_6 = cubes[6], tmp_7 = cubes[7], tmp_8 = cubes[8];
            cubes[0] = tmp_6, cubes[1] = tmp_3, cubes[2] = tmp_0;
            cubes[3] = tmp_7, cubes[5] = tmp_1;
            cubes[6] = tmp_8, cubes[7] = tmp_5, cubes[8] = tmp_2;
            return true;
        }
        return false;
    }
    //// rota el UP del cubo en sentido antihorario
    bool u(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 I4(1.0f);
        cubes[0].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[0].model;
        cubes[1].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[1].model;
        cubes[2].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[2].model;
        cubes[3].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[3].model;
        cubes[4].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[4].model;
        cubes[5].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[5].model;
        cubes[6].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[6].model;
        cubes[7].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[7].model;
        cubes[8].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[8].model;
        if (angle >= 90) {
            angle = 0.0f;
            CuboUnidad tmp_0 = cubes[0];
            CuboUnidad tmp_1 = cubes[1];
            CuboUnidad tmp_2 = cubes[2];
            CuboUnidad tmp_3 = cubes[3];
            CuboUnidad tmp_5 = cubes[5];
            CuboUnidad tmp_6 = cubes[6];
            CuboUnidad tmp_7 = cubes[7];
            CuboUnidad tmp_8 = cubes[8];
            cubes[0] = tmp_2;
            cubes[1] = tmp_5;
            cubes[2] = tmp_8;
            cubes[3] = tmp_1;
            cubes[5] = tmp_7;
            cubes[6] = tmp_0;
            cubes[7] = tmp_3;
            cubes[8] = tmp_6;
            return true;
        }
        return false;
    }
    //// rota el BACK del cubo en sentido horario
    bool B(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 z = glm::vec3(0.0f, 0.0f, -1.0f);// z negativo
        glm::mat4 I4(1.0f);
        cubes[2].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[2].model;
        cubes[1].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[1].model;
        cubes[0].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[0].model;
        cubes[11].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[11].model;
        cubes[10].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[10].model;
        cubes[9].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[9].model;
        cubes[20].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[20].model;
        cubes[19].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[19].model;
        cubes[18].model = glm::rotate(I4, glm::radians(-velocidad), z) * cubes[18].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_2 = cubes[2], tmp_1 = cubes[1], tmp_0 = cubes[0];
            CuboUnidad tmp_11 = cubes[11], tmp_9 = cubes[9];
            CuboUnidad tmp_20 = cubes[20], tmp_19 = cubes[19], tmp_18 = cubes[18];
            cubes[2] = tmp_20, cubes[1] = tmp_11, cubes[0] = tmp_2;
            cubes[11] = tmp_19, cubes[9] = tmp_1;
            cubes[20] = tmp_18, cubes[19] = tmp_9, cubes[18] = tmp_0;
            return true;
        }
        return false;
    }
    //// rota el BACK del cubo en sentido antihorario
    bool b(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 z = glm::vec3(0.0f, 0.0f, -1.0f); // z negativo
        glm::mat4 I4(1.0f);
        cubes[2].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[2].model;
        cubes[1].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[1].model;
        cubes[0].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[0].model;
        cubes[11].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[11].model;
        cubes[10].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[10].model;
        cubes[9].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[9].model;
        cubes[20].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[20].model;
        cubes[19].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[19].model;
        cubes[18].model = glm::rotate(I4, glm::radians(velocidad), z) * cubes[18].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_2 = cubes[2], tmp_1 = cubes[1], tmp_0 = cubes[0];
            CuboUnidad tmp_11 = cubes[11], tmp_9 = cubes[9];
            CuboUnidad tmp_20 = cubes[20], tmp_19 = cubes[19], tmp_18 = cubes[18];
            cubes[2] = tmp_0, cubes[1] = tmp_9, cubes[0] = tmp_18;
            cubes[11] = tmp_1, cubes[9] = tmp_19;
            cubes[20] = tmp_2, cubes[19] = tmp_11, cubes[18] = tmp_20;
            return true;
        }
        return false;
    }
    //// rota el LEFT del cubo en sentido horario
    bool L(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 x = glm::vec3(-1.0f, 0.0f, 0.0f); // negative x
        glm::mat4 I4(1.0f);
        cubes[0].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[0].model;
        cubes[3].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[3].model;
        cubes[6].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[6].model;
        cubes[9].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[9].model;
        cubes[12].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[12].model;
        cubes[15].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[15].model;
        cubes[18].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[18].model;
        cubes[21].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[21].model;
        cubes[24].model = glm::rotate(I4, glm::radians(-velocidad), x) * cubes[24].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_0 = cubes[0], tmp_3 = cubes[3], tmp_6 = cubes[6];
            CuboUnidad tmp_9 = cubes[9], tmp_15 = cubes[15];
            CuboUnidad tmp_18 = cubes[18], tmp_21 = cubes[21], tmp_24 = cubes[24];
            cubes[0] = tmp_18, cubes[3] = tmp_9, cubes[6] = tmp_0;
            cubes[9] = tmp_21, cubes[15] = tmp_3;
            cubes[18] = tmp_24, cubes[21] = tmp_15, cubes[24] = tmp_6;
            return true;
        }
        return false;
    }
    //// rota el LEFT del cubo en sentido antihorario
    bool l(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 x = glm::vec3(-1.0f, 0.0f, 0.0f); // negative x
        glm::mat4 I4(1.0f);
        cubes[0].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[0].model;
        cubes[3].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[3].model;
        cubes[6].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[6].model;
        cubes[9].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[9].model;
        cubes[12].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[12].model;
        cubes[15].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[15].model;
        cubes[18].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[18].model;
        cubes[21].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[21].model;
        cubes[24].model = glm::rotate(I4, glm::radians(velocidad), x) * cubes[24].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_0 = cubes[0], tmp_3 = cubes[3], tmp_6 = cubes[6];
            CuboUnidad tmp_9 = cubes[9], tmp_15 = cubes[15];
            CuboUnidad tmp_18 = cubes[18], tmp_21 = cubes[21], tmp_24 = cubes[24];
            cubes[0] = tmp_6, cubes[3] = tmp_15, cubes[6] = tmp_24;
            cubes[9] = tmp_3, cubes[15] = tmp_21;
            cubes[18] = tmp_0, cubes[21] = tmp_9, cubes[24] = tmp_18;
            return true;
        }
        return false;
    }
    //// rota el DOWN del cubo en sentido horario
    bool D(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 y = glm::vec3(0.0f, -1.0f, 0.0f); // y negativo
        glm::mat4 I4(1.0f);
        cubes[24].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[24].model;
        cubes[25].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[25].model;
        cubes[26].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[26].model;
        cubes[21].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[21].model;
        cubes[22].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[22].model;
        cubes[23].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[23].model;
        cubes[18].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[18].model;
        cubes[19].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[19].model;
        cubes[20].model = glm::rotate(I4, glm::radians(-velocidad), y) * cubes[20].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_24 = cubes[24], tmp_25 = cubes[25], tmp_26 = cubes[26];
            CuboUnidad tmp_21 = cubes[21], tmp_23 = cubes[23];
            CuboUnidad tmp_18 = cubes[18], tmp_19 = cubes[19], tmp_20 = cubes[20];
            cubes[24] = tmp_18, cubes[25] = tmp_21, cubes[26] = tmp_24;
            cubes[21] = tmp_19, cubes[23] = tmp_25;
            cubes[18] = tmp_20, cubes[19] = tmp_23, cubes[20] = tmp_26;
            return true;
        }
        return false;
    }
    //// rota el DOWN del cubo en sentido antihorario
    bool d(){
        static float angle = 0.0f;
        float velocidad = 0.5f;
        angle += velocidad;
        glm::vec3 y = glm::vec3(0.0f, -1.0f, 0.0f); // y negativo
        glm::mat4 I4(1.0f);
        cubes[24].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[24].model;
        cubes[25].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[25].model;
        cubes[26].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[26].model;
        cubes[21].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[21].model;
        cubes[22].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[22].model;
        cubes[23].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[23].model;
        cubes[18].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[18].model;
        cubes[19].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[19].model;
        cubes[20].model = glm::rotate(I4, glm::radians(velocidad), y) * cubes[20].model;
        if (angle >= 90)
        {
            angle = 0.0f;
            CuboUnidad tmp_24 = cubes[24], tmp_25 = cubes[25], tmp_26 = cubes[26];
            CuboUnidad tmp_21 = cubes[21], tmp_23 = cubes[23];
            CuboUnidad tmp_18 = cubes[18], tmp_19 = cubes[19], tmp_20 = cubes[20];
            cubes[24] = tmp_26, cubes[25] = tmp_23, cubes[26] = tmp_20;
            cubes[21] = tmp_25, cubes[23] = tmp_19;
            cubes[18] = tmp_24, cubes[19] = tmp_21, cubes[20] = tmp_18;
            return true;
        }
        return false;
    }
};
// END RUBIK CUBE


// rubik cube
CuboCompleto::STATE_ANIMATION animation_state = CuboCompleto::STATE_ANIMATION::NONE;
bool flagCambioCamara = 0;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rubik CuboUnidad", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Shader program("vertexShader.glsl", "fragmentShader.glsl");

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    GLuint VBO[1], VAO[1], EBO[1];
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // textures attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // wire mode color attribute

    CuboCompleto rubik(
            "shaders/vertexShader.shader",
            "shaders/fragmentShader.shader",
            "assets/CuboRubik.png"
    );
    //asociamos el VAO correspondiente
    rubik.AssociateVAO(VAO[0]);

    solver::Rubik myrubik;

    float velocidad = 0.0f;
    float radius = 10.0f;
    float camX   = 1.0f;
    float camY   = 1.0f;
    float camZ   = 1.0f;
    float dirX = 0.0f;
    float dirY = 1.0f;
    float dirZ = 0.0f;
    int aleatorio = rand()%10000;

    glLineWidth(10.0f);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window, flagCambioCamara);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

        radius = 10.0f;
        float cam1 = sin(glfwGetTime()) * radius;
        float cam2 = cos(glfwGetTime()) * radius;
//        std::cout << "al: " << glfwGetTime() << std::endl;
        if(aleatorio%13==0){
            std::cout<<"2"<<std::endl;
            camX = cam1;
            camY = 0.0f;
            camZ = cam2;
            dirX = dirZ = 0.0f;
            dirY = 1.0f;
        }
        else if(aleatorio%11==0){
            std::cout<<"3"<<std::endl;
            camX = cam1;
            camY = cam2;
            camZ = 0.0f;
            dirX = dirY = 0.0f;
            dirZ = 1.0f;
        }
        else if(aleatorio%7==0){
            std::cout<<"3"<<std::endl;
            camX = 0.0f;
            camY = cam1;
            camZ = cam2;
            dirY = dirZ = 0.0f;
            dirX = 1.0f;
        }
        else if(aleatorio%5==0){
            std::cout<<"3"<<std::endl;
            camX = cam1;
            camY = cam1;
            camZ = cam2;
            dirY = dirZ = 0.0f;
            dirX = 1.0f;
        }
        else if(aleatorio%3==0){
            std::cout<<"3"<<std::endl;
            camX = cam2;
            camY = cam1;
            camZ = cam2;
            dirX = dirY = 0.0f;
            dirZ = 1.0f;
        }
        else{
            std::cout<<"else"<<std::endl;
            camX = cam2;
            camY = cam2;
            camZ = cam1;
            dirX = dirZ = 0.0f;
            dirY = 1.0f;
        }
        // activate shader
        rubik.program.use();
        velocidad+=0.003f;
        std::cout << "vel: " << velocidad << std::endl;
        // camera/view transformation
        if (velocidad>=1.0f) {
            aleatorio = int(glfwGetTime());
            std::cout << "all:: " << aleatorio << std::endl;
            velocidad = 0.0f;
        }
        view = glm::lookAt(
                glm::vec3(camX, camY, camZ),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(dirX, dirY, dirZ)
        );



        glm::mat4 projection = glm::perspective(
                glm::radians(fov),
                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                0.1f,
                100.0f
        );

        // dibujamos el cubo de rubik
        if (animation_state == CuboCompleto::STATE_ANIMATION::SOLVE) {
            rubik.Solve(animation_state);
        }
        rubik.HandleDrawing(
                view,
                projection,
                animation_state
        );

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, bool flagCambioCamara)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        cameraPos += cameraSpeed * cameraFront;
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        cameraPos -= cameraSpeed * cameraFront;
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
//        flagCambioCamara = 1;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (animation_state == CuboCompleto::STATE_ANIMATION::NONE) {
        if (key == GLFW_KEY_F && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::F;
        if (key == GLFW_KEY_R && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::R;
        if (key == GLFW_KEY_U && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::U;
        if (key == GLFW_KEY_B && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::B;
        if (key == GLFW_KEY_L && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::L;
        if (key == GLFW_KEY_D && action == GLFW_PRESS)
            animation_state = CuboCompleto::STATE_ANIMATION::D;
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            animation_state = CuboCompleto::STATE_ANIMATION::SOLVE;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll sheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

