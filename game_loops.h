#ifndef GAME_LOOPS_H_INCLUDED
#define GAME_LOOPS_H_INCLUDED

void loop_select_difficulty(void *data);                                        // loop functions act as callbacks, emscripten expects them to accept a void pointer to our user data
void loop_player_move(void *data);
void loop_opponent_move(void *data);

#endif // GAME_LOOPS_H_INCLUDED
