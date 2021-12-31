#pragma once
#include <string>
#include <vector>
#include <queue>

using std::string;

const unsigned int PARENT = 1;

class Person
{
private:
    Person *parent;
    string name;
    std::vector<Person *> subordinates;

public:
    Person(const std::string in_name, Person *boss);

    string getName() const;
    std::vector<Person *> getSubordinates() const;
    std::size_t subordinatesNumber() const;
    Person *getParent() const;

    void addSubordinate(Person *new_subordiante);
    void removeSubordinate(const std::string &who);
};

class Hierarchy
{
public:
    Hierarchy(Hierarchy &&r) noexcept;
    Hierarchy(const Hierarchy &r); // done
    Hierarchy(const string &data); // done
    ~Hierarchy() noexcept;         // in deveopment
    void operator=(const Hierarchy &) = delete;

    string print() const; // done

    int longest_chain() const;                // done
    bool find(const string &name) const;      // done
    int num_employees() const;                // done
    int num_overloaded(int level = 20) const; // done

    string manager(const string &name) const;         // done
    int num_subordinates(const string &name) const;   // done
    unsigned long getSalary(const string &who) const; // done

    bool fire(const string &who);                     // done
    bool hire(const string &who, const string &boss); // done

    void incorporate(); // done
    void modernize();   // done

    Hierarchy join(const Hierarchy &right) const; // done

    // add head manager only
    Hierarchy(Person *head);

    // promotion and demotion of people
    void promote(Person *who, Person *boss);
    void demote(Person *who, Person *boss);

    // get highest slary from vector of people
    Person *getHighestSalary(const std::vector<Person *> &people);

    // get person by name
    Person *getPerson(const string &name, Person *current) const;

    // join helpers
    // add all people that exist in the right tree only
    void addFromRight(Person *p_right, const Hierarchy &l_hierarchy, Hierarchy &new_hierachy) const;
    // add people that exist in left or both hierarchies
    void joinHelp(Person *left, const Hierarchy &h_right, Hierarchy &new_hierachy) const;

    // get height of tree
    unsigned int height(Person *current) const;

    // retrun both direct and indirect subordinates
    int num_all_subordinates(const string &name) const;

    // promote team leaders on current row
    void promoteRow(std::queue<Person *> &row);

private:
    Person *head_manager = nullptr;
    std::size_t total_employees = 0;

    //traverse tree for copy constructor
    void traverse(const Person *current);

    //count people in hierarchy
    void count(Person *cur, unsigned int& employees) const;

    //helper for num_overloaded()
    void help_overloaded(int level, Person *cur, int &count) const;

    //returns the longest chain
    int calculateLongest(Person *cur) const;

    //returns subtree with head as root
    unsigned int getSubtreeEmplyees(Person* head) const;
};

// helpers
std::string getSubstring(const std::string &str, std::size_t cur_pos, char stmbl);

//returns all people on current level
std::queue<Person *> onLevel(Person *head, int cur_level);