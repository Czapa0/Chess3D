#ifndef WHITE_KING_H
#define WHITE_KING_H

#include "Model.h"
class WhiteKing : public Model
{
public:
    // default constructror
    WhiteKing() = default;

    // constructor, expects a filepath to a 3D model.
    WhiteKing(std::string const& path, bool gamma = false);
};

#endif