
class Handler {
  public:
    static Handler* getInstance();

  private:
    static Handler *_instance;
    Handler();
};

