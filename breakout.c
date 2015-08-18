// Michael Pease 
//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<math.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// height and width of game's paddle in pixels
#define PHEIGHT 10
#define PWIDTH 60

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// height and width of game's bricks in pixels
#define BHEIGHT 10
#define BWIDTH 36

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocity of ball in the x and y direction
    double xVelocity = ((1.0 * drand48()) + 2.0);
    double yVelocity = ((1.0 * drand48()) + 2.0);
    
    waitForClick();

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {   
		// sets ball in motion
        move(ball, xVelocity, yVelocity);
        
        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= WIDTH)
        
            xVelocity = -xVelocity;

        // bounce off left edge of window
        else if (getX(ball) <= 0)

            xVelocity = -xVelocity;
        
        // hit bottom edge bottom edge and reset, lose life
        else if (getY(ball) + getWidth(ball) >= HEIGHT)
        {
		    removeGWindow(window, ball);
            ball = initBall(window);
            lives--;
            waitForClick();
        }    
        // bounce off top edge of window    
        else if (getY(ball) <= 0)
        
            yVelocity = -yVelocity;
            
        // linger before moving again
        pause(10);
             
    	// creates variable to control paddle movement 
        GEvent paddleMovement = getNextEvent(MOUSE_EVENT);
        
        if (paddleMovement != NULL)
        {
            // if the event was movement 
            if (getEventType(paddleMovement) == MOUSE_MOVED)
            {
                // check to make sure the entire paddle stays within 
                // the confines of the game window
                if (getX(paddleMovement) >= (PWIDTH / 2) &&\
                    getX(paddleMovement) <= WIDTH - (PWIDTH/2))
            	{
            	    double x = getX(paddleMovement) - (PWIDTH/2);
                    setLocation(paddle, x, 550);
                }
            }
        }
        
        // detects if ball collides with another GObject in game window
    	GObject object = detectCollision(window, ball);
        
        if (object == NULL)
        {
            continue;   
        }
        
        else if (object == label)
        
            continue;
            
        else if (object == paddle)
        {
            yVelocity = -yVelocity;
        }
        
        else
        {
            yVelocity = -yVelocity;
            bricks--;
            points++;
            removeGWindow(window, object);
            updateScoreboard(window, label, points);
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    int brickLocationX = 2;
    int brickLocationY = 50;
    char* colors[5] = {"RED", "ORANGE", "YELLOW", "GREEN", "CYAN"};
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            GRect brick = newGRect(0, 0, BWIDTH, BHEIGHT);
            setFilled(brick, true);
            setColor(brick, colors[i]);
            setLocation(brick, brickLocationX, brickLocationY);
            brickLocationX += 40;
            add(window, brick);
            
            if (j == 9)
            {
                brickLocationX = 2;       
                brickLocationY += 14; 
            }
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(0, 0, 2 *RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    setLocation(ball, (WIDTH/2) - RADIUS, (HEIGHT/2) - RADIUS);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(0, 0, PWIDTH, PHEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    setLocation(paddle, ((WIDTH/2) - (PWIDTH/2)), 550);
    add(window, paddle);
    return paddle;
}

/*
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{	
    GLabel label = newGLabel("0");
    double x = (WIDTH / 2) - (getWidth(label) + 4.5);
    double y = ((HEIGHT - getHeight(label)) / 2) - RADIUS;
    setLocation(label, x, y);
    setColor(label, "GRAY");
    setFont(label, "Dialog-40");
    add(window, label);
    sendToBack(label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}

