
#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include <string>
#include <vector>

struct Smt_pattern {
  std::string pattern;
  std::string code;
};

class Semantic {

  public:
    Semantic(const char *smt_db);
    ~Semantic();

  private:
    char *buff;
    bool load_status;
    std::vector<Smt_pattern> pattern_db;
    bool parse_status;
    void load(const char *smt_db);
    void parse();

};

#endif // SEMANTIC_H_INCLUDED
