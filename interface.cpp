#include "interface.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <cctype>

// person
Person::Person(const std::string in_name, Person *boss)
{
    name = in_name;
    parent = boss;

    if (boss)
    {
        boss->addSubordinate(this);
    }
}

void Person::addSubordinate(Person *new_subordiante)
{
    new_subordiante->parent = this;
    subordinates.push_back(new_subordiante);

    // sort subordinates
    std::sort(subordinates.begin(), subordinates.end(), [](const Person *lhs, const Person *rhs)
              { return lhs->getName() < rhs->getName(); });
}

std::string Person::getName() const
{
    return name;
}

std::vector<Person *> Person::getSubordinates() const
{
    return subordinates;
}

Person *Person::getParent() const
{
    return parent;
}

std::size_t Person::subordinatesNumber() const
{
    return subordinates.size();
}

void Person::removeSubordinate(const std::string &who)
{
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        // find and remove
        if (subordinates[i]->name == who)
        {
            subordinates.erase(subordinates.begin() + i);

            return;
        }
    }
}

// Hierarchy

void Hierarchy::count(Person *cur, unsigned int &employees) const
{
    if (cur->subordinatesNumber() == 0)
    {
        return;
    }

    // count employees
    std::vector<Person *> subordinates = cur->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        employees++;

        count(subordinates[i], employees);
    }
}

unsigned int Hierarchy::getSubtreeEmplyees(Person *head) const
{
    unsigned int employees = 0;
    count(head, employees);

    return employees;
}

void Hierarchy::traverse(const Person *current_r)
{
    // traverse hierarchy
    if (current_r->getParent())
    {
        new Person(current_r->getName(), getPerson(current_r->getParent()->getName(), head_manager));
    }
    else
    {
        head_manager = new Person(current_r->getName(), nullptr);
    }

    std::vector<Person *> subordinates = current_r->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        traverse(subordinates[i]);
    }
}

Hierarchy::Hierarchy(Hierarchy &&r) noexcept
{
    head_manager = r.head_manager;
    r.head_manager = nullptr;

    total_employees = r.total_employees;
    r.total_employees = 0;
}

Hierarchy::Hierarchy(const Hierarchy &r)
{
    // deep copy all elements
    traverse(r.head_manager);
    total_employees = r.total_employees;
}

Hierarchy::Hierarchy(Person *head)
{
    // set head and count the total subordinates of head
    head_manager = head;
    unsigned int cnt = 0;

    count(head, cnt);

    total_employees = 1 + cnt;
}

Hierarchy::Hierarchy(const string &data)
{
    bool first = true;

    for (std::size_t i = 0; i < data.size(); i++)
    {
        std::string boss_name = getSubstring(data, i, '-');
        if (boss_name == "")
        {
            throw std::invalid_argument("No boss name!");
        }

        i += 1 + boss_name.size();

        std::string subordinate_name = getSubstring(data, i, '\n');
        if (subordinate_name == "")
        {
            throw std::invalid_argument("No subordinate name!");
        }

        i += subordinate_name.size();

        // trim
        boss_name.erase(std::remove_if(boss_name.begin(), boss_name.end(), ::isspace), boss_name.end());
        subordinate_name.erase(std::remove_if(subordinate_name.begin(), subordinate_name.end(), ::isspace), subordinate_name.end());

        if (first)
        {
            // set head manager and his first subordinate
            head_manager = new Person(boss_name, nullptr);
            new Person(subordinate_name, head_manager);

            total_employees += 2;

            first = false;
        }
        else
        {
            // add next subordinate
            new Person(subordinate_name, this->getPerson(boss_name, head_manager));
            total_employees++;
        }
    }
}

bool Hierarchy::find(const string &name) const
{
    Person *returned = this->getPerson(name, head_manager);

    return returned ? true : false;
}

std::string Hierarchy::manager(const string &name) const
{
    Person *returned = getPerson(name, head_manager);

    // manager doesn't exist
    if (!returned)
    {
        return "";
    }

    return returned->getParent() ? returned->getParent()->getName() : "";
}

int Hierarchy::num_subordinates(const string &name) const
{
    Person *returned = getPerson(name, head_manager);

    return returned ? returned->subordinatesNumber() : -1;
}

int Hierarchy::num_all_subordinates(const string &name) const
{
    return getSubtreeEmplyees(getPerson(name, head_manager));
}

int Hierarchy::num_employees() const
{
    return total_employees;
}

unsigned long Hierarchy::getSalary(const string &who) const
{
    Person *returned = getPerson(who, head_manager);
    if (!returned)
    {
        return -1;
    }

    unsigned int direct_subordinates = returned->subordinatesNumber();
    unsigned int indirect_subordinates = num_all_subordinates(who) - direct_subordinates;

    return 500 * direct_subordinates + 50 * indirect_subordinates;
}

bool Hierarchy::hire(const string &who, const string &boss)
{
    Person *p = getPerson(who, head_manager);

    // rehire person
    if (p && p->getParent())
    {
        // get the old and the new boss
        Person *b = getPerson(p->getParent()->getName(), head_manager);
        Person *new_b = getPerson(boss, head_manager);

        if (!new_b)
            return false;

        // same boss
        if (b->getName() == boss)
            return true;

        p->getParent()->removeSubordinate(p->getName());
        new_b->addSubordinate(p);

        return true;
    }

    p = getPerson(boss, head_manager);

    // boss doesn't exist
    if (!p)
        return false;

    new Person(who, p);
    total_employees++;

    return true;
}

bool Hierarchy::fire(const string &who)
{
    // check wether the input person is the head manager
    if (head_manager->getName() == who)
    {
        return false;
    }

    Person *cur = getPerson(who, head_manager);

    if (!cur || !cur->getParent())
    {
        return false;
    }

    Person *boss = cur->getParent();

    // move subordinates to boss
    std::vector<Person *> cur_subordinates = cur->getSubordinates();
    boss->removeSubordinate(who);

    for (std::size_t i = 0; i < cur_subordinates.size(); i++)
    {
        cur->removeSubordinate(cur_subordinates[i]->getName());
        boss->addSubordinate(cur_subordinates[i]);
    }

    delete cur;
    total_employees--;

    return true;
}

void Hierarchy::help_overloaded(int level, Person *cur, int &count) const
{
    // count overloaded
    if (getSubtreeEmplyees(cur) > level)
    {
        count++;
    }

    std::vector<Person *> subordinates = cur->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        help_overloaded(level, subordinates[i], count);
    }
}

int Hierarchy::num_overloaded(int level) const
{
    int count = 0;

    if (head_manager)
    {
        help_overloaded(level, head_manager, count);
    }

    return count;
}

int Hierarchy::calculateLongest(Person *cur) const
{
    // end of chain
    if (cur->subordinatesNumber() == 0)
    {
        return 1;
    }

    int longest = 0;

    std::vector<Person *> subordinates = cur->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        int current = calculateLongest(subordinates[i]);

        if (current + 1 > longest)
        {
            longest = current + 1;
        }
    }

    return longest;
}

Person *Hierarchy::getPerson(const string &name, Person *current) const
{
    if (!current)
    {
        return nullptr;
    }

    // person found in subtree
    if (current->getName() == name)
    {
        return current;
    }

    std::vector<Person *> subordinates = current->getSubordinates();

    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        // check if person was found in subtree
        Person *returned = getPerson(name, subordinates[i]);
        if (returned)
        {
            return returned;
        }
    }

    return nullptr;
}

int Hierarchy::longest_chain() const
{
    if (!head_manager)
    {
        return 0;
    }

    return calculateLongest(head_manager);
}

void printLevelOrder(Person *head, std::string &output)
{
    if (head == nullptr)
        return;

    // queue to store elements of on current level
    std::queue<Person *> queue;
    queue.push(head);

    while (!queue.empty())
    {
        // get the number of nodes on the current row
        int nodeCount = queue.size();

        while (nodeCount > 0)
        {
            // add to string
            Person *current = queue.front();
            if (current && current->getParent())
            {
                output += current->getParent()->getName() + "-" + current->getName() + "\n";
            }

            queue.pop();

            // add the nodes of the next row
            for (std::size_t i = 0; i < current->getSubordinates().size(); i++)
            {
                queue.push(current->getSubordinates()[i]);
            }

            nodeCount--;
        }
    }
}

std::string Hierarchy::print() const
{
    std::string output;
    printLevelOrder(head_manager, output);

    return output;
}

void Hierarchy::promote(Person *who, Person *boss)
{
    std::vector<Person *> siblings = boss->getSubordinates();

    // move person one level up
    for (std::size_t i = 0; i < siblings.size(); i++)
    {
        if (siblings[i]->getName() != who->getName())
        {
            boss->removeSubordinate(siblings[i]->getName());
            who->addSubordinate(siblings[i]);
        }
    }
}

Person *Hierarchy::getHighestSalary(const std::vector<Person *> &people)
{
    int highest = 0;
    std::size_t index = 0;

    for (std::size_t i = 0; i < people.size(); i++)
    {
        std::string name = people[i]->getName();
        int current = this->getSalary(people[i]->getName());
        if (highest < current)
        {
            highest = current;
            index = i;
        }
        // todo add for equal salaries
    }

    return people[index];
}

void Hierarchy::promoteRow(std::queue<Person *> &row)
{
    while (!row.empty())
    {
        Person *current = row.front();

        if (current->subordinatesNumber() > 1)
        {
            promote(getHighestSalary(current->getSubordinates()), current);
        }

        row.pop();
    }
}

void Hierarchy::incorporate()
{
    std::size_t level = height(head_manager);

    // iterate for each hierarchy row
    while (level > 0)
    {
        std::queue<Person *> queue = onLevel(head_manager, level);
        promoteRow(queue);
        level--;
    }
}

unsigned int getLevel(Person *current, int cur_level, std::string name)
{
    if (current->getName() == name)
        return cur_level;

    for (std::size_t i = 0; i < current->getSubordinates().size(); i++)
    {
        // go down one level
        int new_level = getLevel(current->getSubordinates()[i], cur_level + 1, name);

        if (new_level != 0)
            return new_level;
    }

    return 0;
}

unsigned int Hierarchy::height(Person *current) const
{
    if (current == nullptr)
    {
        return 0;
    }

    // get the height of each subtree
    unsigned int total = 0;

    std::vector<Person *> subordinates = current->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        unsigned int returned = height(subordinates[i]);

        // assign the greatest subtree height
        if (total < returned)
        {
            total = returned;
        }
    }

    return total + 1;
}

void Hierarchy::addFromRight(Person *p_right, const Hierarchy &l_hierarchy, Hierarchy &new_hierachy) const
{
    // add if person is only present in the right hierarchy
    if (!l_hierarchy.getPerson(p_right->getName(), l_hierarchy.head_manager))
    {
        new_hierachy.hire(p_right->getName(), p_right->getParent()->getName());
    }

    std::vector<Person *> subordinates = p_right->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        addFromRight(subordinates[i], l_hierarchy, new_hierachy);
    }
}

void Hierarchy::joinHelp(Person *p_left, const Hierarchy &h_right, Hierarchy &new_hierachy) const
{
    Person *p_right = h_right.getPerson(p_left->getName(), h_right.head_manager);

    if (p_right && p_right->getParent())
    {
        // check if boss is the same
        if (p_left->getParent()->getName() != p_right->getParent()->getName())
        {
            // chose the boss on the higher position
            unsigned int left = getLevel(head_manager, 0, p_left->getParent()->getName());
            unsigned int right = getLevel(h_right.head_manager, 0, p_right->getParent()->getName());

            if (right > left)
            {
                new_hierachy.hire(p_right->getName(), p_left->getParent()->getName());
            }
            else if (right < left)
            {
                new_hierachy.hire(p_right->getName(), p_right->getParent()->getName());
            }
            else
            {
                // compare
                (p_left->getParent()->getName() > p_right->getParent()->getName())
                    ? new_hierachy.hire(p_right->getName(), p_right->getParent()->getName())
                    : new_hierachy.hire(p_left->getName(), p_left->getParent()->getName());
            }
        }
        else
        {
            new_hierachy.hire(p_left->getName(), p_left->getParent()->getName());
        }
    }
    else if (p_left->getParent())
    {
        new_hierachy.hire(p_left->getName(), p_left->getParent()->getName());
    }

    std::vector<Person *> subordinates = p_left->getSubordinates();
    for (std::size_t i = 0; i < subordinates.size(); i++)
    {
        joinHelp(subordinates[i], h_right, new_hierachy);
    }
}

Hierarchy Hierarchy::join(const Hierarchy &right) const
{
    Hierarchy new_hierarchy(new Person(head_manager->getName(), nullptr));
    addFromRight(right.head_manager, *this, new_hierarchy);
    joinHelp(this->head_manager, right, new_hierarchy);

    return new_hierarchy;
}

void Hierarchy::demote(Person *who, Person *boss)
{
    fire(who->getName());
}

void Hierarchy::modernize()
{
    // levels from root
    int level = height(head_manager);

    level = (level % 2 == 0) ? level : level - 1;

    std::queue<Person *> current;

    // iterate for each odd level from root
    while (level > 1)
    {
        current = onLevel(head_manager, level);

        while (!current.empty())
        {
            // demote only if current is team leader
            if (current.front()->subordinatesNumber() > 0)
            {
                demote(current.front(), current.front()->getParent());
            }

            current.pop();
        }

        level -= 2;
    }
}

Hierarchy::~Hierarchy()
{
    std::cout << "destruct" << std::endl;
}

// helpers

std::string getSubstring(const std::string &str, std::size_t cur_pos, char symbl)
{
    std::string::size_type pos = str.find(symbl, cur_pos);

    if (pos != std::string::npos)
    {
        return str.substr(cur_pos, pos - cur_pos);
    }
    else
    {
        return "";
    }
}

std::queue<Person *> onLevel(Person *head, int cur_level)
{
    std::size_t level = 1;

    // queue to store elements of on current level
    std::queue<Person *> queue;
    queue.push(head);

    if (cur_level == level)
    {
        return queue;
    }

    while (!queue.empty())
    {
        // get the number of nodes on the current row
        int nodeCount = queue.size();

        while (nodeCount > 0)
        {
            Person *current = queue.front();
            queue.pop();

            // add the nodes of the next row
            for (std::size_t i = 0; i < current->getSubordinates().size(); i++)
            {
                queue.push(current->getSubordinates()[i]);
            }

            nodeCount--;
        }

        level++;

        if (level == cur_level)
        {
            return queue;
        }
    }

    return queue;
}