#include <iostream>
#include <ostream>
#include <vector>

int destroy_count = 0;
int init_count = 0;
int copy_count = 0;
int move_count = 0;


class basic_t {
public:
    int val;

    ~basic_t() {
        std::cout << "basic_t::destroy\n";
        destroy_count++;
        val = 0;
    }

    constexpr basic_t(int val) : val(val) {
        std::cout << "basic_t::init\n";
        init_count++;
    }

    constexpr basic_t(const basic_t& other) : val(other.val) {
        std::cout << "basic_t::copy1\n";
        copy_count++;
    }
    
    
    constexpr basic_t(basic_t&& other) : val(other.val) {
        std::cout << "basic_t::move1\n";
        move_count++;
        other.val = 0;
    }

    basic_t& operator=(const basic_t& other) {
        std::cout << "basic_t::copy2\n";
        copy_count++;
        val = other.val;
        return *this;
    }

    basic_t& operator=(basic_t&& other) {
        std::cout << "basic_t::move2\n";
        move_count++;
        val = other.val;
        other.val = 0;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const basic_t& object) {
        return os << "object(" << object.val << ")";
    }
};

class object_t : public basic_t {
    int val_2;
public:
    constexpr object_t(int val) : basic_t(val), val_2(0) {}

    constexpr object_t(const object_t& other) : basic_t(other.val), val_2(0) {
        std::cout << "object_t::copy1\n";
        copy_count++;
    }

    constexpr object_t(object_t&& other) = default;

};

class dataframe_t {
    
public:
    dataframe_t(std::initializer_list<std::pair<std::string, object_t>>&& init_values) {
        
    }
};



int main(int argc, char** argv) {
    object_t outside(314);

    dataframe_t df = {
        {"A", object_t(0)},
        {"B", object_t(1)},
        {"C", object_t(2)},
        {"D", outside},
        {"E", object_t(4)},
    };


    std::cout << "== count =="
            << "\n  destroy: " << destroy_count
            << "\n  init: " << init_count
            << "\n  copy: " << copy_count
            << "\n  move: " << move_count << "\n"
    ;
}