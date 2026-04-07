#ifndef T3_ACMOJ2455_TETRIS_HPP
#define T3_ACMOJ2455_TETRIS_HPP

#include <iostream>
#include <vector>
#include <string>

namespace sjtu {

    class XorShift {
    private:
        unsigned int x, y, z, w;

        void xor_shift32() {
            unsigned int t = x ^ (x << 11);
            x = y;
            y = z;
            z = w;
            w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        }

    public:
        explicit XorShift(unsigned int seed) {
            x = seed;
            y = seed;
            z = seed;
            w = seed;
            xor_shift32();
        }

        unsigned int next() {
            xor_shift32();
            return w;
        }
    };

    struct Tetris {
        struct Tetromino {
            struct Shape {
                std::vector<std::vector<int>> shape;
                int rotateX, rotateY;
            };
            constexpr static char shapeAlphas[7] = {'I', 'T', 'O', 'S', 'Z', 'J', 'L'};
            const static Shape rotateShapes[7][4];
            int x, y;
            int index;
            int state;
        };

        const static int WIDTH = 10;
        const static int HEIGHT = 20;

        XorShift rand; 
        Tetromino cur;
        int field[HEIGHT][WIDTH];
        int score;
        bool is_falling;

        Tetris(int seed);
        bool new_tetris();
        bool dropping();
        bool move_tetris(const std::string &operation);
        void place_tetris();
        void clear_lines();
        void print_field();
        bool game_over();
        bool check_collision(const Tetromino &next);
        void run();
    };

    const Tetris::Tetromino::Shape Tetris::Tetromino::rotateShapes[7][4] = {
            {
                    {{{1, 1, 1, 1}},         1, -1},
                    {{{1},    {1},    {1}, {1}}, -1, 2},
                    {{{1, 1, 1, 1}},         2, -2},
                    {{{1},    {1},    {1}, {1}}, -2, 1},
            }, // I
            {
                    {{{1, 1, 1}, {0, 1, 0}}, 0, -1},
                    {{{0, 1}, {1, 1}, {0, 1}},   0, 0},
                    {{{0, 1, 0}, {1, 1, 1}}, 1, 0},
                    {{{1, 0}, {1, 1}, {1, 0}},   -1, 1},
            }, // T
            {
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0}
            }, // O
            {
                    {{{0, 1, 1}, {1, 1, 0}}, 0, -1},
                    {{{1, 0}, {1, 1}, {0, 1}},   0, 0},
                    {{{0, 1, 1}, {1, 1, 0}}, 1, 0},
                    {{{1, 0}, {1, 1}, {0, 1}},   -1, 1}
            }, // S
            {
                    {{{1, 1, 0}, {0, 1, 1}}, 0, -1},
                    {{{0, 1}, {1, 1}, {1, 0}},   0, 0},
                    {{{1, 1, 0}, {0, 1, 1}}, 1, 0},
                    {{{0, 1}, {1, 1}, {1, 0}},   -1, 1}
            }, // Z
            {
                    {{{1, 0, 0}, {1, 1, 1}}, 1, 0},
                    {{{1, 1},    {1, 0}, {1, 0}}, -1, 1},
                    {{{1, 1, 1}, {0, 0, 1}}, 0, -1},
                    {{{0, 1},    {0, 1}, {1, 1}},   0, 0}
            }, // J
            {
                    {{{0, 0, 1}, {1, 1, 1}}, 1, 0},
                    {{{1, 0},    {1, 0}, {1, 1}},   -1, 1},
                    {{{1, 1, 1}, {1, 0, 0}}, 0, -1},
                    {{{1, 1},    {0, 1}, {0, 1}},   0, 0}
            } // L
    };

    Tetris::Tetris(int seed) : rand(seed) {
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                field[i][j] = 0;
            }
        }
        score = 0;
        is_falling = false;
    }

    bool Tetris::new_tetris() {
        cur.index = rand.next() % 7;
        cur.state = 0;
        cur.x = (WIDTH >> 1) - (Tetromino::rotateShapes[cur.index][0].shape[0].size() >> 1);
        cur.y = 0;
        is_falling = true;
        
        std::cout << Tetromino::shapeAlphas[cur.index] << "\n";
        
        if (check_collision(cur)) {
            return false;
        }
        return true;
    }

    bool Tetris::dropping() {
        Tetromino next = cur;
        next.y += 1;
        if (check_collision(next)) {
            return false;
        }
        cur = next;
        return true;
    }

    bool Tetris::move_tetris(const std::string &operation) {
        if (operation == "w") {
            Tetromino next = cur;
            next.x += Tetromino::rotateShapes[cur.index][cur.state].rotateX;
            next.y += Tetromino::rotateShapes[cur.index][cur.state].rotateY;
            next.state = (cur.state + 1) % 4;
            if (!check_collision(next)) {
                cur = next;
                return true;
            }
            return false;
        } else if (operation == "a") {
            Tetromino next = cur;
            next.x -= 1;
            if (!check_collision(next)) {
                cur = next;
                return true;
            }
            return false;
        } else if (operation == "d") {
            Tetromino next = cur;
            next.x += 1;
            if (!check_collision(next)) {
                cur = next;
                return true;
            }
            return false;
        } else if (operation == "s") {
            Tetromino next = cur;
            next.y += 1;
            if (!check_collision(next)) {
                cur = next;
                return true;
            }
            return false;
        } else if (operation == "space") {
            while (true) {
                Tetromino next = cur;
                next.y += 1;
                if (!check_collision(next)) {
                    cur = next;
                } else {
                    break;
                }
            }
            return true;
        } else if (operation == "e") {
            return true;
        }
        return false;
    }

    bool Tetris::check_collision(const Tetromino &next) {
        const auto& shape = Tetromino::rotateShapes[next.index][next.state].shape;
        for (int i = 0; i < shape.size(); ++i) {
            for (int j = 0; j < shape[i].size(); ++j) {
                if (shape[i][j]) {
                    int nx = next.x + j;
                    int ny = next.y + i;
                    if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) {
                        return true;
                    }
                    if (field[ny][nx]) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void Tetris::place_tetris() {
        const auto& shape = Tetromino::rotateShapes[cur.index][cur.state].shape;
        for (int i = 0; i < shape.size(); ++i) {
            for (int j = 0; j < shape[i].size(); ++j) {
                if (shape[i][j]) {
                    int nx = cur.x + j;
                    int ny = cur.y + i;
                    if (ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH) {
                        field[ny][nx] = 1;
                    }
                }
            }
        }
        is_falling = false;
    }

    void Tetris::clear_lines() {
        int lines_cleared = 0;
        for (int i = HEIGHT - 1; i >= 0; ) {
            bool full = true;
            for (int j = 0; j < WIDTH; ++j) {
                if (!field[i][j]) {
                    full = false;
                    break;
                }
            }
            if (full) {
                lines_cleared++;
                for (int k = i; k > 0; --k) {
                    for (int j = 0; j < WIDTH; ++j) {
                        field[k][j] = field[k - 1][j];
                    }
                }
                for (int j = 0; j < WIDTH; ++j) {
                    field[0][j] = 0;
                }
            } else {
                --i;
            }
        }
        if (lines_cleared == 1) score += 100;
        else if (lines_cleared == 2) score += 300;
        else if (lines_cleared == 3) score += 600;
        else if (lines_cleared == 4) score += 1000;
    }

    void Tetris::print_field() {
        int temp_field[HEIGHT][WIDTH];
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                temp_field[i][j] = field[i][j];
            }
        }
        if (is_falling) {
            const auto& shape = Tetromino::rotateShapes[cur.index][cur.state].shape;
            for (int i = 0; i < shape.size(); ++i) {
                for (int j = 0; j < shape[i].size(); ++j) {
                    if (shape[i][j]) {
                        int nx = cur.x + j;
                        int ny = cur.y + i;
                        if (ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH) {
                            temp_field[ny][nx] = 1;
                        }
                    }
                }
            }
        }
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                std::cout << temp_field[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    bool Tetris::game_over() {
        for (int j = 0; j < WIDTH; ++j) {
            if (field[0][j]) {
                return true;
            }
        }
        return false;
    }

    void Tetris::run() {
        if (!new_tetris()) {
            std::cout << "Game Over!\n";
            std::cout << "Your score: " << score << "\n";
            return;
        }
        
        while (true) {
            print_field();
            
            std::string op;
            bool valid_op = false;
            while (std::cin >> op) {
                if (op == "w" || op == "a" || op == "d" || op == "s" || op == "space" || op == "e" || op == "esc") {
                    valid_op = true;
                    break;
                }
            }
            
            if (!valid_op) {
                break;
            }
            
            if (op == "esc") {
                std::cout << "See you!\n";
                std::cout << "Your score: " << score << "\n";
                return;
            }
            
            if (move_tetris(op)) {
                std::cout << "Successful operation!\n";
            } else {
                std::cout << "Failed operation!\n";
            }
            
            if (!dropping()) {
                place_tetris();
                clear_lines();
                if (game_over()) {
                    print_field();
                    std::cout << "Game Over!\n";
                    std::cout << "Your score: " << score << "\n";
                    return;
                }
                if (!new_tetris()) {
                    std::cout << "Game Over!\n";
                    std::cout << "Your score: " << score << "\n";
                    return;
                }
            }
        }
    }

} 

#endif //T3_ACMOJ2455_TETRIS_HPP