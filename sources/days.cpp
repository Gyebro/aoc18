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