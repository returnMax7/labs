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
        char* data = nullptr;
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

    class Input
    {
    public:
        explicit Input(const char* text)
            : file(std::fopen((dir.path / "stdin.txt").string().c_str(), "w+"), &std::fclose)
        {
            Assert::IsNotNull(file.get());
            Assert::IsTrue(std::fputs(text, file.get()) != EOF);
            std::rewind(file.get());
        }

        FILE* get() const { return file.get(); }

    private:
        TempDirectory dir;
        std::unique_ptr<FILE, decltype(&std::fclose)> file;
    };

    std::string ReadFile(const fs::path& path)
    {
        std::ifstream file(path);
        Assert::IsTrue(file.is_open());
        return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    }

    void CheckArray(const char* input, const char* expected)
    {
        int length = static_cast<int>(std::strlen(input));
        Buffer buffer;
        if (length != 0)
        {
            buffer.data = static_cast<char*>(std::malloc(length));
            Assert::IsNotNull(buffer.data);
            std::memcpy(buffer.data, input, length);
        }
        Assert::IsTrue(Zadanie1(buffer.data, length));
        Assert::AreEqual(static_cast<int>(std::strlen(expected)), length);
        if (length != 0)
        {
            const std::string actual(buffer.data, length);
            Assert::AreEqual(expected, actual.c_str());
        }
    }

    void CheckCString(const char* input, const char* expected)
    {
        const auto size = std::strlen(input) + 1;
        Buffer buffer;
        buffer.data = static_cast<char*>(std::malloc(size));
        Assert::IsNotNull(buffer.data);
        std::memcpy(buffer.data, input, size);
        Assert::IsTrue(Zadanie2(buffer.data));
        Assert::AreEqual('\0', buffer.data[std::strlen(expected)]);
        Assert::AreEqual(expected, static_cast<const char*>(buffer.data));
    }
}

TEST_CLASS(Lab2Tests)
{
public:
    TEST_METHOD(ArrayHandlesEmptyInput)
    {
        CheckArray("", "");
    }

    TEST_METHOD(ArrayRemovesFewerThanFiveVowels)
    {
        CheckArray("Мама Ё-Y!", "Мм -!");
        CheckArray("ЁяEo", "");
    }

    TEST_METHOD(ArrayPreservesExactlyFiveVowels)
    {
        CheckArray("АeИoУ!", "АeИoУ!");
    }

    TEST_METHOD(ArrayDoublesAllVowelsInBothCases)
    {
        const std::string vowels = "AaEeIiOoUuYyАаЕеЁёИиОоУуЫыЭэЮюЯя";
        for (std::size_t i = 0; i < vowels.size(); i += 2)
        {
            const auto pair = vowels.substr(i, 2);
            const auto input = "b" + pair + pair + pair + "!";
            const auto expected = "b" + pair + pair + pair + pair + pair + pair + "!";
            CheckArray(input.c_str(), expected.c_str());
        }
    }

    TEST_METHOD(CStringPreservesEmptyAndConsonantOnlyInputs)
    {
        CheckCString("", "");
        CheckCString("Брр! 0123 \\x", "Брр! 0123 \\x");
    }

    TEST_METHOD(CStringTerminatesAfterRemovingVowels)
    {
        CheckCString("Мама", "Мм");
        CheckCString("ЁяEo", "");
    }

    TEST_METHOD(CStringPreservesExactlyFiveVowels)
    {
        CheckCString("АeИoУ!", "АeИoУ!");
    }

    TEST_METHOD(CStringTerminatesAfterExpandingMixedVowels)
    {
        CheckCString("aБEёИyУ!", "AaБEeЁёИиYyУу!");

        const std::string input(1200, 'я');
        std::string expected;
        for (std::size_t i = 0; i < input.size(); ++i)
        {
            expected += "Яя";
        }
        CheckCString(input.c_str(), expected.c_str());
    }

    TEST_METHOD(KeyboardInputStopsAtMarkerAndSkipsRestOfLine)
    {
        const auto input = Input("Мама\n!\\0ignored\nnext");
        Buffer buffer;
        Assert::IsTrue(ProchitatStroku(input.get(), buffer.data, true));
        Assert::AreEqual("Мама\n!", static_cast<const char*>(buffer.data));
        Assert::AreEqual(static_cast<int>('n'), std::fgetc(input.get()));
    }

    TEST_METHOD(KeyboardInputDistinguishesEmptyStringFromMissingInput)
    {
        const auto empty = Input("\\0\n");
        Buffer buffer;
        Assert::IsTrue(ProchitatStroku(empty.get(), buffer.data, true));
        Assert::AreEqual("", static_cast<const char*>(buffer.data));

        const auto missing = Input("");
        Buffer missingBuffer;
        Assert::IsFalse(ProchitatStroku(missing.get(), missingBuffer.data, true));
    }

    TEST_METHOD(FileReadPreservesDataAcrossBufferGrowth)
    {
        for (const int length : {0, 1, 4, 5, 6, 9, 10, 11, 2400})
        {
            const std::string text(length, 'б');
            const auto input = Input(text.c_str());
            Buffer buffer;
            Assert::IsTrue(ProchitatStroku(input.get(), buffer.data, false));
            Assert::AreEqual(text.c_str(), static_cast<const char*>(buffer.data));
        }

        const auto input = Input("abc\\0\nnext");
        Buffer buffer;
        Assert::IsTrue(ProchitatStroku(input.get(), buffer.data, false));
        Assert::AreEqual("abc\\0\nnext", static_cast<const char*>(buffer.data));
    }

    TEST_METHOD(FileVariantWritesOriginalAndProcessedText)
    {
        const TempDirectory dir;
        const auto source = dir.path / "input.txt";
        const auto result = dir.path / "output.txt";
        std::ofstream(source) << "old input";
        std::ofstream(result) << "old output";

        const auto input = Input("Мама\nОП!\\0\n");
        Assert::IsTrue(Zadanie3(source.string().c_str(), result.string().c_str(), input.get()));
        Assert::AreEqual("Мама\nОП!", ReadFile(source).c_str());
        Assert::AreEqual("Мм\nП!", ReadFile(result).c_str());
    }

    TEST_METHOD(FileVariantReportsUnwritableInput)
    {
        const TempDirectory dir;
        const auto source = dir.path / "input.txt";
        const auto result = dir.path / "output.txt";
        Assert::IsTrue(fs::create_directory(source));

        const auto input = Input("Мама\\0\n");
        Assert::IsFalse(Zadanie3(source.string().c_str(), result.string().c_str(), input.get()));
        Assert::IsFalse(fs::exists(result));
    }

    TEST_METHOD(FileVariantReportsUnwritableOutput)
    {
        const TempDirectory dir;
        const auto source = dir.path / "input.txt";
        const auto result = dir.path / "output.txt";
        Assert::IsTrue(fs::create_directory(result));

        const auto input = Input("Мама\\0\n");
        Assert::IsFalse(Zadanie3(source.string().c_str(), result.string().c_str(), input.get()));
        Assert::AreEqual("Мама", ReadFile(source).c_str());
    }
};
