#include "LevelB.h"
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
#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8

unsigned int LEVELB_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    3, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2
};

LevelB::~LevelB()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    m_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/Tile.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 4, 1);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.5f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/sprites.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 4;
    m_state.player->m_animation_rows   = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);
    
    // Jumping
    m_state.player->m_jumping_power = 6.5f;
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id_1 = Utility::load_texture("assets/kirby_enemy_2.png");
    GLuint enemy_texture_id_2 = Utility::load_texture("assets/kirby_enemy_3.png");

    m_state.enemies = new Entity[ENEMY_COUNT];
    
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id_1;
    m_state.enemies[0].set_position(glm::vec3(5.5f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(0.50f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[0].m_jumping_power = 0.20f;


    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].m_texture_id = enemy_texture_id_1;
    m_state.enemies[1].set_position(glm::vec3(12.5f, 0.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_speed(0.50f);
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/Gourmet Race.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(20.0f);
    m_state.jump_sfx = Mix_LoadWAV("assets/KirbyJumpSound.wav");
    m_state.died_sfx = Mix_LoadWAV("assets/kirby_death_sound.wav");

}

void LevelB::update(float delta_time)
{
    //main game functionality
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
        m_state.enemies[i].update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
        
        m_state.player->check_collision_y(&m_state.enemies[i], 1);
        m_state.player->check_collision_x(&m_state.enemies[i], 1);
        
        m_state.enemies[i].check_collision_y(m_state.player, 1);
        m_state.enemies[i].check_collision_x(m_state.player, 1);
        
        //if player hops on enemy
        if(m_state.player->m_collided_entity_bottom){
            m_state.enemies[i].Entity::deactivate();
        }
        
        if(m_state.player->m_collided_entity_top){
            m_state.damage = true;
            //m_state.player->deactivate();
        }
        
        if(m_state.player->m_collided_entity_left){
            m_state.damage = true;
            //m_state.player->deactivate();
        }
        
        if(m_state.player->m_collided_entity_right){
            m_state.damage = true;
            //m_state.player->deactivate();
        }
        
        if(m_state.enemies[i].m_collided_entity_bottom){
            m_state.damage = true;
            //m_state.player->deactivate();
        }
        
    }
    //switch levels if done
    if(m_state.player->get_position().y < -10.0f && m_state.player->get_position().x > -16.0f){
            m_state.next_scene_id++;
    }
    
}
void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for(int i =0; i < ENEMY_COUNT; i++)  if(m_state.enemies[i].get_active_status()) m_state.enemies[i].render(program);

}
