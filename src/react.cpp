#include <catch2/catch.hpp>

#include <GSL/gsl>

#include <functional>
#include <ostream>
#include <sstream>
#include <variant>
#include <vector>

class Property;
class Ref;

std::variant<int, float, bool, std::string>;

class Property {
  protected:
    int mValue;
    std::function<int()> mFunc;

    std::vector<Property*> mParents;
    std::vector<Property*> mChildren;

  public:
    using type = int;

    Property() = default;
    ~Property() {
        if (isDynamic()) {
            reset();
        }
    };

    Property(const Property&) = delete;
    Property& operator=(const Property&) = delete;

    operator int() const {
        return mValue;
    }

    void set(int i) {
        if (isDynamic()) {
            reset();
        }
        mValue = i;
        for (auto c : mChildren) {
            c->evaluate();
        }
    }

    void set(std::function<int(const int&)> fnc, Property& d0) {
        reset();
        mFunc = std::bind(fnc, std::cref(d0.get()));

        mParents.push_back(&d0);
        d0.mChildren.push_back(this);

        evaluate();
    }

    void set(std::function<int(const int&, const int&)> fnc, Property& d0, Property& d1) {
        mFunc = std::bind(fnc, std::cref(d0.get()), std::cref(d1.get()));

        mParents.push_back(&d0);
        d0.mChildren.push_back(this);

        mParents.push_back(&d1);
        d1.mChildren.push_back(this);

        evaluate();
    }

    void reset() {
        mFunc = nullptr;
        for (auto& d : mParents) {
            for (auto it = d->mChildren.begin(); it != d->mChildren.end(); it++) {
                if (*it == this) {
                    d->mChildren.erase(it);
                    break;
                }
            }
        }
    }

    void evaluate() {
        if (isDynamic()) {
            mValue = mFunc();
            for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
                (*it)->evaluate();
            }
        }
    }

    constexpr int& get() {
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

    bool dependsOn(const Property& other) {
        return false;
    }

    std::ostream& printDebugInfo(std::ostream& os) const {
        os << "Property, mValue: " << mValue;
        return os;
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
void t(std::variant<Args...>& v, std::remove_pointer_t<Args>... args) {
}

TEST_CASE("Properties", "[properties]") {
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
    p1.set([](const int& d0) -> int { return d0 * 10; }, p0);
    p2.set([](const int& d0, const int& d1) -> int { return d0 + d1; }, p0, p1);

    // p1.evaluate();
    // p2.evaluate();

    REQUIRE(p0 == 1);
    REQUIRE(p1 == 10);
    REQUIRE(p2 == 11);

    p0.set(2);
    // p1.evaluate();
    // p2.evaluate();

    REQUIRE(p0 == 2);
    REQUIRE(p1 == 20);
    REQUIRE(p2 == 22);

    p2.reset();

    p0.set(3);
    // p1.evaluate();
    // p2.evaluate();

    REQUIRE(p0 == 3);
    REQUIRE(p1 == 30);
    REQUIRE(p2 == 22);
}

/*TEST_CASE("DAG", "[properties]") {
    // https://www.electricmonk.nl/log/2008/08/07/dependency-resolving-algorithm/
    Property a, b, c, d, e;

    a.set(0);
    b.set([](int a) { return a + 1; }, a);
    c.set([](int b) { return b + 1; }, b);
    d.set([](int a, int b) { return a + b + 1; }, a, b);
    e.set([](int b, int c) { return b + c + 1; }, b, c);

    std::stringstream ss;
    a.printDebugInfo(ss) << '\n';
    b.printDebugInfo(ss) << '\n';
    c.printDebugInfo(ss) << '\n';
    d.printDebugInfo(ss) << '\n';
    e.printDebugInfo(ss);

    INFO(ss.str());

    REQUIRE(a.isLiteral());

    SECTION("Direct dependencies") {
        REQUIRE(b.dependsOn(a));
        REQUIRE(c.dependsOn(b));

        REQUIRE(d.dependsOn(a));
        REQUIRE(d.dependsOn(c));

        REQUIRE(e.dependsOn(b));
        REQUIRE(e.dependsOn(c));
    }

    SECTION("Indirect dependencies") {
        REQUIRE(c.dependsOn(a));
        REQUIRE(d.dependsOn(b));
        REQUIRE(e.dependsOn(a));
    }
}*/
