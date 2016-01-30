if (1) {print(1)} else if (1) {print(2)} else {print(3)}; print(4);

class X {
    function _add(x) {
        print("add("+x+")\n");
        return this;
    }
};

class Y extends X {
    function f() {
        base += 3; // troublesome
    }
};

y <- Y();
y += 3;
y.f();