#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>

class Database {
public:
    static pqxx::connection& get();
private:
    static pqxx::connection* conn;
    Database() = delete;
};

#endif
