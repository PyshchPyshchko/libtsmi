/***************************************************************************************************
    Copyright 2011 Lewis Potter

    This file is part of libtsmi.

    libtsmi is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libtsmi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libtsmi.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************************************/

#ifndef LIBTSMI_H
#define LIBTSMI_H

#include <glib.h>
#include "libtcod.h"


/*! \file libtsmi.h
    \brief Realtime roguelike engine.
    
    libtsmi is an engine for realtime, tile-based, text-graphic RPG games. It is written in C99, and
    makes use of the libtcod roguelike library, and the GLib general-purpose utility library.
     
    This is being developed in conjunction with a game I am writing in Ruby, so an API that is
    easy to use in higher level languages is a priority.
*/

/**
    Coord struct literals.
*/
#define NORTH     { 0, -1}
#define NORTHEAST { 1, -1}
#define EAST      { 1,  0}
#define SOUTHEAST { 1,  1}
#define SOUTH     { 0,  1}
#define SOUTHWEST {-1,  1}
#define WEST      {-1,  0}
#define NORTHWEST {-1, -1}

/**
    So far the only use of these is for switch statements.
 */ 
enum Direction {
    North,
    Northeast,
    East,
    Southeast,
    South,
    Southwest,
    West,
    Northwest
};

/** 
    Sets the dimensions of the screen, which is defined as where the actual gameplay takes place
    (not including sidebars). 
    
    @param screen_w 
        Width of the screen in tiles
    @param screen_h 
        Height of the screen in tiles
 */
void init_screen_globals(int screen_w, int screen_h);

/**     
    @param window_w 
        Width of the entire window in tiles.
    @param screen_h 
        Height of the entire window in tiles.
    @param title 
        Text that appears on the window bar
    @param fps 
        Frames per second, or refresh frequency in herz. Note if a number is supplied that is too
        demanding of the hardware, the program will simply do its best.        
 */
void init_game(int window_h, int window_w, char* title, int fps);

/** Checks for keypresses in realtime. */
void check_key();

/**************
    STRUCTS
**************/

/*
    These are declared in the order they need to be into compile (as many of these structs refer to
    other structs).
*/

/** 
    @struct Coord
    
    Simple x, y coordinates. mostly used for direction values at this stage. 
*/
typedef struct {
	int x;
	int y; 
}Coord;

/**   
   @struct TileSeed
   
   The "Seed" of each tile struct. It contains the state in common with every single instance of a 
   tile. It's purpose in is to avoid copying information that need only be stated once, and thus
   making the world map much larger than it should be.
*/
typedef struct{
    /** A unique integer representing the type of a tile - autogenerated by make_tile_common */
    int type;

    /** Character used to display tiles of this type */
    char sym;    
    /** Whether this tile can be walked through or not */
    bool solid;
    /** Whether this tile can be seen through or not */
    bool opaque;

    TCOD_color_t source_a;
    TCOD_color_t source_b;
    TCOD_color_t night;

    TCOD_color_t source_a_vis;
    TCOD_color_t source_b_vis;
    TCOD_color_t night_vis;

    /*
        Minimum value for the random interpolation of the a and b colours. Note that this value
        should be between 0.0 and 1.0 for predictable results - though values outside of this can
        produce some interesting colours!
    **/
    float min;
    /*
        Maximum value for the random interpolation of the a and b colours. Note that this value
        should be between 0.0 and 1.0 for predictable results - though values outside of this can
        produce some interesting colours!
    **/
    float max; 
}TileSeed;

/** 
    @struct Tile
    
    The base unit of a level - the smallest quanta of discrete roguelike gameplay.
    Note that each Tile is fundamentally tiled to its TileSeed - if a TileSeed is destroyed then the
    behaviour of all its Tiles is undefined.
*/
typedef struct {
    //these are the only fields of the struct that need be mutable. I may consider making this a
    //separate linked list to make the struct completely const
    
    /** Whether the tile is currently in the players field of vision */    
    bool visible;
    /** Whether the tile has been seen by the player before (all seen tiles are "remembered" */
    bool seen;

    /**A unique integer ID representing the type - this is autogenerated by */
    int type; 
    
    const char sym;
    const bool solid;
    const bool opaque;
    
    const TCOD_color_t day;
    const TCOD_color_t day_vis;
    
    const TCOD_color_t* night;
    const TCOD_color_t* night_vis; 
} Tile;

/**
    @struct Level
    
    A 2d array of tiles of arbitrary size. This represents a single level, whether the entire
    overworld, or a level of a dungeon.
 */ 
typedef struct {
    /** The tiles that form this level. Though a 2d structure it is stored in a flat array. */
    Tile* tiles;
    /** Width of the level, in tiles */
    int width;
    /** Height of the level, in tiles */
    int height;
}Level;

/**
    @struct Area
    
    A rectangular 2d plane on some level. Useful for filling in sections of terrain.
 */ 
typedef struct {
    Level* level;
    int start_x;
    int start_y;
    int end_x;
    int end_y;
}Area;

typedef struct {
    //These three fields should be const, but having a "creation" function forbades this:/
    char sym;
    TCOD_color_t fg;
    TCOD_color_t bg;
    
    enum Direction direction;
    
    short radius; 
    int x;
    int y;    
    
    TCOD_map_t fov;
    
    Level* current_level;
}Creature;

/***************
    ENTITIES
***************/

/*
 *  All the information the engine side needs.
 *  This data is intended to be inside the host object.
 */

/**
    @param c Creature to be moved.
    @param x How many tiles the creature will be moved horizontally.
    @param x How many tiles the creature will be moved verticallly.
    
    @return 
        Returns true if the creature was succesfully moved, and false if unsuccesful (for example
        if the creature was blocked by terrain, etc)
 */
bool creature_move(Creature* c, int x, int y);
/**
    @param c Creature to be turned.
    @param turn_left 
        Is true if the creature is to be turned left, and false if the creature is to be turned 
        right.
 */
void creature_turn(Creature* c, bool turn_left);

/** @return x coordinate of the tile the creature is currently in, WRT to the current level. */
int creature_get_x(Creature* c);
/** @return y coordinate of the tile the creature is currently in, WRT to the current level. */
int creature_get_y(Creature* c);

Creature* create_creature(char sym, int x, int y, enum Direction direction,
                          TCOD_color_t fg, TCOD_color_t bg, short radius, Level* l);

void delete_creature(Creature* c);

/****************
    TILE CODE
****************/

extern const TileSeed NULL_TILE_COMMON;

TileSeed* create_tile_common(char sym, 
                               bool solid, 
                               bool opaque, 
                               TCOD_color_t source_a,
                               TCOD_color_t source_b,
                               TCOD_color_t night,
                               TCOD_color_t source_a_vis,
                               TCOD_color_t source_b_vis,
                               TCOD_color_t night_vis,
                               float min,
                               float max); 

Tile get_tile(Level* l, guint x, guint y);
bool walkable_p(Level* l, guint x, guint y);

void one_tile_fill(Area* a, const TileSeed* tc);

void two_tile_fill(Area* a, TileSeed* tc1, TileSeed* tc2, int ratio);

void tree_pattern_fill(Area* a, TileSeed* tree1, TileSeed* tree2, int tree_number, int tree_ratio);

void veg_pattern_fill(Area* a, 
                      TileSeed* veg1, 
                      TileSeed* veg2,
                      int veg_number,
                      int veg_ratio,
                      TileSeed* avoid);

void cellular_automata(Area* a, TileSeed *tile_a, TileSeed *tile_b, int sum_a, int sum_b);           

/**********************
    RENDERING & FOV
**********************/

/**
    @param l
        Level to be rendered.
    @param camera
        Coordinates of tile you want to start rendering from, IE the top left-hand corner.    
    @param pc
        The player character.
    @param time
        A value between 0.0 and 1.0 that represents the time of day (1.0 is midday).
        This parameter controls the linear interpolation between a tiles day and night colours.
    @param fog_of_war    
        If true, tiles are only displayed if the player has seen them before. If false, every tile
        in the current rendering area is displayed whether it has been seen or not.
    @param directional
        If true, the PC will have a different FOV depending on what direction it is facing.
        If false, the FOV will simply be a disc centered on the player.
*/
void render(Level* l, Coord* camera, Creature * pc, float time, bool fog_of_war, bool directional);
                            
/************
    LEVEL
************/ 

Level* create_level(int height, int width);
void delete_level(Level* l);

/**********
    BSP
**********/

/** 
    @struct BSP_node
    
    Represents a single node or element of a Binary Space Partition Tree. 
*/
typedef struct {
    /**Level the node is on.*/
    Level* level;    
    /** Horizontal position of the top left corner. */
    guint start_x;
    /** Vertical position of the top left corner. */
    guint start_y;
    /** Gorizontal position of the bottom right corner. */
    guint end_x;
    /** Vertical position of the bottom right corner. */
    guint end_y;    
    /** Left child of the node (should be NULL if not applicable). */
    struct BSP_node* left;
    /** Right child of the node (should be NULL if not applicable). */
    struct BSP_node* right;    
}BSP_node;

/**
    Creates a 2D binary space parition tree. This is a recursive function, and will continue until
    the nodes are too small to statisy the min_height and min_width constraints.
 
    @param parent 
        Root node of the tree to be created. Should span the whole area.
    @param min_width
        minimum width of a child node
    @param min_height
        minimum height of a child node
    @param leaves 
        A linked list of all the leaves, or nodes without children. This should grow as the function
        recurses. NOTE that since the basic GList* is a pointer data structure, and the GList* is
        what gets modified (not the GList struct itself), a pointer to a pointer GList** is what
        gets passed in.
*/
void create_bsp_tree(BSP_node* parent, 
                     const guint min_width, 
                     const guint min_height, 
                     GList** leaves);
                       
BSP_node* create_bsp_node(Area* a,  
                          struct BSP_node* left,
                          struct BSP_node* right);
                          
void carve_rectangular_room (gpointer element_data, gpointer user_data);

/************
    MISC.
************/ 

//void clean_up();

#endif
