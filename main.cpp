#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include "types.h"
#include "bitboard.h"
#include "attacks.h"
#include "search.h"
#include "move_generator.h"

int DEPTH = 6;
// function that checks if the move source -> targer that user makes is legal
// if it does not find the move in legal moves then return 0
Move find_user_move(Position &pos, int source, int target)
{
    MoveList move_list;
    generate_pseudo_legal_moves(pos, move_list);

    for (int i = 0; i < move_list.count; i++)
    {
        Move m = move_list.moves[i];
        if (get_source(m) == source && get_target(m) == target)
        {
            return m;
        }
    }
    return 0;
}

int main()
{
    // init with tables for leaper figures
    init_leaper_attacks();

    // board setup
    Position pos;
    board_setup(pos);
    update_occupancies(pos);
    pos.win = 0;
    pos.fullmove_number = 0;

    // sfml grafics library
    const int size_square = 80;
    const int window_size = size_square * 8;

    // window with title bar an X
    sf::RenderWindow window(sf::VideoMode({(unsigned int)window_size, (unsigned int)window_size}), "Chess Engine", sf::Style::Titlebar | sf::Style::Close);

    // rendering textures
    sf::Texture t_board, t_wp, t_wn, t_wb, t_wr, t_wq, t_wk;
    sf::Texture t_bp, t_bn, t_bb, t_br, t_bq, t_bk;

    // chessboard
    (void)t_board.loadFromFile("images_chess/rect-8x8.png");
    sf::Sprite s_board(t_board); // creating a sprite

    // we get the actual size of the chessboard image
    sf::Vector2u board_size = t_board.getSize();
    s_board.setScale({(float)window_size / board_size.x, (float)window_size / board_size.y}); // then I scale it

    (void)t_wp.loadFromFile("images_chess/pieces-png/white-pawn.png");
    (void)t_wn.loadFromFile("images_chess/pieces-png/white-knight.png");
    (void)t_wb.loadFromFile("images_chess/pieces-png/white-bishop.png");
    (void)t_wr.loadFromFile("images_chess/pieces-png/white-rook.png");
    (void)t_wq.loadFromFile("images_chess/pieces-png/white-queen.png");
    (void)t_wk.loadFromFile("images_chess/pieces-png/white-king.png");

    (void)t_bp.loadFromFile("images_chess/pieces-png/black-pawn.png");
    (void)t_bn.loadFromFile("images_chess/pieces-png/black-knight.png");
    (void)t_bb.loadFromFile("images_chess/pieces-png/black-bishop.png");
    (void)t_br.loadFromFile("images_chess/pieces-png/black-rook.png");
    (void)t_bq.loadFromFile("images_chess/pieces-png/black-queen.png");
    (void)t_bk.loadFromFile("images_chess/pieces-png/black-king.png");

    const sf::Texture *piece_textures[12];
    piece_textures[w_pawn] = &t_wp;
    piece_textures[w_knight] = &t_wn;
    piece_textures[w_bishop] = &t_wb;
    piece_textures[w_rook] = &t_wr;
    piece_textures[w_queen] = &t_wq;
    piece_textures[w_king] = &t_wk;

    piece_textures[b_pawn] = &t_bp;
    piece_textures[b_knight] = &t_bn;
    piece_textures[b_bishop] = &t_bb;
    piece_textures[b_rook] = &t_br;
    piece_textures[b_queen] = &t_bq;
    piece_textures[b_king] = &t_bk;

    int selected_square = -1;

    // main loop
    while (window.isOpen())
    {
        // every time the loop starts it checks whether we have any action
        // from the player and if so the it is stored in std::optional
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            // if event == event close then close the window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // getting cordinates of mouse click
            if (const auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (pos.side_to_move == white && mousePressed->button == sf::Mouse::Button::Left)
                {
                    // getting info on which square did user click
                    int file = mousePressed->position.x / size_square;
                    int rank = 7 - (mousePressed->position.y / size_square);
                    int clicked_square = rank * 8 + file;

                    if (selected_square == -1)
                    {
                        selected_square = clicked_square;
                    }
                    else // if we already have the selected square
                    {
                        int target_square = clicked_square;
                        Move user_move = find_user_move(pos, selected_square, target_square);

                        if (user_move != 0) // if user move is legal than move tha pice
                        {
                            Position copy = pos;
                            if (make_move(pos, user_move))
                            {
                                update_occupancies(pos);
                                selected_square = -1;

                                std::cout << "move " << pos.fullmove_number << ": evaluation = " << evaluate_postion(pos) << std::endl;
                                pos.win = check_game_over(pos);
                            }
                            else
                            {
                                pos = copy;
                                selected_square = -1;
                            }
                        }
                        else
                        {
                            selected_square = clicked_square;
                        }
                    }
                }
            }
        }

        // graphics
        window.clear();
        window.draw(s_board);

        // drawing the pices based on the bitboards
        for (int piece = 0; piece < 12; piece++)
        {
            for (int sq = 0; sq < 64; sq++)
            {
                if (get_bit(pos.bitboards[piece], sq))
                {
                    int file = sq % 8;
                    int rank = sq / 8;

                    float x = file * size_square;
                    float y = (7 - rank) * size_square;

                    sf::Sprite sprite(*piece_textures[piece]);

                    // scaling the pices
                    sf::Vector2u piece_size = piece_textures[piece]->getSize();
                    float scale_x = (float)size_square / piece_size.x;
                    float scale_y = (float)size_square / piece_size.y;

                    sprite.setScale({scale_x, scale_y});
                    sprite.setPosition({x, y});
                    window.draw(sprite);
                }
            }
        }

        window.display();

        // now the engine makes the move
        if (pos.side_to_move == black)
        {
            Move best_move = search(pos, DEPTH);
            make_move(pos, best_move);
            update_occupancies(pos);

            std::cout << "move " << pos.fullmove_number << ": evaluation = " << -evaluate_postion(pos) << std::endl;
            pos.win = check_game_over(pos);
        }

        // graphics
        window.clear();
        window.draw(s_board);

        // drawing the pices based on the bitboards
        for (int piece = 0; piece < 12; piece++)
        {
            for (int sq = 0; sq < 64; sq++)
            {
                if (get_bit(pos.bitboards[piece], sq))
                {
                    int file = sq % 8;
                    int rank = sq / 8;

                    float x = file * size_square;
                    float y = (7 - rank) * size_square;

                    sf::Sprite sprite(*piece_textures[piece]);

                    // scaling the pices
                    sf::Vector2u piece_size = piece_textures[piece]->getSize();
                    float scale_x = (float)size_square / piece_size.x;
                    float scale_y = (float)size_square / piece_size.y;

                    sprite.setScale({scale_x, scale_y});
                    sprite.setPosition({x, y});
                    window.draw(sprite);
                }
            }
        }

        window.display();
    }

    return 0;
}
