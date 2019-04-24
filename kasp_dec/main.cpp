#include <iostream>

#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    kasp::decorator db(new kasp::db_mock(), 10);
    //kasp::db_mock db;
    //
    db.put("A","111");
    db.put("B","222");
    db.put("C","333");
    db.put("D","444");
    std::string data = db.get("C");
    std::cout << "data = " << data.c_str() << std::endl;
    data.clear();
    data = db.get("W");
    std::cout << "data = " << data.c_str() << std::endl;
    db.remove("C");
    data.clear();
    data = db.get("C");
    std::cout << "data = " << data.c_str() << std::endl;

    //db.put("A","111");
    //db.put("B","222");
    //db.put("C","333");
    //db.put("D","444");
    //std::string data = db.get("C");
    //std::cout << "data = " << data.c_str() << std::endl;
    //data.clear();
    //data = db.get("W");
    //std::cout << "data = " << data.c_str() << std::endl;
    //db.remove("C");
    //data.clear();
    //data = db.get("C");
    //std::cout << "data = " << data.c_str() << std::endl;
    return 0;
}
