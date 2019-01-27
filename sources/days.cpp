#include "config.h"
#include "days.h"
#include "common.h"

#include <vector>
#include <queue>
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

class day04_record {
public:
    enum class type {
        begins,
        wakes_up,
        falls_asleep
    };
    size_t Y, M, D;
    size_t h, m;
    string note;
    day04_record() {
        Y=M=D=h=m=0;
        note="empty";
    }
    day04_record(const string& line) {
        // Format: [YYYY-MM-DD hh:mm] NOTE
        vector<string> words;
        words = split(line, ']');
        note = words[1].substr(1);
        Y = stoul(words[0].substr(1,4));
        M = stoul(words[0].substr(6,2));
        D = stoul(words[0].substr(9,2));
        h = stoul(words[0].substr(12,2));
        m = stoul(words[0].substr(15,2));
    }
    size_t get_min() const {
        return m + h*60 + D*24*60 + M*31*24*60 + Y*366*31*24*60;
    }
    size_t get_guard_id() const {
        vector<string> words = split(note, '#');
        if (words.empty()) {
            return -1;
        } else {
            return stoul(split(words[1],' ')[0]);
        }
    }
    type get_type() const {
        vector<string> words = split(note, ' ');
        string keyw = "wakes";
        if (contains(words, keyw )) { return type::wakes_up; }
        keyw = "falls";
        if (contains(words, keyw )) { return type::falls_asleep; }
        keyw = "begins";
        if (contains(words, keyw )) { return type::begins; }
        else {
            cout << "Error, invalid note!\n"; return type::wakes_up;
        }
    }
};

bool operator< (const day04_record& l, const day04_record& r) {
    return l.get_min() < r.get_min();
}

void day04(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    vector<day04_record> records;
    for (const string& line : lines) {
        records.push_back(day04_record(line));
    }
    cout << "Found " << records.size() << " records\n";
    sort(records.begin(), records.end());
    cout << "Sorted in chronological order!\n";
    size_t ID = 0;
    vector<pair<size_t, vector<size_t>>> guard_table;
    size_t gt_id = 0;
    vector<size_t> empty_minute_table;
    empty_minute_table.resize(60);
    fill(empty_minute_table.begin(), empty_minute_table.end(), 0);
    size_t i=0;
    bool collecting = true;
    while (collecting) {
        if (i == records.size()) { break; }
        if (records[i].get_type() != day04_record::type::begins) {
            cout << "Error! Invalid record met, expected 'guard begins'\n";
            break;
        }
        ID = records[i].get_guard_id();
        // Check if this guard already has a minute table
        if (guard_table.empty()) {
            guard_table.push_back({ID, empty_minute_table});
        }
        bool guard_exists = false;
        for (size_t m = 0; m < guard_table.size(); m++) {
            if (guard_table[m].first == ID) {
                gt_id = m;
                guard_exists = true;
            }
        }
        if (!guard_exists) {
            // Insert an empty minute table for this guard
            guard_table.push_back({ID, empty_minute_table});
            gt_id = guard_table.size() - 1;
        }
        // Loop on the records until the next guard comes in or file ends
        size_t asleep_at = 0;
        size_t wakes_up = 0;
        bool processing = true;
        bool interval_open = false;
        while (processing) {
            i++;
            if (i == records.size()) {
                collecting = false;
                break;
            }
            switch (records[i].get_type()) {
                case day04_record::type::falls_asleep:
                    asleep_at = records[i].m;
                    interval_open = true;
                    break;
                case day04_record::type::wakes_up:
                    wakes_up = records[i].m;
                    interval_open = false;
                    // Mark current span as asleep in minute_table
                    for (size_t m = asleep_at; m < wakes_up; m++) {
                        guard_table[gt_id].second[m]++;
                    }
                    break;
                case day04_record::type::begins:
                    // Finish previous interval if it wasn't closed
                    if (interval_open) {
                        cout << "Warning: Guard " << ID << " fell asleep at " << asleep_at << " but didn't wake up!";
                        for (size_t m = asleep_at; m < 60; m++) {
                            guard_table[gt_id].second[m]++;
                        }
                        interval_open = false;
                    }
                    processing = false;
                    break;
            }
        }
    }
    size_t most_minutes = 0;     // Most minutes and its corresponding guard ID
    size_t most_minutes_ID = 0;
    size_t minutes_sum;
    size_t out_best_minute, out_best_minute_amt; // Best minute and the amount of sleepy time in it
    for (const pair<size_t, vector<size_t> >& gt : guard_table) {
        minutes_sum = 0;
        size_t best_minute = 0;
        size_t best_minute_amt = 0;
        for (size_t m=0; m<60; m++) {
            minutes_sum += gt.second[m]; // Accumulate sum of guard
            if (gt.second[m] > best_minute_amt) {
                best_minute = m;
                best_minute_amt = gt.second[m];
            }
        }
        if (partone) {
            if (minutes_sum > most_minutes) {
                most_minutes = minutes_sum;
                most_minutes_ID = gt.first;
                out_best_minute = best_minute;
                out_best_minute_amt = best_minute_amt;
            }
        } else {
            if (best_minute_amt > most_minutes) {
                most_minutes = best_minute_amt;
                most_minutes_ID = gt.first;
                out_best_minute = best_minute;
                out_best_minute_amt = best_minute_amt;
            }
        }

    }
    if (partone) {
        cout << "The guard with the most minutes asleep is ID: " << most_minutes_ID << " (mins: " << most_minutes << ")\n";
        cout << "The best minute of his schedule is " << out_best_minute << " (occasions: " << out_best_minute_amt << ")\n";
        cout << "Result: " << most_minutes_ID*out_best_minute << endl;
    } else {
        cout << "The guard with the most probable sleepy minute is ID: " << most_minutes_ID << "\n";
        cout << "The best minute of his schedule is " << out_best_minute << " (occasions: " << out_best_minute_amt << ")\n";
        cout << "Result: " << most_minutes_ID*out_best_minute << endl;
    }
}

bool day05_react(char a, char b) {
    if (a > b) {
        return (a-b)==32;
    } else {
        return (b-a)==32;
    }
}

size_t day05_react_polymer(string& polymer) {
    size_t p = 0;
    while (p < polymer.size()-1) {
        // Check chars at p & p+1
        if (day05_react(polymer[p], polymer[p+1])) {
            // Keep the first (p-1) chars and chars from (p+2)
            string left="", right="";
            if (p > 0) left=polymer.substr(0, p);
            if (p < polymer.size()-2) right=polymer.substr(p+2);
            polymer = left + right;
            if (p > 0) p--;
        } else {
            p++;
        }
    }
    return polymer.size();
}

void day05(string inputfile, bool partone) {
    string polymer = get_lines(inputfile)[0];
    cout << "Polymer has initial length: " << polymer.size() << endl;
    if (partone) {
        day05_react_polymer(polymer);
        cout << "Fully reacted polymer has length: " << polymer.size() << endl;
    } else {
        size_t shortest = polymer.length();
        char problematic_type='A';
        for (char type='A'; type<='Z'; type++) {
            string polymer_copy = polymer;
            // Remove the uppercase variant
            polymer_copy.erase(remove(polymer_copy.begin(), polymer_copy.end(), type), polymer_copy.end());
            // Remove the lowercase variant
            polymer_copy.erase(remove(polymer_copy.begin(), polymer_copy.end(), tolower(type)), polymer_copy.end());
            size_t length = day05_react_polymer(polymer_copy);
            if (length < shortest) {
                shortest = length;
                problematic_type = type;
            }
        }
        cout << "Shortest length " << shortest << " can be achieved by removing " << problematic_type << endl;
    }
}

void day06(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    vector<pair<size_t, size_t>> coords;
    vector<string> words;
    size_t xmin = 10000, xmax = 0;
    size_t ymin = 10000, ymax = 0;
    for (string& line : lines) {
        words = split(line, ',');
        if (words.size() == 2) {
            coords.push_back({stoul(words[0]), stoul(words[1])});
            minmax(coords.back().first, xmin, xmax);
            minmax(coords.back().second, ymin, ymax);
        }
    }
    cout << "Found " << coords.size() << " coordinates\n";
    cout << "Area bounds: X=(" << xmin << "," << xmax << "), Y=(" << ymin << "," << ymax << ")\n";
    // Create a map, padding bounds by 1 extra unit along x and y
    xmin--;
    xmax++;
    ymin--;
    ymax++;
    size_t invalid_key = coords.size();
    vector<vector<size_t>> map;
    vector<size_t> row;
    row.resize(xmax - xmin + 1);
    fill(row.begin(), row.end(), invalid_key);
    map.resize(ymax - ymin + 1);
    fill(map.begin(), map.end(), row);
    size_t x, y;
    if (partone) {
        // Tag map cells with the closest coordinate's index
        size_t lowest_dist, dist;
        size_t closest_coord = invalid_key;
        bool has_closest;
        for (size_t j = 0; j < map.size(); j++) {
            for (size_t i = 0; i < row.size(); i++) {
                x = xmin + i;
                y = ymin + j;
                // Find closest coordinate to (x,y)
                lowest_dist = 2 * (xmax + ymax); // Large value
                has_closest = false;
                for (size_t c = 0; c < coords.size(); c++) {
                    dist = abs((int) coords[c].first - (int) x) +
                           (size_t) abs((int) coords[c].second - (int) y); // 2D Manhattan dist
                    if (dist < lowest_dist) {
                        closest_coord = c;
                        lowest_dist = dist;
                        has_closest = true;
                    }
                }
                // Check for tie with another coord
                for (size_t c = 0; c < coords.size(); c++) {
                    dist = abs((int) coords[c].first - (int) x) +
                           (size_t) abs((int) coords[c].second - (int) y); // 2D Manhattan dist
                    if (dist == lowest_dist && c != closest_coord) {
                        // Another coord found with the lowest distance, invalidate has_closest
                        has_closest = false;
                    }
                }
                if (has_closest) {
                    map[j][i] = closest_coord;
                }
            }
        }
        // Map is now marked with the index of closest coord or 'invalid_key' meaning ties
        vector<pair<size_t, bool> > area_counts; // 'count' and 'is_infinite'
        area_counts.resize(coords.size());
        fill(area_counts.begin(), area_counts.end(), pair<size_t, bool>(0, false));
        for (size_t j = 0; j < map.size(); j++) {
            for (size_t i = 0; i < row.size(); i++) {
                if (map[j][i] < invalid_key) {
                    // Set infinite flag on edges
                    if (i == 0 || j == 0 || i == map.size() - 1 || j == row.size() - 1) {
                        area_counts[map[j][i]].second = true; // This area is infinite
                    }
                    // Add +1 area to the id
                    area_counts[map[j][i]].first++;
                }
            }
        }
        // Sort area_counts in descending order, non-infinite first
        sort(area_counts.begin(), area_counts.end(),
             [](const pair<size_t, bool> &l, const pair<size_t, bool> &r) {
                 if (l.second == r.second) {
                     return l.first > r.first;
                 } else {
                     return !l.second;
                 }
             });
        cout << "Largest non-infinite area is " << area_counts[0].first << " units.\n";
    } else {
        // Part two: tag map cells if they are within D distance to every coord
        size_t D = 10000;
        size_t dist;
        size_t valid_key = 0;
        for (size_t j = 0; j < map.size(); j++) {
            for (size_t i = 0; i < row.size(); i++) {
                x = xmin + i;
                y = ymin + j;
                // Check if every coord is within D to coordinate to (x,y)
                bool ok = true;
                dist = 0;
                for (const pair<size_t, size_t>& coord : coords) {
                    dist += abs((int)coord.first - (int)x) + abs((int)coord.second - (int)y);
                    if (dist >= D) {
                        ok = false;
                        break;
                    }
                }
                // If ok flag is still on, tag this map cell
                if (ok) {
                    map[j][i]=valid_key;
                }
            }
        }
        // Count valid locations
        size_t valid_area = 0;
        for (size_t j = 0; j < map.size(); j++) {
            for (size_t i = 0; i < row.size(); i++) {
                if (map[j][i] == valid_key) { valid_area++; }
            }
        }
        cout << "Area with less than D units to every coordinate: " << valid_area << endl;
    }
}





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

class day22_cave {
public:
    enum class type {
        rocky,
        wet,
        narrow
    };
    enum class tool {
        none,
        torch,
        climbing_gear
    };
    class tile {
    public:
        type region_type;
        size_t geologic_index;
        size_t erosion_level;
    };
    struct coordinate {
        size_t t; // Tool {0,1 or 2} ~ layers of the map
        size_t x, y; // Spatial coordinates
    };
    size_t d, tx, ty;
    size_t w, h;
    vector<vector<tile>> map;
    vector<coordinate> neighbours(size_t t, size_t x, size_t y) {
        return neighbours({t, x, y});
    }
    vector<coordinate> neighbours(coordinate c) {
        // Get tile from the map
        tile t = map[c.y][c.x];
        // Moving to a *rocky* region requires being in layer {torch or climbing_gear}
        // Moving to a *wet*   region requires being in layer {none  or climbing_gear}
        // Moving to a *narrow region requires being in layer {none  or torch}
        vector<coordinate> n;
        // Error checking
        if ((size_t)t.region_type == c.t) {
            cout << "Error: you are here with an incompatible tool!\n";
            switch (t.region_type) {
                case type::rocky:
                    if (c.t == (size_t)tool::none) cout << "Error: you can't be in a rocky region with 'none' tool\n";
                    break;
                case type::wet:
                    if (c.t == (size_t)tool::torch) cout << "Error: you can't be in a wet region with 'torch' tool\n";
                    break;
                case type::narrow:
                    if (c.t == (size_t)tool::climbing_gear) cout << "Error: you can't be in a narrow region with 'climbing gear' tool\n";
                    break;
            }
        }
        // Add up to 6 neighbours
        // Neighbour to other tool
        coordinate nc;
        nc.x = c.x;
        nc.y = c.y;
        switch (t.region_type) {
            case type::rocky:
                nc.t = (c.t == (size_t)tool::climbing_gear) ? (size_t)tool::torch : (size_t)tool::climbing_gear;
                break;
            case type::wet:
                nc.t = (c.t == (size_t)tool::climbing_gear) ? (size_t)tool::none : (size_t)tool::climbing_gear;
                break;
            case type::narrow:
                nc.t = (c.t == (size_t)tool::none) ? (size_t)tool::torch : (size_t)tool::none;
                break;
        }
        n.push_back(nc);
        // Left neighbour
        if (c.x > 0) {
            nc.x = c.x-1; nc.y = c.y; nc.t = c.t;
            if ((size_t)map[nc.y][nc.x].region_type != (size_t)nc.t) { n.push_back(nc); } // Only add if tool is compatible
        }
        // Top neighbour
        if (c.y > 0) {
            nc.x = c.x; nc.y = c.y-1; nc.t = c.t;
            if ((size_t)map[nc.y][nc.x].region_type != (size_t)nc.t) { n.push_back(nc); }
        }
        // Right neighbour
        if (c.x < w) {
            nc.x = c.x+1; nc.y = c.y; nc.t = c.t;
            if ((size_t)map[nc.y][nc.x].region_type != (size_t)nc.t) { n.push_back(nc); }
        }
        // Bottom neighbour
        if (c.y < h) {
            nc.x = c.x; nc.y = c.y+1; nc.t = c.t;
            if ((size_t)map[nc.y][nc.x].region_type != (size_t)nc.t) { n.push_back(nc); }
        }
        // Return up to 5 neighbours (1 tool change, max 4 spatial movement)
        return n;
    }
    day22_cave(size_t depth, size_t W, size_t H, size_t target_x, size_t target_y) : d(depth), tx(target_x), ty(target_y), w(W), h(H) {
        // Generate cave up to W and H
        map.reserve(H+1);
        size_t gi, el;
        type t;
        tile T;
        for (size_t y=0; y<=H; y++) {
            vector<tile> row;
            row.reserve(W+1);
            for (size_t x=0; x<=W; x++) {
                // Determine geologic index
                if (x==0 && y==0) {
                    gi = 0;
                } else if (x==tx && y==ty) {
                    gi = 0;
                } else if (y==0) {
                    gi = x*16807;
                } else if (x==0) {
                    gi = y*48271;
                } else {
                    // geologic index is the result of multiplying the erosion levels of the regions at X-1,Y and X,Y-1
                    gi = row[x-1].erosion_level * map[y-1][x].erosion_level;
                }
                // erosion level is its geologic index plus the cave system's depth, all modulo 20183.
                el = (gi + d) % 20183;
                // Then el % 3 -> {rocky, wet, narrow}
                t = (type)(el % 3);
                // Save region
                T.geologic_index = gi;
                T.erosion_level = el;
                T.region_type = t;
                row.push_back(T);
            }
            map.push_back(row);
        }
    }
    size_t risk_level() {
        size_t risk = 0;
        for (size_t y = 0; y <= ty; y++) {
            for (size_t x = 0; x <= tx; x++) {
                risk += (size_t)(map[y][x].region_type);
            }
        }
        return risk;
    }
    class node {
    public:
        size_t distance;
        coordinate coord;
        node() {
            distance = 1000000000;
            coord = {0,0,0};
        }
        node(size_t dist, coordinate c) : distance(dist), coord(c) {}
    };
    class node_meta {
    public:
        bool visited;
        size_t distance;
        node_meta () {
            visited = false;
            distance = 1000000000;
        }
    };
    size_t distance(const coordinate c1, const coordinate c2) {
        if (c1.t == c2.t) {
            return 1;
        } else {
            return 7;
        }
    }
    size_t rescue_target() {
        // Setup node metadata
        vector<vector<vector<node_meta>>> meta;
        vector<node_meta> row;
        row.resize(w+1);
        vector<vector<node_meta>> layer;
        layer.resize(h+1);
        fill(layer.begin(), layer.end(), row);
        meta.push_back(layer); // None
        meta.push_back(layer); // Torch
        meta.push_back(layer); // Climbing gear
        // This is a weighted graph, all layers are connected, moving between them takes 7 minutes
        // Start is {torch,0,0}, Target is {torch,tx,ty}
        coordinate start =  {(size_t)tool::torch, 0, 0};
        coordinate target = {(size_t)tool::torch, tx, ty};
        meta[start.t][start.y][start.x].distance=0;
        // Create a queue for Dijkstra's algorithm
        auto compare = [](const node& L, const node& R) { return L.distance > R.distance;};
        priority_queue<node, vector<node>, decltype(compare)> Q(compare);
        node n;
        cout << "Setting up queue with " << h*w*3 << " elements... ";
        for (size_t y=0; y<=h; y++) {
            for (size_t x=0; x<=w; x++) {
                for (size_t t=0; t<3; t++) {
                    if (x == start.x && y == start.y && t == start.t) {
                        n.distance = 0;
                    } else {
                        n.distance = 1000000000;
                    }
                    n.coord = {t, x, y};
                    if ((size_t)map[y][x].region_type != t) { // Only push valid nodes!
                        Q.push(n);
                    }
                }
            }
        }
        cout << " Done!\n";
        // Dijkstra with lazy deletion
        while (!Q.empty()) {
            // Extract and remove node with minimum distance
            node n = Q.top();
            Q.pop();
            if (meta[n.coord.t][n.coord.y][n.coord.x].visited) {
                // This is a copy of a node with higher distance, lazy deletion by ignoring this element
            } else {
                // Terminate if we arrive
                if (n.coord.x == target.x && n.coord.y == target.y && n.coord.t == target.t) {
                    return meta[target.t][target.y][target.x].distance;
                }
                // Set the current node visited
                meta[n.coord.t][n.coord.y][n.coord.x].visited = true;
                // For every neighbour of n
                for (coordinate &c : neighbours(n.coord)) {
                    if (!meta[c.t][c.y][c.x].visited) {
                        size_t new_dist = n.distance + distance(c, n.coord);
                        if (new_dist < meta[c.t][c.y][c.x].distance) {
                            meta[c.t][c.y][c.x].distance = new_dist;
                            // Instead of decrease-priority, push a copy with lower distance
                            Q.push(node(new_dist, c));
                        }
                    }
                }
            }
        }
        cout << "No route to target found!\n";
        return meta[target.t][target.y][target.x].distance;
    }
};

void day22(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    size_t depth = stoul(split(lines[0],':')[1]);
    vector<string> coords = split(split(lines[1],':')[1],',');
    size_t x = stoul(coords[0]), y = stoul(coords[1]);
    cout << "Generating cave with depth = " << depth << endl;
    cout << "Target is at (" << x << "," << y << ")\n";
    if (partone) {
        day22_cave cave(depth, x, y, x, y);
        cout << "Risk level of cave: " << cave.risk_level() << endl;
    } else {
        day22_cave cave(depth, x+50, y+50, x, y);
        size_t shortest = cave.rescue_target();
        cout << "Shortest path to the target (in minutes): " << shortest << endl;
    }
}

class nanobot {
public:
    class coord {
    public:
        int x,y,z;
        coord(const int x, const int y, const int z) : x(x), y(y), z(z) {}
        coord() { coord(0,0,0); }
    };
    coord c;
    size_t r;
    nanobot(const int x, const int y, const int z, const size_t r) : c(x,y,z), r(r) {}
    size_t dist(const nanobot& nb) const {
        return abs(c.x-nb.c.x)+abs(c.y-nb.c.y)+abs(c.z-nb.c.z);
    }
    size_t dist(const coord p) const {
        return abs(c.x-p.x)+abs(c.y-p.y)+abs(c.z-p.z);
    }
    bool is_in_range(const nanobot& nb) const {
        return (dist(nb) <= r);
    }
    bool is_in_range(const coord p) const {
        return (dist(p) <= r);
    }
};

bool day23_box_search(const vector<nanobot>& nanobots, nanobot::coord& c0, int r, size_t current_max, size_t& new_max) {
    //cout << "Box search at (" << c0.x << "," << c0.y << "," << c0.z << ") with r=" << r << endl;
    vector<pair<nanobot::coord, size_t>> box;
    // Set up the box corners
    box.push_back({c0, current_max});
    box.push_back({{c0.x+r, c0.y, c0.z}, 0});
    box.push_back({{c0.x+r, c0.y, c0.z+r}, 0});
    box.push_back({{c0.x+r, c0.y, c0.z-r}, 0});
    box.push_back({{c0.x+r, c0.y+r, c0.z}, 0});
    box.push_back({{c0.x+r, c0.y+r, c0.z+r}, 0});
    box.push_back({{c0.x+r, c0.y+r, c0.z-r}, 0});
    box.push_back({{c0.x+r, c0.y-r, c0.z}, 0});
    box.push_back({{c0.x+r, c0.y-r, c0.z+r}, 0});
    box.push_back({{c0.x+r, c0.y-r, c0.z-r}, 0});

    box.push_back({{c0.x-r, c0.y, c0.z}, 0});
    box.push_back({{c0.x-r, c0.y, c0.z+r}, 0});
    box.push_back({{c0.x-r, c0.y, c0.z-r}, 0});
    box.push_back({{c0.x-r, c0.y+r, c0.z}, 0});
    box.push_back({{c0.x-r, c0.y+r, c0.z+r}, 0});
    box.push_back({{c0.x-r, c0.y+r, c0.z-r}, 0});
    box.push_back({{c0.x-r, c0.y-r, c0.z}, 0});
    box.push_back({{c0.x-r, c0.y-r, c0.z+r}, 0});
    box.push_back({{c0.x-r, c0.y-r, c0.z-r}, 0});

    box.push_back({{c0.x, c0.y, c0.z+r}, 0});
    box.push_back({{c0.x, c0.y, c0.z-r}, 0});
    box.push_back({{c0.x, c0.y+r, c0.z}, 0});
    box.push_back({{c0.x, c0.y+r, c0.z+r}, 0});
    box.push_back({{c0.x, c0.y+r, c0.z-r}, 0});
    box.push_back({{c0.x, c0.y-r, c0.z}, 0});
    box.push_back({{c0.x, c0.y-r, c0.z+r}, 0});
    box.push_back({{c0.x, c0.y-r, c0.z-r}, 0});
    // Calculate nanobot counts for locations
    for (size_t b=1; b<box.size(); b++) {
        size_t bots_in_range = 0;
        for (size_t i=0; i<nanobots.size(); i++) {
            if (nanobots[i].is_in_range(box[b].first)) {
                bots_in_range++;
            }
        }
        box[b].second = bots_in_range;
    }
    // Sort boxes based on bots in range and break ties with manhattan distance to {0,0,0}
    sort(box.begin(), box.end(),
         [](const pair<nanobot::coord, size_t>& l, const pair<nanobot::coord, size_t>& r) {
        if (l.second == r.second) {
            size_t dl = abs(l.first.x)+abs(l.first.y)+abs(l.first.z);
            size_t dr = abs(r.first.x)+abs(r.first.y)+abs(r.first.z);
            return dl < dr;
        } else {
            return l.second > r.second;
        }
    });
    new_max = box[0].second;
    c0 = box[0].first;
    return (new_max > current_max);
}

void day23_find_local_opt(const vector<nanobot>& nanobots, const nanobot::coord c, size_t in_range) {
    bool converged = false;
    size_t radius = 512*1048576;
    nanobot::coord c0=c;
    size_t new_max_bots;
    size_t max_bots=in_range;
    while (!converged) {
        if (!day23_box_search(nanobots, c0, radius, max_bots, new_max_bots)) {
            // Terminate if needed
            if (radius == 1) converged = true;
            // Reduce radius
            radius = radius/2;
        }
        max_bots = new_max_bots;
    }
    cout << "Found local opt started from (" << c.x << "," << c.y << "," << c.z << ") finished at (";
    cout << c0.x << "," << c0.y << "," << c0.z << ") with bots in range: " << new_max_bots << endl;
    cout << "Manhattan distance is: " << abs(c0.x)+abs(c0.y)+abs(c0.z) << endl;
}

void day23(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    vector<string> parts, parts2;
    // Format: pos=<x,y,z>, r=r
    int x,y,z;
    size_t r;
    vector<nanobot> nanobots;
    for (string& line : lines) {
        parts = split(line, '>');
        if (!parts.empty()) {
            // Get radius
            parts2 = split(parts[1], '=');
            r = stoul(parts2[1]);
            // Get coords
            parts2 = split(parts[0], '<');
            parts = split(parts2[1], ',');
            x = stoi(parts[0]);
            y = stoi(parts[1]);
            z = stoi(parts[2]);
            nanobots.push_back(nanobot(x,y,z,r));
        }
    }
    cout << "Found " << nanobots.size() << " nanobots\n";
    if (partone) {
        nanobot strongest = *max_element(nanobots.begin(), nanobots.end(),
        [](const nanobot& l, const nanobot& r){ return l.r < r.r; });
        cout << "Strongest nanobot has r=" << strongest.r << endl;
        size_t in_range = 0;
        for (const nanobot& nb : nanobots) {
            if (strongest.is_in_range(nb)) in_range++;
        }
        cout << "Nanobots in it's range: " << in_range << endl;
    } else {
        vector<pair<size_t, size_t>> rssi_on_bots;
        size_t bots_that_reach_this_coord = 0;
        for (size_t i=0; i<nanobots.size(); i++) {
            bots_that_reach_this_coord = 0;
            for (const nanobot& n : nanobots) {
                if (n.is_in_range(nanobots[i])) bots_that_reach_this_coord++;
            }
            rssi_on_bots.push_back({i, bots_that_reach_this_coord});
        }
        sort(rssi_on_bots.begin(), rssi_on_bots.end(),
                [](const pair<size_t, size_t>& l, const pair<size_t, size_t>& r) { return l.second > r.second; });
        // Start exploring from the location which is seen from most bots
        for (size_t i=0; i<1; i++) {
            day23_find_local_opt(nanobots, nanobots[rssi_on_bots[i].first].c, rssi_on_bots[i].second);
        }
    }
}

class cellgroup {
public:
    enum class damage {
        slashing,
        bludgeoning,
        cold,
        fire,
        radiation,
    };
	static const string damage_strings[5];
	static const size_t damage_types = 5;
	static const string weakness_keyword;
	static const string immunity_keyword;
    bool infection;
    size_t ID; // Unique key
    size_t units;
    size_t hp;
	size_t atk;
    damage atk_type;
    size_t initiative;
    bool immunities[damage_types];
    bool weaknesses[damage_types];
    size_t target_id;
    cellgroup() {
        units = 0;
        hp = 0;
        atk = 0;
        initiative = 0;
    }
    void parse_weaknesses(string s) {
        // string contains 'weak to A,B'
        s = s.substr(weakness_keyword.size()+4);
        vector<string> words = split(s, ',');
        if (words.size() > 0) {
            for (string word : words) { weaknesses[find_idx_array(damage_strings, trim_spaces(word))] = true; }
        } else { weaknesses[find_idx_array(damage_strings, s)] = true; }
    }
    void parse_immunities(string s) {
        // string contains 'immune to A,B'
        s = s.substr(immunity_keyword.size()+4);
        vector<string> words = split(s, ',');
        if (words.size() > 0) {
            for (string word : words) { immunities[find_idx_array(damage_strings, trim_spaces(word))] = true; }
        } else { immunities[find_idx_array(damage_strings, s)] = true; }
    }
    cellgroup(string line, bool is_infection, size_t id) : infection(is_infection), ID(id) {
        // Parse string of the following format
        // U units each with H hit points [(weak/immune to A,B,C[;weak/immune to D,E])]
        //  with an attack that does ATK ATK_TYPE damage at initiative I
		vector<string> words = split(line, ' ');
		for (size_t i=1; i<words.size()-1; i++) {
			if (words[i] == "units") {
				units = stoul(words[i-1]);
			} else if (words[i] == "hit") {
				hp = stoul(words[i-1]);
			} else if (words[i] == "damage") {
				atk = stoul(words[i-2]);
				auto it = find(begin(damage_strings), end(damage_strings), words[i-1]);
				atk_type = (damage)(it-begin(damage_strings));
			} else if (words[i] == "initiative") {
				initiative = stoul(words[i+1]);
			}
		}
        for (size_t d = 0; d < damage_types; d++) {
            weaknesses[d] = immunities[d] = false;
        }
        bool has_weaknesses = false;
        bool has_immunities = false;
        size_t brace_open = find_idx(line, '(');
        size_t brace_close = find_idx(line, ')');
        string damage_type_list = "";
        if (brace_open < line.size()) {
            damage_type_list = line.substr(brace_open+1, brace_close-brace_open-1);
            has_weaknesses = contains(split(damage_type_list, ' '), weakness_keyword);
            has_immunities = contains(split(damage_type_list, ' '), immunity_keyword);
        }
        if (has_weaknesses && has_immunities) {
            // line contains (immune to A,B; weak to C), order can vary!
            words = split(damage_type_list, ';');
            if (words[0][0] == 'i') {
                parse_immunities(words[0]);
                parse_weaknesses(words[1]);
            } else {
                parse_weaknesses(words[0]);
                parse_immunities(words[1]);
            }
        } else if (has_weaknesses) {
            parse_weaknesses(damage_type_list);
        } else if (has_immunities) {
            parse_immunities(damage_type_list);
        }
        /* // Debug immunities and weaknesses
        cout << "WI list: " << damage_type_list << endl;
        cout << "   SBCFR\n";
        cout << "W: ";
        for (size_t i=0; i<5; i++) { cout << weaknesses[i]; } cout << endl;
        cout << "I: ";
        for (size_t i=0; i<5; i++) { cout << immunities[i]; } cout << endl;*/
    }
    size_t damage_versus(const cellgroup& target) const {
        size_t mult = 1;
        if (target.immunities[(size_t)atk_type]) {
            mult = 0;
        } else if (target.weaknesses[(size_t)atk_type]) {
            mult = 2;
        }
        return atk * units * mult;
    }
    size_t effective_power() const {
        return atk * units;
    }
    void take_damage(size_t dmg) {
        // This group loses dmg/hp units
        size_t units_lost = (dmg / hp);
        if (units <= units_lost) { units = 0; }
        else { units -= units_lost; }
    }
    void take_damage(size_t dmg, size_t& killed) {
        // This group loses dmg/hp units
        size_t units_lost = (dmg / hp);
        if (units <= units_lost) { killed = units; units = 0; }
        else { killed = units_lost; units -= units_lost; }
    }
    string group_name() const {
        return infection ? ("INF"+to_string(ID)) : ("IS"+to_string(ID));
    }
};

const string cellgroup::weakness_keyword = "weak";
const string cellgroup::immunity_keyword = "immune";
const string cellgroup::damage_strings[] = {
        "slashing",
        "bludgeoning",
        "cold",
        "fire",
        "radiation"};

size_t day24_determine_target(const vector<cellgroup>& c, const size_t idx, vector<size_t> targets, bool verbose = false) {
    size_t potential_damage;
    size_t max_potential_damage = 0;
    size_t best_target = c.size(); // Invalid target
    for (size_t i=0; i<c.size(); i++) {
        // Look for non-self-class enemy which is not already targeted and alive
        if (c[i].infection != c[idx].infection && !contains(targets, c[i].ID) && c[i].units > 0) {
            potential_damage = c[idx].damage_versus(c[i]);
            if (verbose) cout << "Group " << c[idx].group_name() << " would attack group " << c[i].group_name() << " (EP = " << c[i].effective_power() << ") for " << potential_damage << endl;
            if (potential_damage > max_potential_damage) {
                max_potential_damage = potential_damage;
                best_target = i;
            } else if (potential_damage == max_potential_damage) {
                // Choose the target with the highest EffectivePower or in tie, Initiative
                if (c[best_target].effective_power() == c[i].effective_power()) {
                    // Decide based on initiative
                    if (c[best_target].initiative < c[i].initiative) {
                        best_target = i; // New target is i
                    } // Else best_target stays the same
                } else if (c[best_target].effective_power() < c[i].effective_power()) {
                    best_target = i; // New target is i
                } // Else best_target stays the same
            }
        }
    }
    if (best_target != c.size()) {
        // Swap to ID
        if (verbose) cout << "Best target for " << c[idx].group_name() << " is " << c[best_target].group_name() << endl;
        best_target = c[best_target].ID;

    }
    return best_target;
}

bool day24_fight(vector<cellgroup>& c, size_t& is_units, size_t& inf_units, bool verbose = false) {
    // [[ Target selection ]]
    // Sort cellgroups based on effective power (units * atk)
    sort(c.begin(), c.end(), [](const cellgroup& l, const cellgroup& r){
        size_t epl = l.effective_power();
        size_t epr = r.effective_power();
        if (epl == epr) { return l.initiative > r.initiative; }
        else {            return epl > epr; }
    });
    vector<size_t> targets;
    for (size_t i=0; i<c.size(); i++) {
        c[i].target_id = day24_determine_target(c, i, targets, verbose);
        targets.push_back(c[i].target_id);
    }
    // [[ Attack phase ]]
    // Sort cellgroups based on initiative, targets are invalid!
    sort(c.begin(), c.end(), [](const cellgroup& l, const cellgroup& r){
        return l.initiative > r.initiative;
    });
    size_t target_idx;
    size_t units_lost;
    size_t target_id;
    for (size_t i=0; i<c.size(); i++) {
        target_id = c[i].target_id;
        if (c[i].units > 0 && target_id < c.size()) {
            target_idx = find_if(c.begin(), c.end(), [&target_id](const cellgroup& cg) { return cg.ID == target_id; })-c.begin();
            c[target_idx].take_damage( c[i].damage_versus(c[target_idx]), units_lost );
            if (verbose) { cout << "Group " << c[i].group_name() << " hits group " << c[target_idx].group_name() << " for " <<
                 c[i].damage_versus(c[target_idx]) << " damage, killing " << units_lost << " units\n"; }
        }
    }
    // [[ Check battlefield ]]
    is_units = 0;
    inf_units = 0;
    for (const cellgroup& cg : c) {
        if (cg.infection) {
            inf_units += cg.units;
        } else {
            is_units += cg.units;
        }
    }
    if (verbose) {
        cout << "Immune system:\n";
        for (const cellgroup& cg : c) { if (!cg.infection) {cout << "Group " << cg.group_name() << " has " << cg.units << " units\n";} }
        cout << "Infection:\n";
        for (const cellgroup& cg : c) { if (cg.infection) {cout << "Group " << cg.group_name() << " has " << cg.units << " units\n";} }
    }
    if ((is_units == 0 || inf_units == 0)) cout << "Immune system: " << is_units << ", Infection: " << inf_units << endl;
    return (is_units > 0 && inf_units > 0);
}

size_t day24_battle(vector<cellgroup> c, size_t is_boost = 0, bool verbose = false) {
    // Add boost before battle
    for (cellgroup& cg : c) {
        if (!cg.infection) cg.atk += is_boost;
    }
    size_t inf_units, is_units;
    size_t prev_inf_units=0, prev_is_units=0;
    while (day24_fight(c, is_units, inf_units, verbose)) {
        // The battle goes on
        if (inf_units == prev_inf_units && is_units == prev_is_units) {
            cout << "Battle stalled\n";
            break;
        }
        prev_inf_units = inf_units;
        prev_is_units = is_units;
    }
    return inf_units;
}

void day24(string inputfile, bool partone) {
    vector<string> lines = get_lines(inputfile);
    // Look for keywords {Immune, Infection}
    bool is_infection = false;
    string immune_keyword = "Immune";
    string infect_keyword = "Infection:";
    vector<cellgroup> cellgroups;
    size_t id = 0;
    for (string line : lines) {
        if (contains(split(line, ' '), immune_keyword)) {
            is_infection = false;
        } else if (contains(split(line, ' '), infect_keyword)) {
            is_infection = true;
        } else if (split(line, ' ').size() > 2) {
            cellgroups.push_back(cellgroup(line, is_infection, id));
            id++;
        }
    }
    // Battle
    if (partone) {
        day24_battle(cellgroups);
    } else {
        size_t is_boost = 1;
        bool is_won = false;
        while (!is_won) {
            cout << "Executing battle immune system boost is: " << is_boost << endl;
            // Run battle with immune system boost
            size_t inf_units_left = day24_battle(cellgroups, is_boost);
            // Post-battle stats
            if (inf_units_left > 0) {
                is_boost += 1 + inf_units_left / 5000;
            } else {
                is_won = true;
            }
        }
        cout << "Minimum necessary immune system boost is: " << is_boost << endl;
    }
}

class spacetime_coord {
public:
    int x, y, z, t;
    size_t con;
    bool in_con;
    bool checked;
    spacetime_coord(int x, int y, int z, int t) : x(x), y(y), z(z), t(t) {
        in_con = false;
        checked = false;
    }
    spacetime_coord(const vector<string>& words) {
        x = stoi(words[0]);
        y = stoi(words[1]);
        z = stoi(words[2]);
        t = stoi(words[3]);
        in_con = false;
        checked = false;
    }
    spacetime_coord() {
        x = y = z = t = 0;
        in_con = false;
        checked = false;
    }
    bool close_to(const spacetime_coord& other) const {
        return (abs(x-other.x)+abs(y-other.y)+abs(z-other.z)+abs(t-other.t)) <= 3;
    }
};

void day25(string inputfile) {
    vector<string> lines = get_lines(inputfile);
    vector<string> words;
    vector<spacetime_coord> coords;
    int xmin=0, xmax=0;    int ymin=0, ymax=0;
    int zmin=0, zmax=0;    int tmin=0, tmax=0;
    for (const string& line : lines) {
        words = split(line, ',');
        if (words.size() == 4) {
            coords.emplace_back(spacetime_coord(words));
            minmax(coords.back().x,xmin,xmax); minmax(coords.back().y,ymin,ymax);
            minmax(coords.back().z,zmin,zmax); minmax(coords.back().t,tmin,tmax);
        }
    }
    cout << "Found " << coords.size() << " spacetime coordinates\n";
    cout << "Spacetime area covered by the coordinates: \n";
    cout << "X: [" << xmin << ", " << xmax << "]\n";   cout << "Y: [" << ymin << ", " << ymax << "]\n";
    cout << "Z: [" << zmin << ", " << zmax << "]\n";   cout << "T: [" << tmin << ", " << tmax << "]\n";
    // Start looking up constellations
    bool matching = true;
    bool verbose = false;
    size_t current;
    vector<size_t> neighbours;
    size_t constellations = 0;
    vector<size_t> merge_list;
    while (matching) {
        // Select the next spacetime coord without constellation,
        matching = false;
        for (size_t i=0; i<coords.size(); i++) {
            if (!coords[i].checked) {
                matching = true;
                current = i;
                break;
            }
        }
        if (matching) {
            coords[current].checked = true;
            neighbours.clear();
            neighbours.push_back(current);
            for (size_t i=0; i<coords.size(); i++) {
                if (i != current && coords[i].close_to(coords[current])){ neighbours.push_back(i); }
            }
            // All 'neighbours' of current coord is found, check for existing constellation(s)
            merge_list.clear();
            for (size_t& idx : neighbours) {
                if (coords[idx].in_con) {
                    if (!contains(merge_list, coords[idx].con)) { merge_list.push_back(coords[idx].con); }
                }
            }
            if (merge_list.empty()) {
                constellations++;
                if (verbose) cout << "New constellation C" << constellations << endl;
                for (size_t& idx : neighbours) { coords[idx].con = constellations; coords[idx].in_con = true; }
            } else {
                size_t final_constellation = *min_element(merge_list.begin(), merge_list.end());
                if (verbose) cout << "Merging new constellation and (" << merge_list.size()-1 << " others) into C" << final_constellation << endl;
                for (size_t& idx : neighbours) { coords[idx].con = final_constellation; coords[idx].in_con = true; }
                for (spacetime_coord& sc : coords) {
                    if (sc.in_con && contains(merge_list, sc.con)) { sc.con = final_constellation; }
                }
            }
        }
    }
    // Count constellations with more than 1 coords
    vector<size_t> const_list;
    for (const spacetime_coord& sc : coords) {
        if (!sc.in_con || !sc.checked) {
            cout << "Error: unclassified spacetime coord found!\n";
        } else {
            if (!contains(const_list, sc.con)) { const_list.push_back(sc.con); }
        }
    }
    size_t lone_constellations = 0;
    size_t cnt = 0;
    for(size_t & con : const_list) {
        cnt = 0;
        for (const spacetime_coord& sc : coords) {
            if (sc.con == con) cnt++;
            if (cnt >= 2) { break; }
        }
        if (cnt == 1) lone_constellations++;
    }
    cout << "Number of constellations (including single-coord ones): " << const_list.size() << endl;
    cout << "Single-coord constellations: " << lone_constellations << endl;
    cout << "Number of constellations: " << const_list.size()-lone_constellations << endl;
}
