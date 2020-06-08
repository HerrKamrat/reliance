#include <catch2/catch.hpp>

#include <functional>
#include <vector>
#include <variant>

class Property;
class Ref;

std::variant<int, float, bool, std::string>;

class Property
{
    int mValue;
    std::function<int()> mFunc;

public:
    using type = int;

    Property() = default;
    ~Property(){
        if(isDynamic()){
            reset();
        }
    };

    Property(const Property&) = delete;
    Property& operator=(const Property&) = delete;

    operator int() const { return mValue; }

    void set(int i)
    {
        if(isDynamic()){
            reset();
        }
        mValue = i;
    }

    void set(std::function<int(const int&)> fnc, Property &d0){
        mFunc = std::bind(fnc, std::cref(d0.get()));
        evaluate();
    }

    void set(std::function<int(const int&, const int&)> fnc, Property &d0, Property &d1){
        mFunc = std::bind(fnc, std::cref(d0.get()), std::cref(d1.get()));
        evaluate();
    }

    void reset(){
        mFunc = nullptr;
    }

    void evaluate() {
        if (isDynamic())
        {
            mValue = mFunc();
        }
    }

    constexpr int &get() {
        return mValue;
    }

    constexpr const int& get() const {
        return mValue;
    }

    bool isLiteral() const {
        return mFunc == nullptr;
    }

    bool isDynamic() const {
        return mFunc != nullptr;
    }
};


/*
template<class ...Args>
void t(std::function<int(std::add_lvalue_reference<Args>::type...)> fnc, std::reference_wrapper<Args>... args){
    auto f = std::bind(fnc, args...);
    f();
}
*/

template <class... Args>
void t(std::variant<Args...> &v, std::remove_pointer_t<Args>... args)
{
}

TEST_CASE("Properties", "[properties]")
{
/* 
    int i;
    float f;
    bool b;
    std::variant<int *, float *, bool *> v;
    t(v, i, f, b);
*/
    /*
    int i= 0;
    t([](int& i, int& j)->int{
        return i + j;
    }, std::ref(i), std::ref(i));
    */

    Property p0;
    Property p1;
    Property p2;

    p0.set(1);
    p1.set([](const int &d0) -> int {
        return d0 * 10;
    },
           p0);
    p2.set([](const int& d0, const int& d1)->int{
        return d0 + d1;
    }, p0, p1);

    p1.evaluate();
    p2.evaluate();

    REQUIRE(p0 == 1);
    REQUIRE(p1 == 10);
    REQUIRE(p2 == 11);

    p0.set(2);
    p1.evaluate();
    p2.evaluate();

    REQUIRE(p0 == 2);
    REQUIRE(p1 == 20);
    REQUIRE(p2 == 22);

    p2.reset();

    p0.set(3);
    p1.evaluate();
    p2.evaluate();

    REQUIRE(p0 == 3);
    REQUIRE(p1 == 30);
    REQUIRE(p2 == 22);

}
