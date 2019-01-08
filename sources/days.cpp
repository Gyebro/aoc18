#include "config.h"
#include "days.h"
#include "common.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

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
        moved
    };
    bool graph_bfs(tile* target_ptr, const size_t max_steps, vector<tile*>& front, vector<vector<tile*>>& paths) {
        // For every element in the frontline nodes, enumerate compatible children
        if (front.size() == 0) return false;
        cout << "BFS: frontline nodes: " << front.size() << endl;
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
        bool target_goblins = !u.goblin; // Goblins target Elves and vice versa
        vector<unit> targets;
        for (unit& t : units) {
            if (t.goblin == target_goblins && u.tile_ptr != t.tile_ptr) targets.push_back(t);
        }
        if (targets.empty()) {
            return move_result::no_targets_left;
        } else {
            vector<size_t> distances_to_targets;
            vector<tile*> shortest_path;
            vector<tile*> path;
            size_t lowest_distance = W*H;
            size_t dist;
            for (unit& t : targets) {
                vector<size_t> visited;
                bool reachable = false;// TODO graph_bfs(u.tile_ptr, t.tile_ptr, lowest_distance, path);
                if (reachable) {
                    dist = path.size();
                    if (dist < lowest_distance) {
                        shortest_path = path;
                        lowest_distance = dist;
                    }
                }
            }
            // If theres a unit in range, return not_moved_can_attack
            // Do BFS towards all enemy, find who is reachable
            // Move towards the closest (if exists)

        }
        return move_result::moved;
    }
    void attack(unit& u) {
        // TODO: Implement
        // Find closest unit with least hp or with highest priority in range (4 neighbouring tiles)
        // Attack it, -3 hp
        // Remove unit if it dies, clean the tile's pointer too!
        return;
    }
    bool round() {
        // Sort units based on their tile's priority
        sort(units.begin(), units.end(), [](auto &l, auto &r) { return l.tile_ptr->priority < r.tile_ptr->priority; });
        // For every unit
        for (unit& u : units) {
            // Move with units
            switch (move(u)) {
                case move_result::no_targets_left:
                    // End battle
                    return false;
                case move_result::moved:
                case move_result::not_moved_enemy_in_range:
                    attack(u);
                    break;
                case move_result::not_moved_enemy_inaccessibly:
                    // Do nothing
                    break;
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
        cout << "Built graph consisting " << nodes << " nodes\n";
        cout << "Units on battlefield: " << units.size() << endl;
        // Unit summary
        for (unit& u : units) {
            cout << (u.goblin ? "Goblin" : "Elf");
            cout << " at " << u.tile_ptr->priority << " @ " << u.tile_ptr->x << "," << u.tile_ptr->y;
            cout << endl;
        }
        // Test BFS
        tile* start =  &(graph[6*W+24]);
        //tile* target = &(graph[17*W+23]);
        //tile* target = &(graph[8*W+17]);
        tile* target = &(graph[13*W+3]);
        vector<tile*> front;
        vector<vector<tile*>> paths;
        front.push_back(start);
        paths.push_back(front);
        bool reach = graph_bfs(target, W*H, front, paths);
        if (reach) { cout << "Reachable in " << paths[0].size() << " steps\n";
            for (tile* t : paths[0]) {
                cout << t->y << "," << t->x << endl;
            }
        } else {
            cout << "Not reachable" << endl;
        }
    }
    size_t start() {
        size_t rounds = 0;
        while (round()) rounds++; // not counting the last round when the battle ends
        // Calculate the sum of HPs of alive units
        size_t hp_sum = 0;
        for (unit& u : units) {
            if (u.alive) hp_sum += u.hp;
        }
        return rounds*hp_sum;
    }
};



void day15(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    day15_battle battle(lines);
    //cout << "Outcome: " << battle.start() << endl;
    return;
}