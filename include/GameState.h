#ifndef GAMESTATE_H
#define GAMESTATE_H


/*

  This class should hold any and all state information for the game
  and handle all things related to loading/saving/pause and time

*/

class GameState
{
  public:
    GameState();
    ~GameState();

    int current_vm = 0;
    int width = 640;
    int height = 480;
    bool shutdown = false;

  protected:

  private:
};

#endif // GAMESTATE_H
