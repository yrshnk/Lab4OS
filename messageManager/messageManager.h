#pragma once
#include <string>

bool writeMessage(const std::string& fileName, const std::string& msg);
bool readMessage(const std::string& fileName, std::string& outMessage);

void initSyncObjects(int queueSize); 
void openSyncObjects(); 
