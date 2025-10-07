#include "Codec.h"
#include "CommandProcessor.h"
#include "KeyValueStore.h"
#include <gtest/gtest.h>

using namespace command;
using namespace storage;
using namespace codec;

// Error handling tests
TEST(CommandProcessor, EmptyCommand)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd = array({});
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Error>(result.data));
    EXPECT_EQ(std::get<Error>(result.data).value, "ERR empty command");
}

TEST(CommandProcessor, UnknownCommand)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd = array({ bulk("UNKNOWN"), bulk("key") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Error>(result.data));
    EXPECT_EQ(std::get<Error>(result.data).value, "ERR unknown command 'UNKNOWN'");
}

TEST(CommandProcessor, WrongNumberOfArguments)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd = array({ bulk("SET"), bulk("key") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Error>(result.data));
    EXPECT_EQ(std::get<Error>(result.data).value, "ERR wrong number of arguments for 'set' command");
}

// String command tests
TEST(CommandProcessor, SetAndGet)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue setCmd = array({ bulk("SET"), bulk("mykey"), bulk("myvalue") });
    CodecValue setResult = processor.process(setCmd);

    ASSERT_TRUE(std::holds_alternative<SimpleString>(setResult.data));
    EXPECT_EQ(std::get<SimpleString>(setResult.data).value, "OK");

    CodecValue getCmd = array({ bulk("GET"), bulk("mykey") });
    CodecValue getResult = processor.process(getCmd);

    ASSERT_TRUE(std::holds_alternative<BulkString>(getResult.data));
    EXPECT_EQ(std::get<BulkString>(getResult.data).value, "myvalue");
}

TEST(CommandProcessor, GetNonExistentKey)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd = array({ bulk("GET"), bulk("nonexistent") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result.data));
    EXPECT_FALSE(std::get<BulkString>(result.data).value.has_value());
}

TEST(CommandProcessor, Del)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    store.set("key1", "value1");

    CodecValue cmd = array({ bulk("DEL"), bulk("key1") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Integer>(result.data));
    EXPECT_EQ(std::get<Integer>(result.data).value, 1);

    CodecValue cmd2 = array({ bulk("DEL"), bulk("key1") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 0);
}

TEST(CommandProcessor, Exists)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    store.set("key1", "value1");

    CodecValue cmd1 = array({ bulk("EXISTS"), bulk("key1") });
    CodecValue result1 = processor.process(cmd1);

    ASSERT_TRUE(std::holds_alternative<Integer>(result1.data));
    EXPECT_EQ(std::get<Integer>(result1.data).value, 1);

    CodecValue cmd2 = array({ bulk("EXISTS"), bulk("nonexistent") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 0);
}

TEST(CommandProcessor, CaseInsensitiveCommands)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd1 = array({ bulk("set"), bulk("key"), bulk("value") });
    CodecValue result1 = processor.process(cmd1);
    ASSERT_TRUE(std::holds_alternative<SimpleString>(result1.data));

    CodecValue cmd2 = array({ bulk("SeT"), bulk("key2"), bulk("value2") });
    CodecValue result2 = processor.process(cmd2);
    ASSERT_TRUE(std::holds_alternative<SimpleString>(result2.data));

    CodecValue cmd3 = array({ bulk("GET"), bulk("key") });
    CodecValue result3 = processor.process(cmd3);
    ASSERT_TRUE(std::holds_alternative<BulkString>(result3.data));
    EXPECT_EQ(std::get<BulkString>(result3.data).value, "value");
}

// List command tests
TEST(CommandProcessor, LPushAndLPop)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd1 = array({ bulk("LPUSH"), bulk("list"), bulk("value1") });
    CodecValue result1 = processor.process(cmd1);

    ASSERT_TRUE(std::holds_alternative<Integer>(result1.data));
    EXPECT_EQ(std::get<Integer>(result1.data).value, 1);

    CodecValue cmd2 = array({ bulk("LPUSH"), bulk("list"), bulk("value2") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 2);

    CodecValue cmd3 = array({ bulk("LPOP"), bulk("list") });
    CodecValue result3 = processor.process(cmd3);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result3.data));
    EXPECT_EQ(std::get<BulkString>(result3.data).value, "value2");
}

TEST(CommandProcessor, RPushAndRPop)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("RPUSH"), bulk("list"), bulk("value1") }));
    processor.process(array({ bulk("RPUSH"), bulk("list"), bulk("value2") }));

    CodecValue cmd = array({ bulk("RPOP"), bulk("list") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result.data));
    EXPECT_EQ(std::get<BulkString>(result.data).value, "value2");
}

TEST(CommandProcessor, LRange)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("RPUSH"), bulk("list"), bulk("a") }));
    processor.process(array({ bulk("RPUSH"), bulk("list"), bulk("b") }));
    processor.process(array({ bulk("RPUSH"), bulk("list"), bulk("c") }));

    CodecValue cmd = array({ bulk("LRANGE"), bulk("list"), bulk("0"), bulk("2") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Array>(result.data));
    const auto& arr = std::get<Array>(result.data);
    EXPECT_EQ(arr.elements.size(), 3);

    EXPECT_EQ(std::get<BulkString>(arr.elements[0].data).value, "a");
    EXPECT_EQ(std::get<BulkString>(arr.elements[1].data).value, "b");
    EXPECT_EQ(std::get<BulkString>(arr.elements[2].data).value, "c");
}

TEST(CommandProcessor, LPopEmptyList)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd = array({ bulk("LPOP"), bulk("emptylist") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result.data));
    EXPECT_FALSE(std::get<BulkString>(result.data).value.has_value());
}

// Set command tests
TEST(CommandProcessor, SAddAndSMembers)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd1 = array({ bulk("SADD"), bulk("myset"), bulk("member1") });
    CodecValue result1 = processor.process(cmd1);

    ASSERT_TRUE(std::holds_alternative<Integer>(result1.data));
    EXPECT_EQ(std::get<Integer>(result1.data).value, 1);

    processor.process(array({ bulk("SADD"), bulk("myset"), bulk("member2") }));

    CodecValue cmd3 = array({ bulk("SADD"), bulk("myset"), bulk("member1") });
    CodecValue result3 = processor.process(cmd3);

    ASSERT_TRUE(std::holds_alternative<Integer>(result3.data));
    EXPECT_EQ(std::get<Integer>(result3.data).value, 0);

    CodecValue cmd4 = array({ bulk("SMEMBERS"), bulk("myset") });
    CodecValue result4 = processor.process(cmd4);

    ASSERT_TRUE(std::holds_alternative<Array>(result4.data));
    const auto& arr = std::get<Array>(result4.data);
    EXPECT_EQ(arr.elements.size(), 2);
}

TEST(CommandProcessor, SRem)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("SADD"), bulk("myset"), bulk("member1") }));
    processor.process(array({ bulk("SADD"), bulk("myset"), bulk("member2") }));

    CodecValue cmd = array({ bulk("SREM"), bulk("myset"), bulk("member1") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Integer>(result.data));
    EXPECT_EQ(std::get<Integer>(result.data).value, 1);

    CodecValue cmd2 = array({ bulk("SREM"), bulk("myset"), bulk("member1") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 0);
}

// Hash command tests
TEST(CommandProcessor, HSetAndHGet)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd1 = array({ bulk("HSET"), bulk("myhash"), bulk("field1"), bulk("value1") });
    CodecValue result1 = processor.process(cmd1);

    ASSERT_TRUE(std::holds_alternative<Integer>(result1.data));
    EXPECT_EQ(std::get<Integer>(result1.data).value, 1);

    CodecValue cmd2 = array({ bulk("HGET"), bulk("myhash"), bulk("field1") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result2.data));
    EXPECT_EQ(std::get<BulkString>(result2.data).value, "value1");

    CodecValue cmd3 = array({ bulk("HSET"), bulk("myhash"), bulk("field1"), bulk("newvalue") });
    CodecValue result3 = processor.process(cmd3);

    ASSERT_TRUE(std::holds_alternative<Integer>(result3.data));
    EXPECT_EQ(std::get<Integer>(result3.data).value, 0);
}

TEST(CommandProcessor, HGetNonExistentField)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("HSET"), bulk("myhash"), bulk("field1"), bulk("value1") }));

    CodecValue cmd = array({ bulk("HGET"), bulk("myhash"), bulk("nonexistent") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<BulkString>(result.data));
    EXPECT_FALSE(std::get<BulkString>(result.data).value.has_value());
}

TEST(CommandProcessor, HDel)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("HSET"), bulk("myhash"), bulk("field1"), bulk("value1") }));

    CodecValue cmd = array({ bulk("HDEL"), bulk("myhash"), bulk("field1") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Integer>(result.data));
    EXPECT_EQ(std::get<Integer>(result.data).value, 1);

    CodecValue cmd2 = array({ bulk("HDEL"), bulk("myhash"), bulk("field1") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 0);
}

TEST(CommandProcessor, HGetAll)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("HSET"), bulk("myhash"), bulk("field1"), bulk("value1") }));
    processor.process(array({ bulk("HSET"), bulk("myhash"), bulk("field2"), bulk("value2") }));

    CodecValue cmd = array({ bulk("HGETALL"), bulk("myhash") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Array>(result.data));
    const auto& arr = std::get<Array>(result.data);
    EXPECT_EQ(arr.elements.size(), 4);
}

// Sorted Set command tests
TEST(CommandProcessor, ZAddAndZRange)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    CodecValue cmd1 = array({ bulk("ZADD"), bulk("myzset"), bulk("1.5"), bulk("member1") });
    CodecValue result1 = processor.process(cmd1);

    ASSERT_TRUE(std::holds_alternative<Integer>(result1.data));
    EXPECT_EQ(std::get<Integer>(result1.data).value, 1);

    processor.process(array({ bulk("ZADD"), bulk("myzset"), bulk("2.5"), bulk("member2") }));
    processor.process(array({ bulk("ZADD"), bulk("myzset"), bulk("0.5"), bulk("member3") }));

    CodecValue cmd2 = array({ bulk("ZRANGE"), bulk("myzset"), bulk("0"), bulk("2") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Array>(result2.data));
    const auto& arr = std::get<Array>(result2.data);
    EXPECT_EQ(arr.elements.size(), 3);

    EXPECT_EQ(std::get<BulkString>(arr.elements[0].data).value, "member3");
    EXPECT_EQ(std::get<BulkString>(arr.elements[1].data).value, "member1");
    EXPECT_EQ(std::get<BulkString>(arr.elements[2].data).value, "member2");
}

TEST(CommandProcessor, ZRem)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("ZADD"), bulk("myzset"), bulk("1.0"), bulk("member1") }));
    processor.process(array({ bulk("ZADD"), bulk("myzset"), bulk("2.0"), bulk("member2") }));

    CodecValue cmd = array({ bulk("ZREM"), bulk("myzset"), bulk("member1") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Integer>(result.data));
    EXPECT_EQ(std::get<Integer>(result.data).value, 1);

    CodecValue cmd2 = array({ bulk("ZREM"), bulk("myzset"), bulk("member1") });
    CodecValue result2 = processor.process(cmd2);

    ASSERT_TRUE(std::holds_alternative<Integer>(result2.data));
    EXPECT_EQ(std::get<Integer>(result2.data).value, 0);
}

TEST(CommandProcessor, ZAddUpdateScore)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("ZADD"), bulk("myzset"), bulk("1.0"), bulk("member1") }));

    CodecValue cmd = array({ bulk("ZADD"), bulk("myzset"), bulk("2.0"), bulk("member1") });
    CodecValue result = processor.process(cmd);

    ASSERT_TRUE(std::holds_alternative<Integer>(result.data));
    EXPECT_EQ(std::get<Integer>(result.data).value, 0);
}

// Integration test
TEST(CommandProcessor, MultipleOperations)
{
    KeyValueStore store;
    CommandProcessor processor(store);

    processor.process(array({ bulk("SET"), bulk("key1"), bulk("value1") }));
    processor.process(array({ bulk("SET"), bulk("key2"), bulk("value2") }));
    processor.process(array({ bulk("RPUSH"), bulk("list1"), bulk("a") }));
    processor.process(array({ bulk("RPUSH"), bulk("list1"), bulk("b") }));
    processor.process(array({ bulk("SADD"), bulk("set1"), bulk("member1") }));

    CodecValue result1 = processor.process(array({ bulk("GET"), bulk("key1") }));
    ASSERT_TRUE(std::holds_alternative<BulkString>(result1.data));
    EXPECT_EQ(std::get<BulkString>(result1.data).value, "value1");

    CodecValue result2 = processor.process(array({ bulk("LRANGE"), bulk("list1"), bulk("0"), bulk("1") }));
    ASSERT_TRUE(std::holds_alternative<Array>(result2.data));
    EXPECT_EQ(std::get<Array>(result2.data).elements.size(), 2);

    CodecValue result3 = processor.process(array({ bulk("SMEMBERS"), bulk("set1") }));
    ASSERT_TRUE(std::holds_alternative<Array>(result3.data));
    EXPECT_EQ(std::get<Array>(result3.data).elements.size(), 1);
}
