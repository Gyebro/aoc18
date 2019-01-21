#include "config.h"
#include "days.h"
#include "common.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <list>
#include <cmath>

int day01(string inputfile, bool partone) {
    // Parse input
    ifstream in(inputfile);
    vector<int> freqlist;
    string line;
    while(in.good()) {
        getline(in, line);
        freqlist.push_back(stoi(line));
    }
    int result = 0;
    if (partone) {
        // Part one
        for (int i : freqlist) { result += i; }
    } else {
        // Part two
        // Create a bool array to represent the number line
        int offset = 50000;
        vector<bool> n(500000,false);
        // Start applying the sequence, freq is the pointer to the number line
        int freq = 0;
        int i = 0;
        while (n[freq+offset] == false) {
            n[freq+offset] = true;
            freq += freqlist[i];
            i++;
            if (i == freqlist.size()) { i=0; }
        }
        result = freq;
    }
    return result;
}

int day02(string inputfile, bool partone) {
    int result = 0;
    // Parse input
    vector<string> keys = get_lines(inputfile);
    if (partone) {
        // Part One
        vector< pair<char, size_t> > t;
        size_t keys_two = 0;
        size_t keys_three = 0;
        for (string& key : keys) {
            t = string_tally(key);
            if(vector_pair_query(t, '*', (size_t)2, false, true)) keys_two++;
            if(vector_pair_query(t, '*', (size_t)3, false, true)) keys_three++;
        }
        result = keys_two * keys_three;
    } else {
        // Part Two
        // sort the keys
        sort(keys.begin(), keys.end());
        for (size_t i=0; i<keys.size()-1; i++) {
            if (string_diff_chars(keys[i], keys[i+1])==1) {
                cout << string_union(keys[i], keys[i+1]);
            }
        }
    }
    return result;
}

class day03_rect {
public:
    size_t x, y;
    size_t w, h;
    size_t ID;
    day03_rect() : x(0), y(0), w(0), h(0), ID(0) {};
    day03_rect(const string& line) {
        // Parse the claim of format
        // #ID @ X,Y: WxH
        vector<string> pt1 = split(line, '@');
        ID = stoul(pt1[0].substr(1));
        vector<string> pt2    = split(pt1[1], ':');
        vector<string> coords = split(pt2[0], ',');
        vector<string> size   = split(pt2[1], 'x');
        x = stoul(coords[0]);
        y = stoul(coords[1]);
        w = stoul(size[0]);
        h = stoul(size[1]);
    }
};

class day03_map {
private:
    size_t w, h;
    vector<size_t> data; /// row continuous storage
public:
    day03_map(size_t width, size_t height) : w(width), h(height) {
        data.resize(w*h);
        fill(data.begin(),data.end(),0);
    }
    void mark_row(size_t row, size_t first_col, size_t len, size_t ID) {
        // No bounds check for cols/width
        for (size_t c=first_col; c<first_col+len; c++) { data[row*w+c]++; }
    }
    void mark(const day03_rect& rect) {
        // No bounds check for rows/height
        for (size_t r=rect.y; r<rect.y+rect.h; r++) { mark_row(r, rect.x, rect.w, rect.ID); }
    }
    size_t count_multi_marked() {
        size_t multi = 0;
        for (size_t d : data) {
            if (d > 1) multi++;
        }
        return multi;
    }
    bool check_overlap_row(size_t row, size_t first_col, size_t len) {
        // Check row cells to have exactly one mark
        bool ok = true;
        for (size_t c=first_col; c<first_col+len; c++) {
            ok &= (data[row*w+c] == 1);
            if (!ok) break;
        }
        return ok;
    }
    bool check_overlap(const day03_rect& rect) {
        // Check rectangle for having exactly 1 mark
        bool ok = true;
        for (size_t r=rect.y; r<rect.y+rect.h; r++) {
            ok &= check_overlap_row(r, rect.x, rect.w);
            if (!ok) break;
        }
        return ok;
    }
};

size_t day03(string inputfile, bool partone) {
    // Parse input
    vector<string> lines = get_lines(inputfile);
    vector<day03_rect> rects(lines.size());
    for (string& line : lines) {
        rects.emplace_back(day03_rect(line));
    }
    // Build map
    day03_map map(1000,1000);
    for (day03_rect& r : rects) {
        map.mark(r);
    }
    size_t result = 0;
    if (partone) {
        result = map.count_multi_marked();
    } else {
        for (day03_rect& r : rects) {
            if (map.check_overlap(r)) {
                result = r.ID;
            }
        }
    }
    return result;
}

/// Days 04 to 12

class day13_cart_and_tracks {
private:
    enum class track {
        empty,
        straight,
        turn_slash,
        turn_backslash,
        intersection
    };
    enum class direction {
        left,
        down,
        right,
        up
    };
    class cart {
    public:
        direction d;
        size_t x;
        size_t y;
        size_t priority;
    private:
        static const size_t w = 150; // TODO: Tidy-up!
        size_t move; // Move order at intersections: left, straight, right, ...
    public:
        cart(size_t pos_x, size_t pos_y, direction dir) : x(pos_x), y(pos_y), d(dir), move(0) {
            priority = y*w+x;
        };
        void turn(bool left) {
            switch (d) {
                case direction::up:
                    d = left ? direction::left : direction::right;
                    break;
                case direction::left:
                    d = left ? direction::down : direction::up;
                    break;
                case direction::down:
                    d = left ? direction::right : direction::left;
                    break;
                case direction::right:
                    d = left ? direction:: up : direction::down;
                    break;
            }
        }
        void turn_with_track(bool slash) {
            /* Turn map:   /   \
            //  Left       /<  \<
            //  Right     >/  >\
            //  Up         /   \
            //             ^   ^
            //  Down       v   v
            //             /   \    */
            switch (d) {
                case direction::left:
                case direction::right:
                    turn(slash);
                    break;
                case direction::up:
                case direction::down:
                    turn(!slash);
                    break;
            }
        }
        void auto_turn() {
            switch (move % 3) {
                case 0:
                    turn(true); // Left
                    break;
                case 1:
                    // Straight
                    break;
                case 2:
                    turn(false); // Right
                    break;
            }
            move++;
        }
    };
    size_t w, h;
    vector<track> map; // row-continuous
    vector<cart> carts;
    bool move_cart(size_t c, bool remove_crashed = false) {
        // Move carts[c]
        size_t x = carts[c].x;
        size_t y = carts[c].y;
        direction d = carts[c].d;
        switch (d) {
            case direction::left:
                x--;
                break;
            case direction::right:
                x++;
                break;
            case direction::up:
                y--;
                break;
            case direction::down:
                y++;
                break;
        }
        // Update cart's location
        carts[c].x = x;
        carts[c].y = y;
        // Recalculate priority
        carts[c].priority = y*w+x;
        // Check track and turn cart if needed
        track t = map[y*w+x];
        switch (t) {
            case track::straight:
                // No need to turn cart
                break;
            case track::intersection:
                // Turn cart according to it's movement pattern
                carts[c].auto_turn();
                break;
            case track::turn_slash:
                // Turn cart based on track
                carts[c].turn_with_track(true);
                break;
            case track::turn_backslash:
                // Turn cart based on track
                carts[c].turn_with_track(false);
                break;
            case track::empty:
                cout << "Error: cart derailed!\n";
                break;
        }
        bool ok = true;
        // Check collisions with other carts
        for (size_t i=0; i<carts.size(); i++) {
            if (i != c) {
                size_t x_other = carts[i].x;
                size_t y_other = carts[i].y;
                if (x == x_other && y == y_other) {
                    if (!remove_crashed) {
                        // Not removing crashed carts
                        ok = false;
                        cout << "Cart " << c << " collides with cart " << i << " at (" << x << "," << y << ")\n";
                    } else {
                        // Removing crashed carts and ok = true if more than 1 cart left
                        vector<cart> remaining;
                        cout << "Cart " << c << " collides with cart " << i << " Elves will quickly remove them\n";
                        for (size_t j=0; j<carts.size(); j++) {
                            if (j != i && j != c) remaining.push_back(carts[j]);
                        }
                        carts = remaining;
                        if (carts.size() == 1) {
                            cout << "Last cart is at (" << carts[0].x << "," << carts[0].y << "), finishing tick...\n";
                            ok = false;
                        }
                    }
                }
            }
        }
        return ok;
    }
public:
    day13_cart_and_tracks(vector<string> lines) {
        h = lines.size();
        w = lines[0].size(); // Not checking invalid input map
        map.resize(h*w);
        char c;
        for (size_t y=0; y<h; y++) { // For every line
            for (size_t x=0; x<w; x++) {
                c = lines[y][x];
                track t;
                switch (c) {
                    case ' ':
                        t = track::empty;
                        break;
                    case '|':
                    case '-':
                        t = track::straight;
                        break;
                    case '\\':
                        t = track::turn_backslash;
                        break;
                    case '/':
                        t = track::turn_slash;
                        break;
                    case '+':
                        t = track::intersection;
                        break;
                    case 'v':
                        t = track::straight;
                        carts.push_back(cart(x, y, direction::down));
                        break;
                    case '^':
                        t = track::straight;
                        carts.push_back(cart(x, y, direction::up));
                        break;
                    case '<':
                        t = track::straight;
                        carts.push_back(cart(x, y, direction::left));
                        break;
                    case '>':
                        t = track::straight;
                        carts.push_back(cart(x, y, direction::right));
                        break;
                    default:
                        cout << "Error: invalid character in map!";
                        break;
                }
                map[y*w+x]=t;
            }
        }
        cout << "Found " << carts.size() << " carts on a map of " << h << "x" << w << endl;
    }
    bool tick(bool remove_crashed = false) {
        // Sort carts to determine their move order
        sort(carts.begin(), carts.end(), [](auto &l, auto &r) { return l.priority < r.priority; });
        // Move carts one by one and check for inter-tick collisions
        bool ok = true;
        for (size_t c = 0; c<carts.size(); c++) {
            ok &= move_cart(c, remove_crashed);
        }
        if (remove_crashed && !ok) {
            cout << "Last cart is at (" << carts[0].x << "," << carts[0].y << "), at the end of tick.\n";
        }
        return ok;
    }
};

void day13(string inputfile, bool partone) {
    // Create the map
    day13_cart_and_tracks cat(get_lines(inputfile));
    if (partone) {
        // Part one
        while (cat.tick());
    } else {
        // Part two
        while (cat.tick(true));
    }
}

void day14_combine(vector<uint8_t>& recipes, size_t& e1, size_t& e2) {
    uint8_t r1 = recipes[e1];
    uint8_t r2 = recipes[e2];
    uint8_t sum = r1+r2;
    if (sum < 10) {
        recipes.push_back(sum);
    } else {
        uint8_t n1 = sum / 10;
        recipes.push_back(n1);
        recipes.push_back(sum - n1*10);
    }
    // Move elves
    size_t l = recipes.size();
    e1 = (e1+r1+1) % l;
    e2 = (e2+r2+1) % l;
}

bool day14_check(const vector<uint8_t>& recipes, const vector<uint8_t>& target, size_t at) {
    // Check for target sequence at the given location
    bool ok = true;
    if (recipes.size() < at+target.size()) return false;
    for (size_t i=at; i<at+target.size(); i++) {
        ok &= recipes[i] == target[i-at];
    }
    return ok;
}

void day14(string inputfile, bool partone) {
    vector<uint8_t> recipes;
    recipes.push_back(3);
    recipes.push_back(7);
    string number = get_lines(inputfile)[0];
    size_t target_number = stoul(number);
    size_t sample = 10;
    // Elves' location
    size_t e1 = 0;
    size_t e2 = 1;
    if (partone) {
        while (recipes.size() < target_number+sample) {
            day14_combine(recipes, e1, e2);
        }
        for (size_t i=target_number; i<target_number+sample; i++) {
            cout << (int)recipes[i];
        }
        cout << endl;
    } else {
        // Part two
        vector<uint8_t> target;
        for (char c : number) {
            target.push_back(atol(&c));
        }
        // Generate some recipes until we have enough buffer
        while (recipes.size() < 2+target.size()) {
            day14_combine(recipes, e1, e2);
        }
        bool found = false;
        size_t r = recipes.size();
        size_t i = 0;
        while (!found) {
            // Combine recipes
            day14_combine(recipes, e1, e2);
            for (; r<=recipes.size(); r++) {
                found = day14_check(recipes, target, i); i++;
                if (found) break;
            }
        }
        cout << "Target sequence found at " << i << " recipes to the left: " << i-1 << endl;
    }
    return;
}

class day15_battle {
private:
    class unit;
    class tile {
    public:
        size_t x, y;
        size_t priority;
        vector<tile*> neighbors;
        bool wall;
        bool occupied;
        tile() {
            x = y = priority = 0;
            wall = occupied = false;
            neighbors.resize(0);
        }
        tile(size_t X, size_t Y, bool is_wall, bool has_unit) : x(X), y(Y),
                       wall(is_wall), occupied(has_unit) {
            priority = y*1000+x; // TODO: Use proper width
            neighbors.resize(0); // Will be generated later
        }
    };
    class unit {
    public:
        int hp; // 200
        int attack; // 3
        bool goblin; // false if elf
        bool alive;
        tile* tile_ptr;
        unit() {
            hp = 200;
            attack = 3;
            alive = true;
            goblin = false;
            tile_ptr = nullptr;
        }
        unit(bool is_goblin, tile* t_ptr) : goblin(is_goblin), tile_ptr(t_ptr) {
            hp = 200;
            attack = 3;
            alive = true;
        }
    };
    size_t W,H;
    vector<tile> graph; // Row-continuous matrix but also a node-edge graph representation
    vector<unit> units;
    enum class move_result {
        no_targets_left,
        not_moved_enemy_in_range,
        not_moved_enemy_inaccessibly,
        moved_enemy_in_range,
        moved
    };
    bool graph_bfs(tile* target_ptr, const size_t max_steps, vector<tile*>& front, vector<vector<tile*>>& paths) {
        // For every element in the frontline nodes, enumerate compatible children
        if (front.size() == 0) return false;
        //cout << "BFS: frontline nodes: " << front.size() << endl;
        vector<tile*> new_front;
        vector<vector<tile*>> new_paths;
        bool found = false;
        tile* t;
        for (size_t i=0; i<front.size(); i++) {
            t = front[i];
            for (tile* n : t->neighbors) {
                if (n == target_ptr) {
                    // Target reached
                    // Update paths with this single one
                    new_front.clear();
                    new_paths.clear();
                    front = new_front;
                    new_paths.push_back(paths[i]);
                    new_paths.back().push_back(n);
                    paths = new_paths;
                    return true;
                } else {
                    if (!n->occupied) {
                        // Check for max steps and already visited
                        if (paths[i].size()+1 >= max_steps) {
                            // Skip this node, max step constraint
                        } else if (contains(paths, n)) {
                            // Skip this node as current path already visited n
                        } else if (contains(new_front, n)) {
                            // Skip this node as it is already enumerated in the current depth
                        } else {
                            new_front.push_back(n);
                            new_paths.push_back(paths[i]);
                            new_paths.back().push_back(n);
                        }
                    }
                }
            }
        }
        front = new_front;
        paths = new_paths;
        return graph_bfs(target_ptr, max_steps, front, paths);
    }
    move_result move(unit& u) {
        vector<tile*> targets;
        for (const unit& t : units) {
            if (t.goblin == !u.goblin && t.alive) targets.push_back(t.tile_ptr);
        }
        if (targets.empty()) {
            return move_result::no_targets_left;
        } else {
            size_t lowest_distance = W*H;
            size_t dist;
            bool possible_to_move = false;
            vector<vector<tile*>> paths;
            vector<tile*> front;
            vector<tile*> shortest_path;
            for (tile* t : targets) {
                front.clear();
                paths.clear();
                front.push_back(u.tile_ptr);
                paths.push_back(front);
                bool reachable = graph_bfs(t, lowest_distance, front, paths);
                if (reachable) {
                    dist = paths[0].size();
                    possible_to_move = true;
                    if (dist < lowest_distance) {
                        lowest_distance = dist;
                        shortest_path = paths[0];
                    }
                }
            }
            if (possible_to_move) {
                if (lowest_distance == 2) {
                    //cout << "Not moved, as target at " << shortest_path.back()->priority << " is in range\n";
                    return move_result::not_moved_enemy_in_range;
                } else {
                    // Move unit
                    u.tile_ptr->occupied = false;
                    u.tile_ptr = shortest_path[1]; // 0-th element is the start tile
                    u.tile_ptr->occupied = true;
                    if (lowest_distance == 3) {
                        return move_result::moved_enemy_in_range;
                    } else {
                        return move_result::moved;
                    }
                }
            } else {
                // There is a target, but not reachable
                return move_result::not_moved_enemy_inaccessibly;
            }
        }
    }
    class unitinfo { // Helper struct for selecting enemy
    public:
        int hp;
        size_t id;
        size_t priority;
        unitinfo(size_t id, int hp, size_t priority) : hp(hp), id(id), priority(priority) {};
    };
    void attack(unit& u) {
        vector<tile*> occupied_tiles;
        // Find closest unit with least hp or with highest priority in range (4 neighbouring tiles)
        for (tile* n : u.tile_ptr->neighbors) {
            if (n->occupied) occupied_tiles.push_back(n);
        }
        vector<unitinfo> enemies;
        for (size_t i=0; i<units.size(); i++) {
            for (tile* t : occupied_tiles) {
                // TODO: Find bug, why are there alive units at this point, someone else steps on them?
                if (units[i].tile_ptr == t && units[i].goblin != u.goblin && units[i].alive) {
                    // units[i] is an enemy in range
                    enemies.emplace_back(unitinfo(i, units[i].hp, t->priority));
                }
            }
        }
        if (enemies.empty()) {
            cout << "Error: unit can't attack, but an enemy should be in it's range!\n";
        } else {
            // First sort based on priority
            sort(enemies.begin(), enemies.end(), [](auto &l, auto &r) { return l.priority < r.priority; });
            // Then sort based on HP, so that in case of tie' priority will be the second sort
            sort(enemies.begin(), enemies.end(), [](auto &l, auto &r) { return l.hp < r.hp; });
            size_t target_id = enemies[0].id;
            // Attack
            if (units[target_id].hp <= 0) {
                cout << "BUG: Error here, hitting an already dead unit!\n";
            }
            units[target_id].hp -= u.attack;
            if (units[target_id].hp <= 0) {
                //cout << "Unit " << target_id << " died!\n";
                units[target_id].alive = false;
                units[target_id].tile_ptr->occupied = false;
            }
        }
    }
    bool round(bool stop_if_an_elf_dies = false) {
        // Sort units based on their tile's priority
        sort(units.begin(), units.end(), [](auto &l, auto &r) { return l.tile_ptr->priority < r.tile_ptr->priority; });
        // For every unit
        for (unit& u : units) {
            if (u.alive) {
                // Move with units
                switch (move(u)) {
                    case move_result::no_targets_left:
                        // End battle
                        return false;
                    case move_result::moved:
                        // Moved but cant attack yet
                        break;
                    case move_result::moved_enemy_in_range:
                    case move_result::not_moved_enemy_in_range:
                        attack(u);
                        break;
                    case move_result::not_moved_enemy_inaccessibly:
                        // Do nothing
                        break;
                }
            }
        }
        if (stop_if_an_elf_dies) {
            for (unit& u : units) {
                if (!u.goblin && !u.alive) return false;
            }
        }
        return true;
    }
    void build_edges() {
        size_t x,y;
        for (tile& t : graph) {
            if (!t.wall) {
                // Check four neighbours in reading order
                x = t.x; y = t.y;
                if (y > 0) {   if (!graph[(y-1)*W+x].wall) t.neighbors.push_back(&graph[(y-1)*W+x]); } // Top neighbour
                if (x > 0) {   if (!graph[y*W+(x-1)].wall) t.neighbors.push_back(&graph[y*W+(x-1)]); } // Left
                if (x < W-1) { if (!graph[y*W+(x+1)].wall) t.neighbors.push_back(&graph[y*W+(x+1)]); } // Right
                if (y < H-1) { if (!graph[(y+1)*W+x].wall) t.neighbors.push_back(&graph[(y+1)*W+x]); } // Bottom
            }
        }
    }
public:
    day15_battle(vector<string> lines) {
        // Build a graph which will hold tiles. Walls will be edgeless nodes
        H = lines.size();
        W = lines[0].size();
        units.resize(0);
        graph.reserve(H*W); // otherwise tile_ptrs will be invalid upon vector auto-resize
        size_t nodes = 0;
        for (size_t y = 0; y < H; y++) {
            for (size_t x = 0; x < W; x++) {
                switch (lines[y][x]) {
                    case '#':
                        // Add wall tile
                        graph.emplace_back(tile(x, y, true, false));
                        break;
                    case '.':
                        // Add tile
                        graph.emplace_back(tile(x, y, false, false));
                        nodes++;
                        break;
                    case 'G':
                        // Add Goblin + tile below
                        graph.emplace_back(tile(x, y, false, true));
                        units.emplace_back(unit(true, &(graph.back())));
                        nodes++;
                        break;
                    case 'E':
                        // Add Elf + tile below
                        graph.emplace_back(tile(x, y, false, true));
                        units.emplace_back(unit(false, &(graph.back())));
                        nodes++;
                        break;
                    default:
                        cout << "Error: invalid character found in battle map!\n";
                        break;
                }
            }
        }
        build_edges();
        //cout << "Built graph consisting " << nodes << " nodes\n";
        //cout << "Units on battlefield: " << units.size() << endl;
    }
    size_t start() {
        size_t rounds = 0;
        while (round()) rounds++; // not counting the last round when the battle ends
        // Calculate the sum of HPs of alive units
        size_t hp_sum = 0;
        for (unit& u : units) {
            if (u.alive) hp_sum += u.hp;
        }
        //cout << rounds << endl;
        //cout << hp_sum << endl;
        return rounds*hp_sum;
    }
    bool start(int elves_attack, size_t& outcome) {
        size_t rounds = 0;
        // Set attack power
        for (unit& u : units) {
            if (!u.goblin) u.attack = elves_attack;
        }
        while (round(true)) rounds++; // not counting the last round when the battle ends
        // Calculate the sum of HPs of alive units
        size_t hp_sum = 0;
        bool success = true;
        for (unit& u : units) {
            if (u.alive) hp_sum += u.hp;
            if (!u.goblin && !u.alive) success = false;
        }
        outcome = rounds*hp_sum;
        return success;
    }
};



void day15(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    size_t outcome;
    if (partone) {
        day15_battle battle(lines);
        outcome = battle.start();
        cout << "Outcome = " << outcome << endl;
    } else {
        for (size_t elves_attack = 4; elves_attack < 200; elves_attack++) {
            day15_battle battle(lines);
            if (battle.start(elves_attack, outcome)) {
                cout << "Elves won with attack_power = " << elves_attack << ", outcome = " << outcome << endl;
                break;
            } else {
                cout << "An elf died when the elves had attack_power = " << elves_attack << endl;
            }
        }
    }
    return;
}

class day16_instr {
public:
    uint8_t opcode;
    int a, b, c;
};

enum class day16_op {
    addr,
    addi,
    mulr,
    muli,
    banr,
    bani,
    borr,
    bori,
    setr,
    seti,
    gtir,
    gtri,
    gtrr,
    eqir,
    eqri,
    eqrr
};

const day16_op day16_ops[] = {day16_op::addr, day16_op::addi, day16_op::mulr, day16_op::muli,
                              day16_op::banr, day16_op::bani, day16_op::borr, day16_op::bori,
                              day16_op::setr, day16_op::seti, day16_op::gtir, day16_op::gtri,
                              day16_op::gtrr, day16_op::eqir, day16_op::eqri, day16_op::eqrr};

const vector<string> day16_op_names = {"addr", "addi", "mulr", "muli",
                                 "banr", "bani", "borr", "bori",
                                 "setr", "seti", "gtir", "gtri",
                                 "gtrr", "eqir", "eqri", "eqrr"};

// Apply instruction 'i' as operation 'o' on registers 'r'
vector<int> day16_apply(day16_op o, day16_instr i, const vector<int>& r) {
    vector<int> ro = r;
    switch (o) {
        case day16_op::addr:
            ro[i.c] = r[i.a]+r[i.b];
            break;
        case day16_op::addi:
            ro[i.c] = r[i.a]+i.b;
            break;
        case day16_op::mulr:
            ro[i.c] = r[i.a]*r[i.b];
            break;
        case day16_op::muli:
            ro[i.c] = r[i.a]*i.b;
            break;
        case day16_op::banr:
            ro[i.c] = r[i.a] & r[i.b];
            break;
        case day16_op::bani:
            ro[i.c] = r[i.a] & i.b;
            break;
        case day16_op::borr:
            ro[i.c] = r[i.a] | r[i.b];
            break;
        case day16_op::bori:
            ro[i.c] = r[i.a] | i.b;
            break;
        case day16_op::setr:
            ro[i.c] = r[i.a];
            break;
        case day16_op::seti:
            ro[i.c] = i.a;
            break;
        case day16_op::gtir:
            ro[i.c] = (i.a > r[i.b]);
            break;
        case day16_op::gtri:
            ro[i.c] = (r[i.a] > i.b);
            break;
        case day16_op::gtrr:
            ro[i.c] = (r[i.a] > r[i.b]);
            break;
        case day16_op::eqir:
            ro[i.c] = (i.a == r[i.b]);
            break;
        case day16_op::eqri:
            ro[i.c] = (r[i.a] == i.b);
            break;
        case day16_op::eqrr:
            ro[i.c] = (r[i.a] == r[i.b]);
            break;
    }
    return ro;
}

class day16_sample {
private:
    vector<int> before;
    vector<int> after;
    day16_instr instr;
public:
    day16_sample(const string& l1, const string& l2, const string& l3) {
        vector<string> before_str = split(l1.substr(9,10),',');
        vector<string> instr_str  = split(l2, ' ');
        vector<string> after_str  = split(l3.substr(9,10),',');
        for (string& s : before_str) { before.push_back(stoi(s)); }
        for (string& s : after_str)  { after.push_back(stoi(s)); }
        instr.opcode = stoul(instr_str[0]);
        instr.a = stoi(instr_str[1]);
        instr.b = stoi(instr_str[2]);
        instr.c = stoi(instr_str[3]);
    }
    size_t test() {
        size_t matches = 0;
        bool match;
        vector<int> result;
        for (const auto op : day16_ops) {
            result = day16_apply(op, instr, before);
            match = true;
            for (size_t i=0; i<4; i++) {
                if (result[i] != after[i]) {
                    match = false;
                }
            }
            if (match) matches++;
        }
        return matches;
    }
    void test_with_dictionary(vector<pair<uint8_t, day16_op>>& dict) {
        // Check if current opcode already recognized
        for (auto& p : dict) {
            if (p.first == instr.opcode) return;
        }
        size_t matches = 0;
        bool match;
        vector<int> result;
        day16_op last_match_op;
        uint8_t last_match_opcode;
        for (const auto op : day16_ops) {
            result = day16_apply(op, instr, before);
            match = true;
            for (size_t i=0; i<4; i++) {
                if (result[i] != after[i]) {
                    match = false;
                }
            }
            if (match) {
                // Check if this match belongs to an already identified opcode
                bool already_found = false;
                for (auto& p : dict) {
                    if (p.second == op) already_found = true;
                }
                if (!already_found) {
                    matches++;
                    last_match_op = op;
                    last_match_opcode = instr.opcode;
                }
            }
        }
        if (matches == 1) {
            cout << "Opcode " << (int)last_match_opcode << " is identified as " << day16_op_names[(int)last_match_op] << "!\n";
            dict.emplace_back( pair<uint8_t, day16_op>({last_match_opcode, last_match_op}) );
        }
    }
};

void day16(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    vector<day16_sample> samples;
    size_t split_line;
    for (size_t i=0; i<lines.size(); i+=4) {
        if (lines[i].substr(0,6) == "Before") {
            samples.emplace_back(day16_sample(lines[i],lines[i+1],lines[i+2]));
        } else {
            split_line = i;
            break;
        }
    }
    cout << "Found " << samples.size() << " samples\n";
    cout << "First part of input ends at line " << split_line << endl;
    if (partone) {
        size_t three_or_more = 0;
        for (day16_sample& s : samples) {
            if (s.test() >= 3) three_or_more++;
        }
        cout << "Samples behaving like 3 or more opcodes: " << three_or_more << endl;
    } else {
        vector< pair<uint8_t, day16_op> > opcode_dictionary;
        while(opcode_dictionary.size() < 16) {
            for (day16_sample& s : samples) {
                s.test_with_dictionary(opcode_dictionary);
            }
        }
        cout << "All opcodes identified, parsing program...\n";
        // Parse lines
        day16_instr i;
        vector<day16_instr> program;
        for (size_t j=split_line; j<lines.size(); j++) {
            vector<string> p = split(lines[j], ' ');
            if (p.size()==4) {
                i.opcode = stoul(p[0]);
                i.a = stoul(p[1]);
                i.b = stoul(p[2]);
                i.c = stoul(p[3]);
                program.push_back(i);
            }
        }
        cout << "Found " << program.size() << " instructions\n";
        // Sort dictionary based on opcode
        sort(opcode_dictionary.begin(), opcode_dictionary.end(),
             [] (const auto& l, const auto& r) { return l.first < r.first; });
        // Registers
        vector<int> regs(4);
        fill(regs.begin(), regs.end(), 0);
        // Run program
        cout << "Running program...\n";
        for(const day16_instr& i : program) {
            regs = day16_apply(opcode_dictionary[i.opcode].second, i, regs);
        }
        cout << "Register 0 is '" << regs[0] << "'\n";
    }
}

class day17_depth_map {
public:
    enum class tile {
        sand,
        clay,
        water,
        settled_water
    };
    class clayrange {
    public:
        bool x_fixed;
        size_t fixed;
        size_t min, max;
        clayrange() {
            x_fixed = true; fixed = min = max = 0;
        }
        clayrange(const string& s) {
            vector<string> p = split(s, ',');
            vector<string> p1 = split(p[0],'=');
            fixed = stoul(p1[1]);
            x_fixed = p1[0]=="x";
            p1 = split(p[1],'=');
            vector<string> p2 = split(p1[1],'.');
            min = stoul(p2.front());
            max = stoul(p2.back());
        }
    };
    pair<size_t, size_t> source;
    list<pair<size_t, size_t>> waterfront;
    vector<vector<tile>> map;
    size_t xmin, xmax, ymin, ymax;
    day17_depth_map(string& inputfile) {
        vector<string> lines = get_lines(inputfile);
        vector<clayrange> clays;
        clayrange cr;
        xmin = 10000; xmax = 0;
        ymin = 10000; ymax = 0;
        for (string& line : lines) {
            if (line.size()>3) {
                cr = clayrange(line);
                if (cr.x_fixed) {
                    minmax(cr.fixed, xmin, xmax);
                    minmax(cr.min, ymin, ymax);
                    minmax(cr.max, ymin, ymax);
                } else {
                    minmax(cr.fixed, ymin, ymax);
                    minmax(cr.min, xmin, xmax);
                    minmax(cr.max, xmin, xmax);
                }
                clays.push_back(cr);
            }
        }
        // Field dimensions along x should be extended by 1
        xmin--; xmax++;
        cout << "Field ranges x=" << xmin << ".." << xmax << " y=" << ymin << ".." << ymax << endl;
        // Build map matrix
        vector<tile> row;
        row.resize(xmax-xmin+1);
        fill(row.begin(), row.end(), tile::sand);
        map.resize(ymax-ymin+1);
        fill(map.begin(), map.end(), row);
        cout << "Depth map is " << map.size() << "x" << map[0].size() << " large\n";
        // Insert clays
        size_t claycount = 0;
        for (const clayrange& c : clays) {
            if (c.x_fixed) {
                size_t x = c.fixed;
                for (size_t y = c.min; y <= c.max; y++) {
                    if (map[y-ymin][x-xmin] != tile::clay) {
                        map[y-ymin][x-xmin] = tile::clay;
                        claycount++;
                    }
                }
            } else {
                // y value is fixed
                size_t y = c.fixed;
                for (size_t x = c.min; x <= c.max; x++) {
                    if (map[y-ymin][x-xmin] != tile::clay) {
                        map[y-ymin][x-xmin] = tile::clay;
                        claycount++;
                    }
                }
            }
        }
        cout << "Scan reports " << claycount << " blocks of clay\n";
        // Water source
        source = {500, ymin};
        get_tile(500, ymin) = tile::water;
        // Waterfront is the single source currently
        waterfront.push_back(source);
        cout << "Waterfront is currently at " << source.first << "," << source.second << endl;
    }
    void print(ostream& out, bool area_only=false, size_t x=0, size_t y=0) {
        const int width = 15;
        if (area_only) {
            for (int yy=y-width; yy<=y+width; yy++) {
                for (int xx=x-width; xx<=x+width; xx++) {
                    if (xx >= xmin && xx <= xmax && yy >= ymin && yy <= ymax) {
                        if (xx == x && yy == y) {
                            switch (get_tile(xx,yy)) {
                                case tile::sand:
                                    out << ':';
                                    break;
                                case tile::clay:
                                    out << 'X';
                                    break;
                                case tile::water:
                                    out << '"';
                                    break;
                                case tile::settled_water:
                                    out << '=';
                                    break;
                            }
                        } else {
                            switch (get_tile(xx,yy)) {
                                case tile::sand:
                                    out << '.';
                                    break;
                                case tile::clay:
                                    out << '#';
                                    break;
                                case tile::water:
                                    out << '|';
                                    break;
                                case tile::settled_water:
                                    out << '~';
                                    break;
                            }
                        }

                    }
                }
                cout << endl;
            }
        } else {
            for (vector<tile>& row : map) {
                for (tile& t : row) {
                    switch (t) {
                        case tile::sand:
                            out << '.';
                            break;
                        case tile::clay:
                            out << '#';
                            break;
                        case tile::water:
                            out << '|';
                            break;
                        case tile::settled_water:
                            out << '~';
                            break;
                    }
                }
                out << endl;
            }
        }
    }
    tile& get_tile(const size_t x, const size_t y) {
        return map[y-ymin][x-xmin];
    }
    void spread_water(size_t x, size_t y, size_t& left_wall, size_t& right_wall, bool left) {
        bool spreading = true;
        while (spreading) {
            if (left) {x--;} else {x++;}
            // TODO: Fails if diagonal clay walls are allowed!
            if (get_tile(x,y+1)==tile::clay || get_tile(x,y+1)==tile::settled_water) {
                if (get_tile(x,y)==tile::sand) {
                    get_tile(x,y)=tile::water; // Sand turns to water
                } else if (get_tile(x,y)==tile::clay) {
                    if (left) {left_wall = x;} else {right_wall = x;}
                    spreading = false;
                } else if (get_tile(x,y)==tile::water) {
                    // Go on spreading, but check for sand gap
                    if (left) { if (get_tile(x+1,y)==tile::sand) get_tile(x+1,y)=tile::water; }
                    else      { if (get_tile(x-1,y)==tile::sand) get_tile(x-1,y)=tile::water; }
                } else {
                    cout << "Warning: horizontal flow meets [settled_water]!\n";
                    spreading = false;
                }
            } else if (get_tile(x,y)==tile::sand) { // No support and left side is sand
                get_tile(x,y)=tile::water;
                waterfront.emplace_back(pair<size_t,size_t>(x,y)); // No support, new waterfront
                spreading = false;
            } else {
                // No support and tile is something else
                spreading = false;
            }
        }
    }
    void solve_waterfront(pair<size_t, size_t> wf, bool print_to_console = false) {
        // Solve a single waterfront and create new ones if necessary
        // Waterfront should be an (unsettled) water
        // Fall downwards until
        // a) [clay or settled_water] is met
        // b) map ends
        // Case a) save current location for backtrack
        //  Move to the left and right until water is supported by [clay|settled]
        //  Case i) horizontal movement is blocked by [clay]
        //  Case ii) support ends -> new waterfront
        // If both ends are blocked by [clay], create a row of [settled_water] and backtrack
        size_t x = wf.first;
        size_t y = wf.second;
        bool flowing = true;
        size_t back_x, back_y;
        bool spreading;
        size_t left_wall, right_wall;
        while (flowing) { // tile(x,y) == water
            if (print_to_console) {
                print(cout, true, x, y);
                cout << endl;
            }
            if (get_tile(x,y) != tile::water) {
                //cout << "Warning: waterfront contains non-[water] tile!\n";
                switch(get_tile(x,y)) {
                    case tile::sand:
                        get_tile(x,y) = tile::water;
                        break;
                    case tile::settled_water:
                    case tile::clay:
                        flowing = false;
                        break;
                }
            }
            // Check next tile below
            if (y+1 > ymax) {
                // End of flow
                flowing = false;
            } else {
                switch (get_tile(x,y+1)) {
                    case tile::water:
                        // End of flow, flat unsettled water should already be resolved
                        flowing = false;
                        break;
                    case tile::sand:
                        // Flowing continues down, wetting the current sand
                        get_tile(x,y+1)=tile::water;
                        y++;
                        break;
                    case tile::clay:
                    case tile::settled_water:
                        // Start spreading horizontally
                        back_x=x; back_y=y-1;
                        left_wall = right_wall = 0;
                        spread_water(x, y, left_wall, right_wall, true); // Spread left
                        spread_water(x, y, left_wall, right_wall, false); // Spread right
                        if (left_wall > 0 && right_wall > 0) {
                            // Convert water to settled_water
                            for (size_t wx = left_wall+1; wx<right_wall; wx++) {
                                get_tile(wx, y) = tile::settled_water;
                            }
                            // Backtrack
                            x = back_x;
                            y = back_y;
                        } else {
                            flowing = false;
                        }
                        break;
                }
            }

        }

    }
    void solve(bool print_to_console = false) {
        // Continue all waterfronts and simulate water flow
        while (waterfront.size() > 0) {
            solve_waterfront(waterfront.front(),print_to_console);
            waterfront.pop_front();
        }
    }
    size_t count_water(bool settled_only = false) {
        size_t water = 0;
        for (const vector<tile>& row : map) {
            for (const tile& t : row) {
                if (settled_only) {
                    if (t == tile::settled_water) water++;
                } else {
                    if (t == tile::water || t == tile::settled_water) water++;
                }
            }
        }
        return water;
    }
};

void day17(string inputfile, bool partone) {
    day17_depth_map map(inputfile);
    map.solve();
    if (partone) {
        cout << "Water amount: " << map.count_water() << endl;
    } else {
        cout << "Settled water amount: " << map.count_water(true) << endl;
    }
    ofstream out(inputfile+".map.txt");
    map.print(out);
}

class day18_forest {
public:
    enum class acre {
        open_ground,
        trees,
        lumberyard,
        undefined
    };
private:
    size_t W;
    size_t H;
    uint8_t frame_ptr;
    vector<acre> maps[2]; // Row-continuous
    // Double buffering for ticks to increase speed
public:
    day18_forest(string inputfile) {
        vector<string> lines = get_lines(inputfile);
        H = lines.size();
        W = lines[0].size();
        cout << "Building map for a " << H << "x" << W << " acre large forest\n";
        vector<acre> row;
        frame_ptr = 0;
        maps[frame_ptr].reserve(H*W);
        acre a = acre::undefined;
        for (const string& line : lines) {
            for (const char& c : line) {
                switch (c) {
                    case '|':
                        a = acre::trees;
                        break;
                    case '.':
                        a = acre::open_ground;
                        break;
                    case '#':
                        a = acre::lumberyard;
                        break;
                    default:
                        cout << "Error: invalid character found in the map of forest '" << c << "'\n";
                        break;
                }
                maps[frame_ptr].push_back(a);
            }
        }
        // Initialize other map
        maps[1] = maps[0];
    }
    acre get(int x, int y) {
        if (0 <= x && x < W && 0 <= y && y < H) {
            return maps[frame_ptr][y*W+x];
        } else {
            return acre::undefined;
        }
    }
    bool magic(int x, int y) {
        // Get neighbours
        vector<acre> neighbours;
        neighbours.push_back(get(x-1,y-1));
        neighbours.push_back(get(x  ,y-1));
        neighbours.push_back(get(x+1,y-1));
        neighbours.push_back(get(x-1,y));
        // Skip itself           x  ,y
        neighbours.push_back(get(x+1,y));
        neighbours.push_back(get(x-1,y+1));
        neighbours.push_back(get(x  ,y+1));
        neighbours.push_back(get(x+1,y+1));
        size_t trees       =  count(neighbours.begin(), neighbours.end(), acre::trees);
        size_t lumberyards =  count(neighbours.begin(), neighbours.end(), acre::lumberyard);
        size_t open_grounds = count(neighbours.begin(), neighbours.end(), acre::open_ground);
        acre result = maps[frame_ptr][y*W+x]; // Assume that nothing happens
        switch (maps[frame_ptr][y*W+x]) {
            case acre::open_ground:
                // An open acre will become filled with trees if three or more adjacent acres contained trees.
                // Otherwise, nothing happens.
                if (trees >= 3) result = acre::trees;
                break;
            case acre::trees:
                // An acre filled with trees will become a lumberyard if three or more adjacent acres were lumberyards.
                // Otherwise, nothing happens.
                if (lumberyards >= 3) result = acre::lumberyard;
                break;
            case acre::lumberyard:
                // An acre containing a lumberyard will remain a lumberyard if it was adjacent to at least one
                // other lumberyard and at least one acre containing trees. Otherwise, it becomes open.
                if (lumberyards == 0 || trees == 0) result = acre::open_ground;
                break;
            case acre::undefined:
                cout << "Error: map contains undefined acre!\n";
                break;
        }
        uint8_t other = (frame_ptr == 0) ? 1 : 0;
        maps[other][y*W+x] = result;
        return maps[frame_ptr][y*W+x] == result; // Return true if no change
    }
    void print() {
        for (size_t y = 0; y < H; y++) {
            for (size_t x = 0; x < W; x++) {
                switch (maps[frame_ptr][y*W+x]) {
                    case acre::open_ground:
                        cout << '.'; break;
                    case acre::trees:
                        cout << '|'; break;
                    case acre::lumberyard:
                        cout << '#'; break;
                    case acre::undefined:
                        cout << '?'; break;
                }
            }
            cout << endl;
        }
        cout << endl;
    }
    bool tick(bool print_forest = false) {
        bool steady = true;
        for (size_t y = 0; y < H; y++) {
            for (size_t x = 0; x < W; x++) {
                steady &= magic(x, y);
            }
        }
        // Replace map after tick is done
        uint8_t other = (frame_ptr == 0) ? 1 : 0;
        frame_ptr = other;
        if (print_forest) {
            print();
        }
        return steady;
    }
    size_t count_acres(const acre& a) {
        return count(maps[frame_ptr].begin(), maps[frame_ptr].end(), a);
    }
};

void day18(string inputfile, bool partone) {
    day18_forest forest(inputfile);
    if (partone) {
        for (size_t t=0; t<10; t++) forest.tick(false); // true will print the map
        size_t resource = forest.count_acres(day18_forest::acre::trees) * forest.count_acres(day18_forest::acre::lumberyard);
        cout << "Resource value: " << resource << endl;
    } else {
        vector<size_t> resource_values;
        size_t resource;
        size_t t;
        size_t t_end = 1000000000;
        size_t period = 0; // Period candidate, will be accepted if two successive samples show the same period
        for (t=1; t<=t_end; t++) {
            if (forest.tick(false)) { // Check for steady state
                cout << "Steady state after " << t << " minutes\n";
                break;
            } else { // Check for periodicity
                resource = forest.count_acres(day18_forest::acre::trees) * forest.count_acres(day18_forest::acre::lumberyard);
                if (contains(resource_values, resource)) {
                    auto result = find(resource_values.rbegin(), resource_values.rend(), resource);
                    size_t pos = distance(resource_values.rbegin(), result)+1;
                    if (period == pos) {
                        period = pos;
                        resource_values.push_back(resource);
                        cout << "Forest evolution is periodic with p = " << period << endl;
                        cout << "Stopped at t = " << t << endl;
                        break;
                    }
                    period = pos;
                } else {
                    period = 0;
                }
                resource_values.push_back(resource);
            }
        }
        // Calculate resource value at t_end using periodicity, end of evolution sequence is periodic now
        size_t tmod = t_end % period;
        // Get resource_value from the end of sequence, at tmod + k*period < t, where k is max
        size_t k = (t-tmod)/period;
        resource = resource_values[tmod+k*period-1];
        cout << "Resource value at t=" << t_end << " is " << resource << endl;
    }
}

class time_travel_device_emulator {
private:
    vector<day16_instr> program;
    vector<int> regs;
    int ip;
    size_t ip_reg;
    void apply(const day16_instr& i) {
        // Write ip to ip_reg
        regs[ip_reg] = ip;
        switch ((day16_op)i.opcode) {
            case day16_op::addr:
                regs[i.c] = regs[i.a]+regs[i.b];
                break;
            case day16_op::addi:
                regs[i.c] = regs[i.a]+i.b;
                break;
            case day16_op::mulr:
                regs[i.c] = regs[i.a]*regs[i.b];
                break;
            case day16_op::muli:
                regs[i.c] = regs[i.a]*i.b;
                break;
            case day16_op::banr:
                regs[i.c] = regs[i.a] & regs[i.b];
                break;
            case day16_op::bani:
                regs[i.c] = regs[i.a] & i.b;
                break;
            case day16_op::borr:
                regs[i.c] = regs[i.a] | regs[i.b];
                break;
            case day16_op::bori:
                regs[i.c] = regs[i.a] | i.b;
                break;
            case day16_op::setr:
                regs[i.c] = regs[i.a];
                break;
            case day16_op::seti:
                regs[i.c] = i.a;
                break;
            case day16_op::gtir:
                regs[i.c] = (i.a > regs[i.b]);
                break;
            case day16_op::gtri:
                regs[i.c] = (regs[i.a] > i.b);
                break;
            case day16_op::gtrr:
                regs[i.c] = (regs[i.a] > regs[i.b]);
                break;
            case day16_op::eqir:
                regs[i.c] = (i.a == regs[i.b]);
                break;
            case day16_op::eqri:
                regs[i.c] = (regs[i.a] == i.b);
                break;
            case day16_op::eqrr:
                regs[i.c] = (regs[i.a] == regs[i.b]);
                break;
        }
        // Read ip_reg into ip
        ip = regs[ip_reg];
    }
    string print_reg(int r, size_t idx) {
        if (r == ip_reg) {
            return ""+to_string(idx);
        } else {
            return "r["+to_string(r)+"]";
        }
    }
    void interpret_inst(const day16_instr& i, size_t x) {
        string rhs;
        bool tab = false;
        switch ((day16_op)i.opcode) {
            case day16_op::addr:
                rhs = print_reg(i.a,x) + "+" + print_reg(i.b,x);
                break;
            case day16_op::addi:
                rhs = print_reg(i.a,x) + "+" + to_string(i.b);
                break;
            case day16_op::mulr:
                rhs = print_reg(i.a,x) + "*" + print_reg(i.b,x);
                break;
            case day16_op::muli:
                rhs = print_reg(i.a,x) + "*" + to_string(i.b);
                break;
            case day16_op::banr:
                rhs = print_reg(i.a,x) + "&" + print_reg(i.b,x);
                break;
            case day16_op::bani:
                rhs = print_reg(i.a,x) + "&" + to_string(i.b);
                break;
            case day16_op::borr:
                rhs = print_reg(i.a,x) + "|" + print_reg(i.b,x);
                break;
            case day16_op::bori:
                rhs = print_reg(i.a,x) + "|" + to_string(i.b);
                break;
            case day16_op::setr:
                rhs = print_reg(i.a,x); tab = true;
                break;
            case day16_op::seti:
                rhs = to_string(i.a); tab = true;
                break;
            case day16_op::gtir:
                rhs = to_string(i.a) + ">" + print_reg(i.b,x);
                break;
            case day16_op::gtri:
                rhs = print_reg(i.a,x) + ">" + to_string(i.b);
                break;
            case day16_op::gtrr:
                rhs = print_reg(i.a,x) + ">" + print_reg(i.b,x);
                break;
            case day16_op::eqir:
                rhs = to_string(i.a) + "==" + print_reg(i.b,x);
                break;
            case day16_op::eqri:
                rhs = print_reg(i.a,x) + "==" + to_string(i.b);
                break;
            case day16_op::eqrr:
                rhs = print_reg(i.a,x) + "==" + print_reg(i.b,x);
                break;
        }
        cout << "r[" << i.c << "]=";
        cout << rhs;
        if (tab) cout << "\t";
        if (i.c == ip_reg) {
            cout << "\tGOTO: " << rhs << "+1";
        }
        cout << endl;
    }
public:
    time_travel_device_emulator(string inputfile) {
        day16_instr i;
        ip = 0;
        for (string& line : get_lines(inputfile)) {
            vector<string> p = split(line, ' ');
            if (p.size()==4) {
                // Find opcode based on name
                auto result = find(begin(day16_op_names), end(day16_op_names), p[0]);
                i.opcode = distance(begin(day16_op_names),result);
                if (i.opcode >= day16_op_names.size()) {
                    cout << "Error: invalid instruction name found while parsing program\n";
                }
                i.a = stoul(p[1]);
                i.b = stoul(p[2]);
                i.c = stoul(p[3]);
                program.push_back(i);
            } else if (p.size()==2) {
                // IP
                ip_reg = stoul(p[1]);
                cout << "Instruction pointer is bound to register " << ip_reg << endl;
            }
        }
        cout << "Program contains " << program.size() << " instructions\n";
        // Registers
        regs.resize(6);
        fill(regs.begin(), regs.end(), 0);
    }
    void run(vector<int> initial_reg_values, size_t report_rate = 10000000) {
        // Initialize registers
        regs = initial_reg_values;
        // Reset IP
        ip = 0;
        // Run program
        cout << "Running program...\n";
        bool running = true;
        size_t t = 0;
        while (running) {
            apply(program[ip]);
            ip++;
            if (ip >= program.size() || ip < 0) {
                running = false;
            }
            if (t > 0 && (t % report_rate == 0)) {
                cout << "t = " << t << ", regs: [ ";
                for (int& i : regs) { cout << i << " "; }
                cout << "]\n";
            }
            t++;
        }
    }
    void run() {
        run({0,0,0,0,0,0});
    }
    void decompile() {
        for (size_t i=0; i<program.size(); i++) {
            cout << "i:" << i << "\t";
            interpret_inst(program[i], i);
        }
    }
    vector<int> registers() {
        return regs;
    }
    int background_proc(int r0_val = 0) {
        int r0=r0_val, r1=0, r2=0, r3=0, r4=0, r5=0;
        // First part of initialisation
        r2 += 2;  // i17
        r2 *= r2; // i18
        r2 *= 19; // i19
        r2 *= 11; // i20
        r3 += 8;  // i21
        r3 *= 22; // i22
        r3 += 16; // i23
        r2 += r3; // i24, r2 = 1028
        if (r0 == 1) { // i25
            // Jump over the next GOTO, arriving to second part of init
            r3 = 27;  // i27
            r3 *= 28; // i28
            r3 += 29; // i29
            r3 *= 30; // i30
            r3 *= 14; // i31
            r3 *= 32; // i32
            r2 += r3; // i33, r2 = 1028 + 10550400 = 10551428
            r0 = 0;
        }
        // Main program
        while (r1 <= r2) { // i13-i15
            r4 = 1;
            while (r4 <= r2) { // i9-i11
                r3 = r1*r4;
                if (r3 == r2) {
                    r0 += r1; // Counting the sum divisors of r2 into r0
                }
                r4++;
            }
            r1++;
        }
        return r0;
    }
    int background_proc_opt(int r0_val = 0) {
        int r0=r0_val, r1=0, r2=0, r3=0, r4=0, r5=0;
        // First part of initialisation
        r2 += 2;  // i17
        r2 *= r2; // i18
        r2 *= 19; // i19
        r2 *= 11; // i20
        r3 += 8;  // i21
        r3 *= 22; // i22
        r3 += 16; // i23
        r2 += r3; // i24, r2 = 1028
        if (r0 == 1) { // i25
            // Jump over the next GOTO, arriving to second part of init
            r3 = 27;  // i27
            r3 *= 28; // i28
            r3 += 29; // i29
            r3 *= 30; // i30
            r3 *= 14; // i31
            r3 *= 32; // i32
            r2 += r3; // i33, r2 = 1028 + 10550400 = 10551428
            r0 = 0;
        }
        // Main program
        // Find divisors of r2 and calculate their sum into r0
        for (r1 = 1; r1 < sqrt(r2); r1++) {
            if (r2 % r1 == 0) {
                r0 += r1 + (r2/r1); // Add divisors in pairs
            }
        }
        return r0;
    }
    size_t activation_proc(size_t r0_val = 0, bool force_quit = false) {
        size_t r0 = r0_val;
        size_t r1 = 0;
        size_t r2 = 0;
        size_t r3 = 0;
        size_t r4 = 0;
        vector<size_t> r0_vals;
        do {
            r4 = r1|65536;
            r1 = 12772194;
            loop1:
            r3 = r4&255;
            r1 += r3;
            r1 = r1&16777215;
            r1 *= 65899;
            r1 = r1&16777215;
            if (256 > r4) {
                // Return to outer cycle
            } else {
                r3 = 0;
                loop2:
                r2 = r3+1;
                r2 *= 256;
                if (r2 > r4) {
                    r4 = r3;
                    goto loop1;
                } else {
                    r3++;
                    goto loop2;
                }
            }
            if (force_quit) return r1;
            else {
                if (!contains(r0_vals, r1)) {
                    r0_vals.push_back(r1);
                } else {
                    return r0_vals.back();
                }
            }
        } while (r1 != r0);
        return 0;
    }
};

void day19(string inputfile, bool partone) {
    time_travel_device_emulator device(inputfile);
    if (partone) {
        device.run();
        cout << "Register 0 is " << device.registers()[0] << endl;
        cout << "Checking with compiled version: " << device.background_proc(0) << endl;
        cout << "Checking with optimized version: " << device.background_proc_opt(0) << endl;
    } else {
        // Interpret program instructions, does not unfold goto-s into while loops
        //device.decompile();
        // This is terribly slow
        //device.run({1,0,0,0,0,0}, 10000000);
        // Even this is too slow
        //cout << "Trying with compiled version: " << device.background_proc(1) << endl;
        cout << "Running optimized background process: " << device.background_proc_opt(1) << endl;
    }
}

class day20_map {
public:
    enum class direction {
        N, // +y
        W, // -x
        S, // -y
        E  // +x
    };
    class node { // To represent rooms
    public:
        pair<size_t, size_t> coord;
        bool doors[4]; // to represent edges, in NWSE order
        node(pair<size_t, size_t> coordinate) : coord(coordinate) {
            doors[0] = doors[1] = doors[2] = doors[3] = false; // No doors by default
        }
        node() {
            coord = {0,0};
            doors[0] = doors[1] = doors[2] = doors[3] = false; // No doors by default
        }
        bool operator==(const node& other) {
            return (coord == other.coord &&
                    doors[0] == other.doors[0] &&
                    doors[1] == other.doors[1] &&
                    doors[2] == other.doors[2] &&
                    doors[3] == other.doors[3]);
        }
    };
    vector<vector<node>> graph;
    size_t W, H;
    size_t xmin, xmax, ymin, ymax;
    string regex;
    size_t regex_ptr;
    bool bfs_print;
    pair<size_t, size_t> origin;
    // Rooms = nodes
    // Doors = edges
    // Walls won't be modelled
    node& get_node(pair<size_t, size_t> pos) {
        return graph[pos.second][pos.first];
    }
    direction char_to_direction(const char c, bool opposite=false) {
        switch (c) {
            case 'N':
                return opposite ? direction::S : direction::N;
            case 'W':
                return opposite ? direction::E : direction::W;
            case 'S':
                return opposite ? direction::N : direction::S;
            case 'E':
                return opposite ? direction::W : direction::E;
            default:
                return direction::N;
        }
    }
    void step(pair<size_t, size_t>& pos, const char c) {
        switch (c) {
            case 'N':
                pos.second++;
                break;
            case 'W':
                pos.first--;
                break;
            case 'S':
                pos.second--;
                break;
            case 'E':
                pos.first++;
                break;
        }
    }
    vector<pair<size_t, size_t>> build_graph(size_t& depth, vector<pair<size_t, size_t>> front, size_t &c_ptr) {  // Breadth-first approach
        bool tracking = true;
        vector<pair<size_t, size_t>> start_front=front;
        vector<pair<size_t, size_t>> result;
        while (tracking) {
            char c = regex[c_ptr];
            switch (c) {
                case 'N':
                case 'S':
                case 'E':
                case 'W':
                    for (pair<size_t, size_t> &pos : front) {
                        get_node(pos).doors[(size_t) char_to_direction(c)] = true; // Forward edge
                        step(pos, c);
                        get_node(pos).doors[(size_t) char_to_direction(c, true)] = true; // Backward edge
                        minmax(pos.first, xmin, xmax);
                        minmax(pos.second, ymin, ymax);
                        //cout << regex.substr(0, c_ptr+1) << endl;
                        //print(pos);
                    }
                    c_ptr++;
                    break;
                case '(':
                    // Branch off, recurse deeper
                    depth++;
                    c_ptr++;
                    result = build_graph(depth, front, c_ptr);
                    break;
                case ')':
                    // Save current front to result
                    for (pair<size_t, size_t> pos : front) { result.push_back(pos); }
                    //cout << regex.substr(0, c_ptr+1) << endl;
                    // Done with branching, backtrack
                    depth--;
                    c_ptr++;
                    tracking = false;
                    break;
                case '|':
                    // New option
                    c_ptr++;
                    for (pair<size_t, size_t> pos : front) { result.push_back(pos); }
                    front = start_front;
                    break;
                case '$':
                    result = front;
                    //cout << regex.substr(0, c_ptr+1) << endl;
                    tracking = false;
                    break;
            }
        }
        return result;
    }
    day20_map(const string& regex_str, bool print_map = false) : regex(regex_str) {
        // Count movements
        size_t w = count(regex.begin(), regex.end(), 'W');
        size_t e = count(regex.begin(), regex.end(), 'E');
        size_t n = count(regex.begin(), regex.end(), 'N');
        size_t s = count(regex.begin(), regex.end(), 'S');
        W = w+e+3;
        H = n+s+3;
        origin = {w, s};
        xmin = xmax = origin.first;
        ymin = ymax = origin.second;
        // Pre-allocate graph
        cout << "Allocating map WxH:" << W << "x" << H << endl;
        for (size_t y=0; y<H; y++) {
            vector<node> row;
            for (size_t x=0; x<W; x++) {
                row.push_back(node(pair<int,int>(x,y)));
            }
            graph.push_back(row);
        }
        // Parse regex
        if (regex[0] != '^') {
            cout << "Error: invalid regex format!";
        } else {
            size_t depth = 0;
            vector<pair<size_t, size_t>> front;
            front.push_back(origin);
            regex_ptr = 1;
            build_graph(depth, front, regex_ptr);
        }
        if (print_map) print(origin);
    }
    size_t bfs_count_steps(bool print = false, size_t min_steps = 0) {
        vector<node> front;
        front.push_back(get_node(origin));
        vector<node> visited, far_rooms;
        visited.push_back(get_node(origin));
        size_t steps = 0;
        bfs_print = print;
        bfs(front, visited, steps, min_steps, far_rooms);
        if (min_steps == 0) {
            return steps;
        } else {
            return far_rooms.size();
        }
    }
    void print(pair<int,int> loc, vector<node> front = {}, vector<node> visited = {}) {
        string row1;
        string row2;
        for (size_t x = xmin; x <= xmax; x++) {
            cout << "##";
        }
        cout << "#\n";
        for (size_t y = ymax; y >= ymin; y--) {
            row1 = "#";
            row2 = "#";
            for (size_t x = xmin; x <= xmax; x++) {
                node n = get_node({x, y});
                if (contains(front, n)) {
                    row1 += "*";
                } else if (contains(visited, n)) {
                    row1 += "v";
                } else if (loc.first == x && loc.second == y) {
                    row1 += "X";
                } else if (origin.first == x && origin.second == y) {
                    row1 += "o";
                } else {
                    row1 += ".";
                }
                if (n.doors[(size_t)direction::E]) {row1 += "|";} else { row1 += "#";}
                if (n.doors[(size_t)direction::S]) {row2 += "-";} else { row2 += "#";}
                row2 += "#";
            }
            row1 += "\n";
            row2 += "\n";
            cout << row1;
            cout << row2;
        }
        cout << endl;
    }
    bool bfs(vector<node>& front, vector<node>& visited, size_t& steps, size_t min_steps, vector<node>& far_rooms) {
        // For every element in the frontline nodes, enumerate compatible children
        node t, neighbour;
        vector<node> new_front;
        for (size_t i=0; i<front.size(); i++) {
            t = front[i];
            for (size_t d=0; d<4; d++) {
                if (t.doors[d]) {
                    // This edge is open
                    switch((direction)d) {
                        case direction::N:
                            neighbour = get_node({t.coord.first,   t.coord.second+1});
                            break;
                        case direction::W:
                            neighbour = get_node({t.coord.first-1, t.coord.second});
                            break;
                        case direction::S:
                            neighbour = get_node({t.coord.first,   t.coord.second-1});
                            break;
                        case direction::E:
                            neighbour = get_node({t.coord.first+1, t.coord.second});
                            break;
                    }
                    if (!contains(visited, neighbour)) {
                        new_front.push_back(neighbour);
                        visited.push_back(neighbour);
                        if (min_steps > 0 && steps >= min_steps-1) {
                            if (!contains(far_rooms, neighbour)) {
                                far_rooms.push_back(neighbour);
                            }
                        }
                    }
                } // End if edge exists
            } // End for all edges
        }
        front = new_front;
        if (front.empty()) return false;
        steps++;
        //cout << "Steps: " << steps << ", visited: " << visited.size() << ", front: " << front.size() << endl;
        if (bfs_print) print(t.coord, front, visited);
        return bfs(front, visited, steps, min_steps, far_rooms);
    }
};

bool operator==(const day20_map::node& lhs, const day20_map::node& rhs) {
    return (lhs.coord == rhs.coord &&
            lhs.doors[0] == rhs.doors[0] &&
            lhs.doors[1] == rhs.doors[1] &&
            lhs.doors[2] == rhs.doors[2] &&
            lhs.doors[3] == rhs.doors[3]);
}

void day20(string inputfile, bool partone) {
    string regex = get_lines(inputfile)[0];
    day20_map map(regex);
    if (partone) {
        size_t steps = map.bfs_count_steps();
        cout << "Max steps to reach the furthest room (i.e. walk through the whole facility): " << steps << endl;
    } else {
        size_t rooms = map.bfs_count_steps(false, 1000);
        cout << "Rooms with at least 1000 doors away: " << rooms << endl;
    }
}

void day21(string inputfile, bool partone) {
    time_travel_device_emulator device(inputfile);
    if (partone) {
        //device.decompile();
        // Only for debug mode
        //device.run();
        // Run the activation process with force_quit
        cout << "Activation halts fastest with r0 = " << device.activation_proc(0, true) << endl; // Passed!
    } else {
        cout << "Activation halts slowest with r0 = " << device.activation_proc(0, false) << endl;
    }
}