

#include "Win.h"
#include "Utility.h"

/**
* Author: JADA FORRESTER
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define LEVEL_WIDTH 17
#define LEVEL_HEIGHT 8

unsigned int WIN_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Win::~Win()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Win::initialise()
{
//    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
    
    m_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/Tile.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, WIN_DATA, map_texture_id, 1.0f, 4, 1);

    // Existing
    m_state.player = new Entity();
    m_state.player->set_position(glm::vec3(-10.0f, 0.0f, 0.0f));
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/kirby_win_music.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(30.0f);
}

void Win::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_state.player->deactivate();
    if (m_state.player->get_position().y < -10.0f) m_state.next_scene_id = 1;
}

void Win::render(ShaderProgram *program)
{
    //text rednering
    std::string win_text = "YOU WIN";
    GLuint font_texture_id_1 = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, font_texture_id_1, win_text, .40f, 0.0f, glm::vec3(2.0f,-3.5f,0.0f));
    m_state.map->render(program);
    m_state.player->render(program);
}

