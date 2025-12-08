#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "messageManager.h"

int main() {
    setlocale(LC_ALL, "ru");

    std::string fileName;
    int numSenders, numMessages;

    std::cout << "Введите имя бинарного файла: ";
    std::cin >> fileName;

    std::cout << "Введите количество процессов Sender: ";
    std::cin >> numSenders;

    std::cout << "Введите размер кольцевой очереди (количество сообщений): ";
    std::cin >> numMessages;

    std::fstream file(fileName, std::ios::binary | std::ios::trunc | std::ios::out);
    
    struct RingQueue {
        int head = 0;
        int tail = 0;
        int capacity = 0;
    }

    header;
    header.capacity = numMessages;
    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    file.close();

    initSyncObjects(numMessages - 1);

    for (int i = 0; i < numSenders; i++) {
        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(si);

        std::string cmd = "D:\\OS_labs\\Lab_4\\sender\\out\\build\\x64-debug\\sender.exe " + fileName;

        if (CreateProcessA(NULL, cmd.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cout << "Процесс Sender " << (i + 1) << " запущен" << std::endl;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Ошибка запуска процесса " << (i + 1) << ", код ошибки: " << GetLastError() << std::endl;
        }
    }

    std::cout << "\nКоманды: read / finish" << std::endl;

    while (true) {
        std::string cmd;
        std::cout << "> ";
        std::cin >> cmd;

        if (cmd == "finish") 
            break;

        if (cmd == "read") {
            std::string msg;
            if (readMessage(fileName, msg)) {
                std::cout << "Сообщение: " << msg << std::endl;
            }
            else {
                std::cout << "Новых сообщений нет" << std::endl;
            }
        }
    }

    std::cout << "Receiver завершил работу" << std::endl;
    return 0;
}
