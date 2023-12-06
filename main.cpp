#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Win.h"
#include "Lose.h"
#include "Effects.h"
#include "LevelX.h"


/**
* Author: JADA FORRESTER
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH  = 800,
          WINDOW_HEIGHT = 580;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//greyscale
const float MILLISECONDS_IN_SECOND = 1000.0;


// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;

LevelX *g_levelX;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Lose *g_lose;
Win *g_win;



Effects *g_effects;
Scene   *g_levels[6];
Entity *background;
//lives
int g_lives;

int g_enemy_lives;


SDL_Window* g_display_window;
bool g_game_is_paused = false;
bool g_game_is_running = true;
GLuint g_greyscale_flag = 0;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Kirby",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
  
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    //grey
    g_shader_program.set_greyscale_flag(g_greyscale_flag);


    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    //lives
    g_lives = 3;
    g_enemy_lives = 3;
    
    //BACK GROUDN
    background = new Entity();
    background->set_entity_type(BACKGROUND);
    

    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //levels
    g_levelX = new LevelX();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_win = new Win();
    g_lose = new Lose();

    g_levels[0] = g_levelX;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_lose;
    g_levels[5] = g_win;

    
    // Start at level X
    switch_to_scene(g_levels[0]);
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(SHRINK, 2.5f);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_game_is_running = false;
                        break;
                        
                  
                    case SDLK_SPACE:
                        // Jump
                        if (g_current_scene->m_state.player->m_collided_bottom)
                        {
                            g_current_scene->m_state.player->m_is_jumping = true;
                            Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                        }
                        break;
                        
                    case SDLK_p:
                        
                        g_greyscale_flag = g_greyscale_flag == 0 ? 1 : 0;
                        g_game_is_paused = !g_game_is_paused;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
    //start
    else if(key_state[SDL_SCANCODE_K]){
        switch_to_scene(g_levels[1]);
    }

    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

void update()
{
    if (g_game_is_paused) return;

    // Get the current ticks
    float current_ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;

    // Calculate the time passed since the last update, including the time during pause
    float delta_time = current_ticks - g_previous_ticks;
    g_previous_ticks = current_ticks;

    // Accumulate the delta_time
    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);

        if (g_is_colliding_bottom == false && g_current_scene->m_state.player->m_collided_bottom) g_effects->start(SHAKE, 1.0f);

        g_is_colliding_bottom = g_current_scene->m_state.player->m_collided_bottom;

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);

    if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }

    // level 1 -2
    if (g_current_scene == g_levelA && g_current_scene->m_state.player->get_position().y < -10.0f && g_current_scene->m_state.player->get_position().x > 18.0f && g_lives >= 1){
        switch_to_scene(g_levelB);
    }
    // restart if fall throguh platforms level 1
    if (g_current_scene == g_levelA &&  g_current_scene->m_state.player->get_position().y < -10.0f && !(g_current_scene->m_state.player->get_position().x > 18.0f) && g_lives >= 1){
        switch_to_scene(g_current_scene);
        g_lives -= 1;
    }

    // level 2-3
    if (g_current_scene == g_levelB && g_current_scene->m_state.player->get_position().y < -10.0f && g_current_scene->m_state.player->get_position().x > 16.0f && g_lives >= 1){
        switch_to_scene(g_levelC);
    }
    // level restart if fall
    if (g_current_scene == g_levelB &&  g_current_scene->m_state.player->get_position().y < -10.0f && !(g_current_scene->m_state.player->get_position().x > 16.0f) && g_lives >= 1){
        switch_to_scene(g_current_scene);
        g_lives -= 1;
    }
    
    //win
    if(g_lives >= 1 && g_current_scene == g_levelC && g_current_scene->m_state.player->get_position().y < -10.0f && g_current_scene->m_state.player->get_position().x > 18.0f && g_enemy_lives == 0){
        switch_to_scene(g_win);
    }
    if(g_lives >= 1 && g_current_scene == g_levelC && g_current_scene->m_state.player->get_position().y < -10.0f && g_current_scene->m_state.player->get_position().x > 18.0f && g_enemy_lives != 0){
        g_lives -= 1;
        switch_to_scene(g_current_scene);
    }
    
    
    // taking damage from eneny
    if(g_current_scene->m_state.damage){
        g_current_scene->m_state.damage = false;
        g_lives -= 1;
        switch_to_scene(g_current_scene);
    }
    // taking damage from eneny
    if(g_current_scene->m_state.enemy_damage && g_current_scene == g_levelC && g_enemy_lives >= 1){
        g_enemy_lives -= 1;
        g_current_scene->m_state.enemy_damage = false;
        switch_to_scene(g_current_scene);
    }
    
    
    //no more lives
    if(g_lives == 0){
        Mix_PlayChannel(-1, g_current_scene->m_state.died_sfx, 0);
        switch_to_scene(g_lose);
    }

    g_view_matrix = glm::translate(g_view_matrix, g_effects->m_view_offset);
}




void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);


    glClear(GL_COLOR_BUFFER_BIT);

glUseProgram(g_shader_program.get_program_id());
    
    glm::mat4 invertedViewMatrix = glm::inverse(g_view_matrix);
    g_shader_program.set_model_matrix(invertedViewMatrix * background->get_m_model_matrix());
    
    float halfWidth = 10 / 2.0f;
    float halfHeight = 7.5 / 2.0f;
    
    float vertices[] = {
        -halfWidth, -halfHeight,  // bottom-left
        halfWidth, -halfHeight,   // bottom-right
        halfWidth, halfHeight,    // top-right
        -halfWidth, -halfHeight,  // bottom-left
        halfWidth, halfHeight,    // top-right
        -halfWidth, halfHeight    // top-left
    };
    
    float tex_coords[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0
    };
    
    
    glBindTexture(GL_TEXTURE_2D, background->m_texture_id);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    //main menu
    if (g_current_scene == g_levelX){
        background->m_texture_id = Utility::load_texture("assets/KMA Plains.png");
    }
    //LEVEL 1
    if(g_current_scene == g_levelA){
        background->m_texture_id = Utility::load_texture("assets/KMA Plains.png");
    }
    //LEVEL 2
    if(g_current_scene == g_levelB){
        background->m_texture_id = Utility::load_texture("assets/kirby_level2.png");
    }
    // LEVEL 3
    if(g_current_scene == g_levelC){
        background->m_texture_id = Utility::load_texture("assets/kirby_level3.png");
        
        std::string enemy_lives = std::to_string(g_enemy_lives);
        GLuint font_texture_id_2 = Utility::load_texture("assets/font1.png");

        Utility::draw_text(&g_shader_program, font_texture_id_2, enemy_lives, .40f, -2.5f, glm::vec3(g_current_scene->m_state.enemies[0].get_position().x, g_current_scene->m_state.enemies[0].get_position().y +0.5,0.0f));
        
    }
    //win
    if(g_current_scene == g_win){
        background->m_texture_id = Utility::load_texture("assets/KMA Plains.png");
       
    }
    // lose
    if(g_current_scene == g_lose){
        background->m_texture_id = Utility::load_texture("assets/kirby_level3.png");
       
    }

    //lives
    std::string lives = std::to_string(g_lives);
    GLuint font_texture_id_1 = Utility::load_texture("assets/font1.png");
    Utility::draw_text(&g_shader_program, font_texture_id_1, lives, .40f, -2.5f, glm::vec3(g_current_scene->m_state.player->get_position().x, g_current_scene->m_state.player->get_position().y + 0.5,0.0f));
   
    
    g_current_scene->render(&g_shader_program);
    g_effects->render();
    SDL_GL_SwapWindow(g_display_window);
    //grey
    g_shader_program.set_greyscale_flag(g_greyscale_flag);

   
}

void shutdown()
{
    SDL_Quit();
    delete g_levelX;
    delete g_levelA;
    delete g_levelB;
    delete g_effects;

}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
            
            update();
            if (g_current_scene->m_state.next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);
            render();
        
    }
    shutdown();
    return 0;
}
