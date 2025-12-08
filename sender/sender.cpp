#include <iostream>
#include <string>
#include "D:\OS_labs\Lab_4\messageManager\messageManager.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru");

    if (argc < 2) {
        std::cerr << "Ошибка: не указан файл" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];

    openSyncObjects();

    std::cout << "Процесс Sender запущен, команды: send / finish" << std::endl;

    while (true) {
        std::string cmd;
        std::cout << "> ";
        std::cin >> cmd;

        if (cmd == "finish") break;
        if (cmd == "send") {
            std::cin.ignore();
            std::string msg;
            std::cout << "Введите сообщение (до 20 символов): ";
            std::getline(std::cin, msg);

            if (msg.empty()) {
                std::cout << "Пустое сообщение" << std::endl;
                continue;
            }

            if (!writeMessage(fileName, msg)) {
                std::cerr << "Ошибка записи" << std::endl;
                continue;
            }

            std::cout << "Сообщение записано" << std::endl;
        }
    }

    std::cout << "Sender завершил работу" << std::endl;
    return 0;
}
