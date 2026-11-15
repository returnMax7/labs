#include "CppUnitTest.h"
#include "../functions.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace
{
    struct Buffer
    {
        char* data;

        Buffer(const char* text, std::size_t size)
            : data(static_cast<char*>(std::malloc(size)))
        {
            Assert::IsNotNull(data);
            std::memcpy(data, text, size);
        }

        ~Buffer() { std::free(data); }
    };

    struct TempDirectory
    {
        fs::path path = fs::temp_directory_path() /
            ("lab2-tests-" + std::to_string(std::random_device{}()));

        TempDirectory() { Assert::IsTrue(fs::create_directory(path)); }
        ~TempDirectory()
        {
            std::error_code error;
            fs::remove_all(path, error);
        }
    };

    std::string ReadFile(const fs::path& path)
    {
        std::ifstream file(path);
        Assert::IsTrue(file.is_open());
        return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    }

    void CheckZadanie1(const char* text, const char* expected)
    {
        int length = static_cast<int>(std::strlen(text));
        Buffer buffer(text, length);

        Assert::IsTrue(Zadanie1(buffer.data, length));
        Assert::AreEqual(static_cast<int>(std::strlen(expected)), length);
        Assert::AreEqual(expected, std::string(buffer.data, length).c_str());
    }

    void CheckZadanie2(const char* text, const char* expected)
    {
        Buffer buffer(text, std::strlen(text) + 1);

        Assert::IsTrue(Zadanie2(buffer.data));
        Assert::AreEqual('\0', buffer.data[std::strlen(expected)]);
        Assert::AreEqual(expected, static_cast<const char*>(buffer.data));
    }

    void CheckZadanie3(const char* text, const char* expected)
    {
        const TempDirectory dir;
        const auto source = dir.path / "input.txt";
        const auto result = dir.path / "output.txt";
        const auto keyboard = dir.path / "keyboard.txt";
        const std::unique_ptr<FILE, decltype(&std::fclose)> input(
            _wfopen(keyboard.c_str(), L"w+"), &std::fclose);
        Assert::IsNotNull(input.get());
        Assert::IsTrue(std::fputs(text, input.get()) != EOF);
        Assert::IsTrue(std::fputs("\\0\n", input.get()) != EOF);
        std::rewind(input.get());

        Assert::IsTrue(Zadanie3(source.string().c_str(), result.string().c_str(), input.get()));
        Assert::AreEqual(text, ReadFile(source).c_str());
        Assert::AreEqual(expected, ReadFile(result).c_str());
    }
}

TEST_CLASS(Lab2Tests)
{
public:
    TEST_METHOD(Zadanie1_RemovesFourVowels)
    {
        CheckZadanie1("Мама Ё-Y!", "Мм -!");
    }

    TEST_METHOD(Zadanie1_PreservesFiveVowels)
    {
        CheckZadanie1("АeИoУ!", "АeИoУ!");
    }

    TEST_METHOD(Zadanie1_DoublesSixVowels)
    {
        CheckZadanie1("aБEёИyУ!", "AaБEeЁёИиYyУу!");
    }

    TEST_METHOD(Zadanie2_RemovesFourVowels)
    {
        CheckZadanie2("ЁяEo", "");
    }

    TEST_METHOD(Zadanie2_PreservesFiveVowels)
    {
        CheckZadanie2("АeИoУ!", "АeИoУ!");
    }

    TEST_METHOD(Zadanie2_DoublesSixVowels)
    {
        CheckZadanie2("aБEёИyУ!", "AaБEeЁёИиYyУу!");
    }

    TEST_METHOD(Zadanie3_RemovesFourVowels)
    {
        CheckZadanie3("Мама\nЁП-Y!", "Мм\nП-!");
    }

    TEST_METHOD(Zadanie3_PreservesFiveVowels)
    {
        CheckZadanie3("АeИoУ!", "АeИoУ!");
    }

    TEST_METHOD(Zadanie3_DoublesSixVowels)
    {
        CheckZadanie3("aБEёИyУ!", "AaБEeЁёИиYyУу!");
    }
};
