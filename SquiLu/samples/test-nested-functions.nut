function z() {
    function y() {
        return 3;
    }
    return y();
}

print(z());
print("z'd\n");
print(y());
print("y'd\n");