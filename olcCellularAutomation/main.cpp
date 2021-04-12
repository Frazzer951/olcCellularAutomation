#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Game : public olc::PixelGameEngine
{
public:
  Game() { sAppName = "Cellular Automation"; }

private:
  std::unique_ptr<bool[]> cells;
  bool                    bSimulate        = false;
  float                   fTargetFrameTime = 1.0f / 100.0f;    // Virtual FPS of 100fps
  float                   fAccumulatedTime = 0.0f;

public:
  bool OnUserCreate() override
  {
    cells = std::make_unique<bool[]>( ScreenHeight() * ScreenWidth() );

    for( int x = 0; x < ScreenWidth(); x++ )
    {
      for( int y = 0; y < ScreenHeight(); y++ ) { cells[y * ScreenWidth() + x] = false; }
    }

    return true;
  }

  bool OnUserUpdate( float fElapsedTime ) override
  {
    // Take user input
    if( GetKey( olc::Key::SPACE ).bPressed ) { bSimulate = !bSimulate; }    // Pause and Unpause simulation
    if( GetKey( olc::Key::R ).bPressed ) { randomizeCells(); }              // Randomize all the cells
    if( GetKey( olc::Key::C ).bPressed ) { clearCells(); }                  // Clear all the cells

    // Left click to make cell alive
    if( GetMouse( 0 ).bHeld ) { cells[GetMouseY() * ScreenWidth() + GetMouseX()] = true; }
    // Right click to kill cell
    if( GetMouse( 1 ).bHeld ) { cells[GetMouseY() * ScreenWidth() + GetMouseX()] = false; }

    // Only update the game at the specified frame rate
    fAccumulatedTime += fElapsedTime;
    if( fAccumulatedTime >= fTargetFrameTime )
    {
      fAccumulatedTime -= fTargetFrameTime;
      fElapsedTime = fTargetFrameTime;
    }
    else
      return true;    // Don't do anything this frame

    // Only update the simulation if the game is not paused
    if( bSimulate ) { updateCells(); }

    // Clear the screen
    Clear( olc::VERY_DARK_GREY );

    for( int x = 0; x < ScreenWidth(); x++ )
    {
      for( int y = 0; y < ScreenHeight(); y++ )
      {
        switch( cells[y * ScreenWidth() + x] )
        {
          case false:    // Do Nothing
            break;
          case true:    // Draw Pixel
            Draw( { x, y }, olc::WHITE );
        }
      }
    }

    // Tell User the inputs
    DrawString( 0, 1, "Press R to randomize cells", olc::DARK_CYAN );
    DrawString( 0, 10, "Press C to clear cells", olc::DARK_CYAN );

    // If Paused display how to unpause on screen
    if( !bSimulate ) DrawString( 0, 19, "Press SPACE to Unpause", olc::DARK_CYAN );

    return true;
  }

  void randomizeCells()
  {
    for( int x = 0; x < ScreenWidth(); x++ )
    {
      for( int y = 0; y < ScreenHeight(); y++ ) { cells[y * ScreenWidth() + x] = rand() % 2; }
    }
  }

  void clearCells()
  {
    for( int x = 0; x < ScreenWidth(); x++ )
    {
      for( int y = 0; y < ScreenHeight(); y++ ) { cells[y * ScreenWidth() + x] = false; }
    }
  }

  void updateCells()
  {
    auto new_cells = std::make_unique<bool[]>( ScreenHeight() * ScreenWidth() );

    for( int x = 0; x < ScreenWidth(); x++ )
    {
      for( int y = 0; y < ScreenHeight(); y++ )
      {
        // First find neighbor count
        int neighbors = 0;
        for( int j = -1; j <= 1; j++ )
        {
          for( int i = -1; i <= 1; i++ )
          {
            if( i == 0 && j == 0 ) continue;
            if( cells[( ( j + y ) % ScreenHeight() ) * ScreenWidth() + ( ( i + x ) % ScreenWidth() )] == true )
              neighbors++;
          }
        }

        // Based on # of neighbors update cell
        if( neighbors == 3 ) new_cells[y * ScreenWidth() + x] = true;
        if( cells[y * ScreenWidth() + x] == true && neighbors == 2 ) new_cells[y * ScreenWidth() + x] = true;
      }
    }
    for( int i = 0; i < ScreenHeight() * ScreenWidth(); i++ ) cells[i] = new_cells[i];
  }
};

int main()
{
  Game game;
  if( game.Construct( 400, 400, 2, 2 ) ) game.Start();
  return 0;
}