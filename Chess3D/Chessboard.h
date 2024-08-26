#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "Model.h"

class Chessboard : public Model
{
public:
    // default constructror
    Chessboard() = default;

    // constructor, expects a filepath to a 3D model.
    Chessboard(std::string const& path, bool gamma = false);
};

#endif