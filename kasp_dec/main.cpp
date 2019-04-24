#include <iostream>

#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    kasp::decorator db(new kasp::db_mock(), 1, 1);
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

    std::this_thread::sleep_for(std::chrono::seconds(20));

    return 0;
}
