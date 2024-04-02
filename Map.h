#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Map
{
private:
    int m_width;
    int m_height;
    
    // Here, the level_data is the numerical "drawing" of the map
    unsigned int    *m_level_data;
    GLuint          m_texture_id;
    
    float m_tile_size;
    int   m_tile_count_x;
    int   m_tile_count_y;
    
    // Just like with rendering text, we're rendering several sprites at once
    // So we need vectors to store their respective vertices and texture coordinates
    std::vector<float> m_vertices;
    std::vector<float> m_texture_coordinates;
    
    // The boundaries of the map
    float m_left_bound, m_right_bound, m_top_bound, m_bottom_bound;
    
public:
    // Constructor
    Map(int width, int height, unsigned int *level_data, GLuint texture_id, float tile_size, int
        tile_count_x, int tile_count_y);
    
    // Methods
    void build();
    void render(ShaderProgram *program);
    bool is_solid(glm::vec3 position, float *penetration_x, float *penetration_y);
    
    // Getters
    int const get_width()  const;
    int const get_height() const;
    
    unsigned int* const get_level_data() const;
    GLuint        const get_texture_id() const;
    
    float const get_tile_size()    const;
    int   const get_tile_count_x() const;
    int   const get_tile_count_y() const;
    
    std::vector<float> const get_vertices() const;
    std::vector<float> const get_texture_coordinates() const;
    
    float const get_left_bound()   const;
    float const get_right_bound()  const;
    float const get_top_bound()    const;
    float const get_bottom_bound() const;
};


