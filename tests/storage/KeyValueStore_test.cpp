#include "storage/KeyValueStore.h"
#include <gtest/gtest.h>

using namespace storage;

// String operations
TEST(KeyValueStoreTest, StringSetGetExistsDel)
{
    KeyValueStore kv;
    kv.set("foo", "bar");
    EXPECT_EQ(kv.get("foo"), "bar");
    EXPECT_TRUE(kv.exists("foo"));
    EXPECT_TRUE(kv.del("foo"));
    EXPECT_FALSE(kv.exists("foo"));
    EXPECT_THROW(kv.get("foo"), std::runtime_error);
}

// List operations
TEST(KeyValueStoreTest, ListPushPopRange)
{
    KeyValueStore kv;
    kv.rpush("mylist", "a");
    kv.rpush("mylist", "b");
    kv.lpush("mylist", "c");
    // mylist: c, a, b
    auto vals = kv.lrange("mylist", 0, -1);
    ASSERT_EQ(vals.size(), 3);
    EXPECT_EQ(vals[0], "c");
    EXPECT_EQ(vals[1], "a");
    EXPECT_EQ(vals[2], "b");
    EXPECT_EQ(kv.lpop("mylist"), "c");
    EXPECT_EQ(kv.rpop("mylist"), "b");
    vals = kv.lrange("mylist", 0, -1);
    ASSERT_EQ(vals.size(), 1);
    EXPECT_EQ(vals[0], "a");
    EXPECT_EQ(kv.lpop("mylist"), "a");
    EXPECT_THROW(kv.lpop("mylist"), std::runtime_error); // after popping last element
}

// Set operations
TEST(KeyValueStoreTest, SetAddRemoveMembers)
{
    KeyValueStore kv;
    kv.sadd("myset", "x");
    kv.sadd("myset", "y");
    kv.sadd("myset", "z");
    auto members = kv.smembers("myset");
    EXPECT_EQ(members.size(), 3);
    EXPECT_TRUE(members.count("x"));
    EXPECT_TRUE(members.count("y"));
    EXPECT_TRUE(members.count("z"));
    kv.srem("myset", "y");
    members = kv.smembers("myset");
    EXPECT_EQ(members.size(), 2);
    EXPECT_FALSE(members.count("y"));
}

// Hash operations
TEST(KeyValueStoreTest, HashSetGetDelAll)
{
    KeyValueStore kv;
    EXPECT_TRUE(kv.hset("myhash", "field1", "val1"));
    EXPECT_FALSE(kv.hset("myhash", "field1", "val2")); // overwrite
    EXPECT_EQ(kv.hget("myhash", "field1"), "val2");
    EXPECT_TRUE(kv.hdel("myhash", "field1"));
    EXPECT_FALSE(kv.hdel("myhash", "field1"));
    kv.hset("myhash", "a", "1");
    kv.hset("myhash", "b", "2");
    auto all = kv.hgetall("myhash");
    EXPECT_EQ(all.size(), 2);
    EXPECT_EQ(all["a"], "1");
    EXPECT_EQ(all["b"], "2");
    EXPECT_THROW(kv.hget("myhash", "field1"), std::runtime_error);
}

// Sorted Set operations
TEST(KeyValueStoreTest, ZSetAddRemoveRange)
{
    KeyValueStore kv;
    kv.zadd("myzset", 1.0, "one");
    kv.zadd("myzset", 2.0, "two");
    kv.zadd("myzset", 0.5, "zero");
    auto vals = kv.zrange("myzset", 0, -1);
    ASSERT_EQ(vals.size(), 3);
    EXPECT_EQ(vals[0], "zero");
    EXPECT_EQ(vals[1], "one");
    EXPECT_EQ(vals[2], "two");
    kv.zrem("myzset", "one");
    vals = kv.zrange("myzset", 0, -1);
    ASSERT_EQ(vals.size(), 2);
    EXPECT_EQ(vals[0], "zero");
    EXPECT_EQ(vals[1], "two");
    EXPECT_THROW(kv.zrange("notfound", 0, -1), std::runtime_error);
}

// Type safety
TEST(KeyValueStoreTest, TypeSafety)
{
    KeyValueStore kv;
    kv.set("foo", "bar");
    EXPECT_NO_THROW(kv.lpush("foo", "baz")); // Overwrites "foo" as a list
    kv.lpush("mylist", "a");
    EXPECT_NO_THROW(kv.set("mylist", "str")); // Overwrites "mylist" as a string
    kv.sadd("myset", "x");
    EXPECT_THROW(kv.hget("myset", "field"), std::runtime_error); // Wrong type get
}
