#pragma once

class Board {
  public:
    Board();

    char get(int row, int col) const;
    void set(int row, int col, char piece);

  private:
    char layout[8][8];
};
