#include <sstream>
#include <fstream>

#include <gtest/gtest.h>
#include <argparser.h>

using namespace ArgumentParser;

/*
    Функция принимает в качество аргумента строку, разделяет ее по "пробелу"
    и возвращает вектор полученных слов
*/
std::vector<std::string> split_string(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}


TEST(ArgParserTestSuite, EmptyTest) {
    ArgParser parser("My Empty Parser");

    ASSERT_TRUE(parser.parse(split_string("app")));
}


TEST(ArgParserTestSuite, StringTest) {
    ArgParser parser("My Parser");
    parser.add_string_argument("param1");

    ASSERT_TRUE(parser.parse(split_string("app --param1=value1")));
    ASSERT_EQ(parser.get_string_value("param1"), "value1");
}


TEST(ArgParserTestSuite, ShortNameTest) {
    ArgParser parser("My Parser");
    parser.add_string_argument('p', "param1");

    ASSERT_TRUE(parser.parse(split_string("app -p=value1")));
    ASSERT_EQ(parser.get_string_value("param1"), "value1");
}


TEST(ArgParserTestSuite, DefaultTest) {
    ArgParser parser("My Parser");
    parser.add_string_argument("param1").set_default_value("value1");

    ASSERT_TRUE(parser.parse(split_string("app")));
    ASSERT_EQ(parser.get_string_value("param1"), "value1");
}


TEST(ArgParserTestSuite, NoDefaultTest) {
    ArgParser parser("My Parser");
    parser.add_string_argument("param1");

    ASSERT_FALSE(parser.parse(split_string("app")));
}


TEST(ArgParserTestSuite, StoreValueTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.add_string_argument("param1").store_value(value);

    ASSERT_TRUE(parser.parse(split_string("app --param1=value1")));
    ASSERT_EQ(value, "value1");
}


TEST(ArgParserTestSuite, MultiStringTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.add_string_argument("param1").store_value(value);
    parser.add_string_argument('a', "param2");

    ASSERT_TRUE(parser.parse(split_string("app --param1=value1 --param2=value2")));
    ASSERT_EQ(parser.get_string_value("param2"), "value2");
}


TEST(ArgParserTestSuite, IntTest) {
    ArgParser parser("My Parser");
    parser.add_int_argument("param1");

    ASSERT_TRUE(parser.parse(split_string("app --param1=100500")));
    ASSERT_EQ(parser.get_int_value("param1"), 100500);
}


TEST(ArgParserTestSuite, MultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    parser.add_int_argument('p', "param1").mark_multi_value().store_values(int_values);

    ASSERT_TRUE(parser.parse(split_string("app --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(parser.get_int_value("param1", 0), 1);
    ASSERT_EQ(int_values[1], 2);
    ASSERT_EQ(int_values[2], 3);
}


TEST(ArgParserTestSuite, MinCountMultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 10;
    parser.add_int_argument('p', "param1").mark_multi_value(MinArgsCount).store_values(int_values);

    ASSERT_FALSE(parser.parse(split_string("app --param1=1 --param1=2 --param1=3")));
}


TEST(ArgParserTestSuite, FlagTest) {
    ArgParser parser("My Parser");
    parser.add_flag('f', "flag1");

    ASSERT_TRUE(parser.parse(split_string("app --flag1")));
    ASSERT_TRUE(parser.get_flag("flag1"));
}


TEST(ArgParserTestSuite, FlagsTest) {
    ArgParser parser("My Parser");
    bool flag3 ;
    parser.add_flag('a', "flag1");
    parser.add_flag('b', "flag2").set_default_value(true);
    parser.add_flag('c', "flag3").store_value(flag3);

    ASSERT_TRUE(parser.parse(split_string("app -ac")));
    ASSERT_TRUE(parser.get_flag("flag1"));
    ASSERT_TRUE(parser.get_flag("flag2"));
    ASSERT_TRUE(flag3);
}


TEST(ArgParserTestSuite, PositionalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.add_int_argument("Param1").mark_multi_value(1).mask_positional().store_values(values);

    ASSERT_TRUE(parser.parse(split_string("app 1 2 3 4 5")));
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, PositionalAndNormalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.add_flag('f', "flag", "Flag");
    parser.add_int_argument('n', "number", "Some Number");
    parser.add_int_argument("Param1").mark_multi_value(1).mask_positional().store_values(values);

    ASSERT_TRUE(parser.parse(split_string("app -n 0 1 2 3 4 5 -f")));
    ASSERT_TRUE(parser.get_flag("flag"));
    ASSERT_EQ(parser.get_int_value("number"), 0);
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, RepeatedParsingTest) {
    ArgParser parser("My Parser");
    parser.add_help('h', "help", "Some Description about program");
    parser.add_string_argument('i', "input", "File path for input file");
    parser.add_string_argument('o', "output", "File path for output directory");
    parser.add_flag('s', "flag1", "Read first number");
    parser.add_flag('p', "flag2", "Read second number");
    parser.add_int_argument("number", "Some Number");

    ASSERT_TRUE(parser.parse(split_string("app --number 2 -s -i test -o=test")));

    if (parser.get_flag("flag1")) {
      parser.add_int_argument("first", "First Number");
    } else if (parser.get_flag("flag2")) {
      parser.add_int_argument("second", "Second Number");
    }

    ASSERT_TRUE(parser.parse(split_string("app --number 2 -s -i test -o=test --first=52")));
    ASSERT_EQ(parser.get_int_value("first"), 52);
}


TEST(ArgParserTestSuite, HelpTest) {
    ArgParser parser("My Parser");
    parser.add_help('h', "help", "Some Description about program");

    ASSERT_TRUE(parser.parse(split_string("app --help")));
    ASSERT_TRUE(parser.help());
}


TEST(ArgParserTestSuite, HelpStringTest) {
    ArgParser parser("My Parser");
    parser.add_help('h', "help", "Some Description about program");
    parser.add_string_argument('i', "input", "File path for input file").mark_multi_value(1);
    parser.add_flag('s', "flag1", "Use some logic").set_default_value(true);
    parser.add_flag('p', "flag2", "Use some logic");
    parser.add_int_argument("numer", "Some Number");


    ASSERT_TRUE(parser.parse(split_string("app --help")));
    // Проверка закоментирована намеренно. Ождиается, что результат вызова функции будет приблизительно такой же,
    // но не с точностью до символа

    // ASSERT_EQ(
    //     parser.get_help_description(),
    //     "My Parser\n"
    //     "Some Description about program\n"
    //     "\n"
    //     "-i,  --input=<string>,  File path for input file [repeated, min args = 1]\n"
    //     "-s,  --flag1,  Use some logic [default = true]\n"
    //     "-p,  --flag2,  Use some logic\n"
    //     "     --number=<int>,  Some Number\n"
    //     "\n"
    //     "-h, --help Display this help and exit\n"
    // );
}
