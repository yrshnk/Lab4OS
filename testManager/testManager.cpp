#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "messageManager.h"

static const std::string TESTFILE = "test.bin";

void createQueueFile(int capacity)
{
    std::fstream file(TESTFILE, std::ios::binary | std::ios::trunc | std::ios::out);

    struct Header {
        int head = 0;
        int tail = 0;
        int capacity = 0;
    } header;

    header.capacity = capacity;
    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    file.close();
}

void deleteQueueFile()
{
    std::remove(TESTFILE.c_str());
}

class MessageManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        createQueueFile(5);
        initSyncObjects(5);  
    }

    void TearDown() override {
        deleteQueueFile();
    }
};

TEST_F(MessageManagerTest, WriteSingleMessage)
{
    std::string msg = "test1";

    bool ok = writeMessage(TESTFILE, msg);
    ASSERT_TRUE(ok);

    std::string read;
    ASSERT_TRUE(readMessage(TESTFILE, read));
    ASSERT_EQ(read, msg);
}


TEST_F(MessageManagerTest, WriteReadFIFO)
{
    writeMessage(TESTFILE, "test");
    writeMessage(TESTFILE, "testtest");
    writeMessage(TESTFILE, "tester");

    std::string msg;

    ASSERT_TRUE(readMessage(TESTFILE, msg));
    EXPECT_EQ(msg, "test");

    ASSERT_TRUE(readMessage(TESTFILE, msg));
    EXPECT_EQ(msg, "testtest");

    ASSERT_TRUE(readMessage(TESTFILE, msg));
    EXPECT_EQ(msg, "tester");
}


TEST_F(MessageManagerTest, ReadEmptyQueue)
{
    std::string msg;
    ASSERT_FALSE(readMessage(TESTFILE, msg)); 
}


TEST_F(MessageManagerTest, QueueOverflow)
{
    for (int i = 0; i < 5; i++)
        ASSERT_TRUE(writeMessage(TESTFILE, "text"));

    ASSERT_FALSE(writeMessage(TESTFILE, "textErr"));
}


TEST_F(MessageManagerTest, RingBuffer)
{
    writeMessage(TESTFILE, "1");
    writeMessage(TESTFILE, "2");
    writeMessage(TESTFILE, "3");
    writeMessage(TESTFILE, "4");
    writeMessage(TESTFILE, "5");

    std::string message;
    readMessage(TESTFILE, message);
    readMessage(TESTFILE, message);

    ASSERT_TRUE(writeMessage(TESTFILE, "6"));
    ASSERT_TRUE(writeMessage(TESTFILE, "7"));

    readMessage(TESTFILE, message);
    EXPECT_EQ(message, "3");
    readMessage(TESTFILE, message);
    EXPECT_EQ(message, "4");
    readMessage(TESTFILE, message);
    EXPECT_EQ(message, "5");
    readMessage(TESTFILE, message);
    EXPECT_EQ(message, "6");
    readMessage(TESTFILE, message);
    EXPECT_EQ(message, "7");

    ASSERT_FALSE(readMessage(TESTFILE, message));
}


TEST_F(MessageManagerTest, LongMessages)
{
    std::string longMessage = "sss67s676s76s88s8sss8s8s8s8s8s8s8sssss"; 
    ASSERT_TRUE(writeMessage(TESTFILE, longMessage));

    std::string read;
    ASSERT_TRUE(readMessage(TESTFILE, read));

    EXPECT_EQ(read.size(), 20);
}

