#include "messageManager.h"
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <windows.h>
#include <iostream>

const int MAXLEN = 20;

struct RingQueueHeader {
    int head = 0;
    int tail = 0;
    int capacity = 0;
};

HANDLE hMutex = nullptr;
HANDLE hFull = nullptr;
HANDLE hEmpty = nullptr;

void initSyncObjects(int queueSize) {
    hMutex = CreateMutex(NULL, FALSE, "FileMutex");
    hFull = CreateSemaphore(NULL, 0, queueSize, "MsgCount");
    hEmpty = CreateSemaphore(NULL, queueSize, queueSize, "SpaceCount");
}

void openSyncObjects() {
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "FileMutex");
    hFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "MsgCount");
    hEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SpaceCount");
}

bool writeMessage(const std::string& fileName, const std::string& message) {
    if (!hMutex || !hFull || !hEmpty) return false;

    uint8_t len = static_cast<uint8_t>(std::min<size_t>(message.size(), MAXLEN));

    WaitForSingleObject(hEmpty, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);

    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hEmpty, 1, nullptr);
        return false;
    }

    RingQueueHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    file.seekp(sizeof(header) + header.tail * (1 + MAXLEN));
    file.write(reinterpret_cast<char*>(&len), sizeof(len));

    char buf[MAXLEN] = { 0 };
    std::copy_n(message.c_str(), len, buf);
    file.write(buf, MAXLEN);

    header.tail = (header.tail + 1) % header.capacity;
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    file.close();
    ReleaseMutex(hMutex);
    ReleaseSemaphore(hFull, 1, nullptr);

    return true;
}

bool readMessage(const std::string& fileName, std::string& outMessage) {
    if (!hMutex || !hFull || !hEmpty) return false;

    WaitForSingleObject(hFull, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);

    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hFull, 1, nullptr);
        return false;
    }

    RingQueueHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    file.seekg(sizeof(header) + header.head * (1 + MAXLEN));
    uint8_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));

    char buf[MAXLEN];
    file.read(buf, MAXLEN);

    outMessage.assign(buf, len);

    header.head = (header.head + 1) % header.capacity;
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    file.close();
    ReleaseMutex(hMutex);
    ReleaseSemaphore(hEmpty, 1, nullptr);

    return true;
}
