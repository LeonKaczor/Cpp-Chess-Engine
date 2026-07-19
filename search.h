#pragma once
#include "types.h"
#include "move_generator.h"
#include "make_move.h"
#include "attacks.h"
#include "evaluate_position.h"
int negamax(Position &pos, int depth, int alpha, int beta);
Move search(Position &pos, int depth);
