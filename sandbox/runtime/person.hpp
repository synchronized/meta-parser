#pragma once

#include <string>

#include "runtime/meta/reflection.hpp"

namespace Sandbox {

CLASS(Email, Fields) {
public:
    Email() {}
    Email(
        std::string name,
        std::string email
    ) : name(name), email(email) {}

    std::string name;
    std::string email;
};

CLASS(Person, Fields) {
public:
    Person() {}
    Person(
        int id,
        std::string name
    ) : id(id), name(name) {}
    Person(
        int id,
        std::string name,
        std::vector<Email> emails
    ) : id(id), name(name), emails(emails) {}

    int id;
    std::string name;
    std::vector<Email> emails;
};

CLASS(Student: public Person, Fields) {
public:
    Student() {}
    Student(
        int id,
        std::string name,
        std::vector<Email> emails,
        Person* tech
    ) : Person(id, name, emails), tech(tech) {}

    Person* tech{nullptr};
};

}
