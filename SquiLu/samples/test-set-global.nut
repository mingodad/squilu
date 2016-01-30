global <- 0;
class Test {
  static function Go() {
    print(global + "\n");
    global = 1;
    print(global + "\n");
  }
}
Test.Go();