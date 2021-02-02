﻿#include "tkc/fscript.h"
#include "tkc/object_default.h"
#include "gtest/gtest.h"

TEST(FScript, basic1) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2)", &v);
  ASSERT_EQ(3, value_int(&v));
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, basic2) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2);sum(1, 2, 3, 4)", &v);
  ASSERT_EQ(10, value_int(&v));
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, basic3) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2)\nsum(1, 2, 3, 4)\nsum(123, 321)", &v);
  ASSERT_EQ(444, value_int(&v));
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, basic4) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "print(sum(1, 2))", &v);
  ASSERT_EQ(true, value_bool(&v));
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, if1) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "if(1, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "hello");
  value_reset(&v);

  fscript_eval(obj, "if(0, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "world");
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, while1) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "set(a,0);set(b,0);while(<=(a,100), set(b, +(b, a)),set(a, +(a,1)));int(b)", &v);
  ASSERT_EQ(value_int(&v), 5050);
  value_reset(&v);
}

TEST(FScript, if2) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "if(true, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "hello");
  value_reset(&v);

  fscript_eval(obj, "if(false, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "world");
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, if3) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "if(true, sum(1, 2), sum(1, 2, 3)", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "if(false, sum(1, 2), sum(1, 2, 3)", &v);
  ASSERT_EQ(value_int(&v), 6);
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, convert) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "int(\"3\")", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "float(\"3.5\")", &v);
  ASSERT_EQ(value_float(&v), 3.5);
  value_reset(&v);

  fscript_eval(obj, "str(123)", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  fscript_eval(obj, "str(int(123))", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, join) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "join(\",\",\"a\", \"b\", \"c\")", &v);
  ASSERT_STREQ(value_str(&v), "a,b,c");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, set) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "set(a, 123)sum(a, 321)", &v);
  ASSERT_EQ(value_int(&v), 444);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, iformat) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "iformat(\"hello:%d\", 123)", &v);
  ASSERT_STREQ(value_str(&v), "hello:123");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, fformat) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "fformat(\"hello:%lf\", 123)", &v);
  ASSERT_STREQ(value_str(&v), "hello:123.000000");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, mod) {
  value_t v;
  object_t* obj = object_default_create();
  fscript_eval(obj, "%(23, 7)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, and) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "and(true, true)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "and(true, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "and(false, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, or) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "or(true, true)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "||(true, false)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "||(false, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, not ) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "not(true)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "!(false)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, bit_and) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "&(1, 1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "&(1, 2)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, bit_or) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "|(1, 1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "|(1, 2)", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, bit_not) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "~(1)", &v);
  ASSERT_EQ(value_int(&v), ~1);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, less) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "<(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "<(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, le) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "<=(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "<=(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, great) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, ">(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, ">(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, ge) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "ge(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "ge(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, eq) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "eq(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "eq(2, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "eq(\"2\", \"2\")", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);
  OBJECT_UNREF(obj);
}

TEST(FScript, trim) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "trim(\" aaa \")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  fscript_eval(obj, "trim(\"aaa \")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, len) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "len(\"aaa\")", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "len(\"\")", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, toupper) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "toupper(\"aaa\")", &v);
  ASSERT_STREQ(value_str(&v), "AAA");
  value_reset(&v);

  fscript_eval(obj, "trim(\"AAA\")", &v);
  ASSERT_STREQ(value_str(&v), "AAA");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, tolower) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "tolower(\"aaa\")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  fscript_eval(obj, "tolower(\"AAA\")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, replace) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "replace(\"aaa\", \"a\", \"B\")", &v);
  ASSERT_STREQ(value_str(&v), "BBB");
  value_reset(&v);

  fscript_eval(obj, "replace(\"aaa123\", \"a\", \"\")", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, contains) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "contains(\"aaa\", \"a\")", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "contains(\"aaa\", \"A\")", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, substr) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "substr(\"1234567\", 1, 3)", &v);
  ASSERT_STREQ(value_str(&v), "234");
  value_reset(&v);

  fscript_eval(obj, "substr(\"1234567\", 0, 3)", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  fscript_eval(obj, "substr(\"1234567\", 0, 300)", &v);
  ASSERT_STREQ(value_str(&v), "1234567");
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, min) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "min(100, 50)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  fscript_eval(obj, "min(50, 100)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, max) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "max(100, 50)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  fscript_eval(obj, "max(50, 100)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, clamp) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "clamp(100, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  fscript_eval(obj, "clamp(10, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  fscript_eval(obj, "clamp(1000, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 200);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, pow) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "pow(3, 2)", &v);
  ASSERT_EQ(value_int(&v), 9);
  value_reset(&v);

  fscript_eval(obj, "pow(3, 0)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, sqrt) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "sqrt(4)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FScript, complex) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "sub(sum(sqrt(4), sqrt(9)), 1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "if(true, 123, sub(sum(sqrt(4), sqrt(9)), 1))", &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  fscript_eval(obj, "if(false, 123, sub(sum(sqrt(4), sqrt(9)), 1))", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "sqrt(if(<(3, 1), 123, sub(sum(sqrt(4), sqrt(9)), 1)))", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

static ret_t func_foo(fscript_t* fscript, fscript_args_t* args, value_t* v) {
  value_set_int(v, 123);

  return RET_OK;
}

TEST(FScript, func) {
  value_t v;
  object_t* obj = object_default_create();
  object_set_prop_pointer(obj, "function.foo", (void*)func_foo);

  fscript_eval(obj, "foo()", &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  OBJECT_UNREF(obj);
}
