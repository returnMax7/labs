"""Run with: python test_lab2.py path/to/lab2.exe"""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


ENCODING = 'cp1251'


def run_lab(data, directory):
    result = subprocess.run(
        [str(EXECUTABLE)], input=data.encode(ENCODING), cwd=directory,
        capture_output=True, timeout=15,
    )
    return result.returncode, result.stdout.decode(ENCODING).replace('\r\n', '\n')


class Lab2Tests(unittest.TestCase):
    def check_all_variants(self, text, expected):
        self.check_inputs(text, text, text, expected, expected, expected)

    def check_inputs(self, first, second, third, expected1, expected2, expected3, suffix=''):
        with tempfile.TemporaryDirectory(prefix='lab2-') as temp:
            directory = Path(temp)
            (directory / 'input.txt').write_text('old input', encoding=ENCODING)
            (directory / 'output.txt').write_text('old output', encoding=ENCODING)
            status, output = run_lab(f'{first}\n{second}\\0{suffix}\n{third}\\0{suffix}\n', directory)
            self.assertEqual(status, 0, output)
            console1 = output.split('\nЗадание 1.', 1)[1].split('\n\nЗадание 2.', 1)[0]
            console2 = output.split('\nЗадание 2.', 1)[1].split('\n\nЗадание 3.', 1)[0]
            with self.subTest(variant=1):
                self.assertEqual(console1.split('\nРезультат: ', 1)[1], expected1)
            with self.subTest(variant=2):
                self.assertEqual(console2.split('\nРезультат: ', 1)[1], expected2)
            with self.subTest(variant=3, file='input.txt'):
                self.assertEqual((directory / 'input.txt').read_text(encoding=ENCODING), third)
            with self.subTest(variant=3, file='output.txt'):
                self.assertEqual((directory / 'output.txt').read_text(encoding=ENCODING), expected3)

    def test_empty(self):
        self.check_all_variants('', '')

    def test_no_vowels_and_literal_zero(self):
        self.check_all_variants('Брр! 0123 \\x', 'Брр! 0123 \\x')

    def test_remove_vowels(self):
        self.check_all_variants('Мама Ё-Y!', 'Мм -!')

    def test_remove_entire_text(self):
        self.check_all_variants('ЁяEo', '')

    def test_five_vowels_preserve_case(self):
        self.check_all_variants('АeИoУ!', 'АeИoУ!')

    def test_double_vowels(self):
        self.check_all_variants('aБEёИyУ!', 'AaБEeЁёИиYyУу!')

    def test_all_vowels_in_both_cases(self):
        vowels = 'AaEeIiOoUuYyАаЕеЁёИиОоУуЫыЭэЮюЯя'
        for index in range(0, len(vowels), 2):
            pair = vowels[index:index + 2]
            with self.subTest(letter=pair):
                self.check_all_variants('x' + pair * 3 + '!', 'x' + pair * 6 + '!')

    def test_capacity_boundaries(self):
        for length in (1, 4, 5, 6, 9, 10, 11):
            with self.subTest(length=length):
                self.check_all_variants('b' * length, 'b' * length)

    def test_multiple_allocations(self):
        self.check_all_variants('яб' * 1200, 'Яяб' * 1200)

    def test_different_inputs_and_multiline_file(self):
        self.check_inputs('a\\0b', 'Мама\n!', 'Мама\nОП!', '\\0b', 'Мм\n!', 'Мм\nП!')

    def test_marker_ignores_rest_of_line(self):
        self.check_inputs('b', 'Мама', 'ЁЁЁЁЁЁ', 'b', 'Мм', 'Ёё' * 6, suffix='ignored')

    def test_cannot_write_input(self):
        with tempfile.TemporaryDirectory(prefix='lab2-') as temp:
            directory = Path(temp)
            (directory / 'input.txt').mkdir()
            status, output = run_lab('a\na\\0\na\\0\n', directory)
            self.assertEqual(status, 1, output)
            self.assertIn('Ошибка', output)
            self.assertFalse((directory / 'output.txt').exists())

    def test_cannot_write_result(self):
        with tempfile.TemporaryDirectory(prefix='lab2-') as temp:
            directory = Path(temp)
            (directory / 'output.txt').mkdir()
            status, output = run_lab('a\na\\0\nМама\\0\n', directory)
            self.assertEqual(status, 1, output)
            self.assertIn('Ошибка', output)
            self.assertEqual((directory / 'input.txt').read_text(encoding=ENCODING), 'Мама')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    EXECUTABLE = Path(sys.argv.pop(1)).resolve(strict=True)
    unittest.main()
