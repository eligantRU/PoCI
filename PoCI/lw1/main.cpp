#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

enum class Mode
{
	Encrypt,
	Decrypt,
};

const std::set<int8_t> alphabet = {
	//'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'\r', '\n',
	'.', ',', '?', '!', '-',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'А', 'Б', 'В', 'Г', 'Д', 'Е', 'Ё', 'Ж', 'З', 'И', 'Й', 'К', 'Л', 'М', 'Н', 'О', 'П', 'Р', 'С', 'Т', 'У', 'Ф', 'Х', 'Ц', 'Ч', 'Ш', 'Щ', 'Ъ', 'Ы', 'Ь', 'Э', 'Ю', 'Я',
	'а', 'б', 'в', 'г', 'д', 'е', 'ё', 'ж', 'з', 'и', 'й', 'к', 'л', 'м', 'н', 'о', 'п', 'р', 'с', 'т', 'у', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ъ', 'ы', 'ь', 'э', 'ю', 'я',
};

void ValidateMessage(const std::vector<int8_t>& msg)
{
	for (const auto& el : msg)
	{
		if (!alphabet.count(el))
		{
			throw std::runtime_error("Invalid <MESSAGE>");
		}
	}
}

Mode ToMode(const std::string& mode)
{
	if (mode == "E")
	{
		return Mode::Encrypt;
	}
	else if (mode == "D")
	{
		return Mode::Decrypt;
	}
	throw std::invalid_argument("Unknown <MODE>: '" + mode + "'");
}

std::string ToKey(const std::string& key, size_t msgLength)
{
	for (const auto& el : key)
	{
		if (!alphabet.count(el))
		{
			throw std::invalid_argument("Invalid <KEY>: '" + key + "'");
		}
	}

	std::string result;
	result.reserve(msgLength);

	for (size_t i = 0; i < msgLength; ++i)
	{
		result.push_back(key[i % key.size()]);
	}
	return result;
}

size_t AlphabetIndex(int8_t el)
{
	return std::distance(alphabet.cbegin(), std::find(alphabet.cbegin(), alphabet.cend(), el));
}

std::vector<int8_t> Encrypt(const std::vector<int8_t>& msg, const std::string& key, const std::vector<std::vector<int8_t>>& table)
{
	std::vector<int8_t> encrypted;
	encrypted.reserve(msg.size());
	for (size_t i = 0; i < msg.size(); ++i)
	{
		const auto column = AlphabetIndex(msg[i]);
		const auto row = AlphabetIndex(key[i]);
		encrypted.push_back(table[row][column]);
	}
	return encrypted;
}

std::vector<int8_t> Decrypt(const std::vector<int8_t>& msg, const std::string& key, const std::vector<std::vector<int8_t>>& table)
{
	std::vector<int8_t> decrypted;
	decrypted.reserve(msg.size());
	for (size_t i = 0; i < msg.size(); ++i)
	{
		const auto column = AlphabetIndex(key[i]);
		const auto q = table[column];
		const auto w = std::distance(q.cbegin(), std::find(q.cbegin(), q.cend(), msg[i]));
		decrypted.push_back(table.front()[w]);
	}
	return decrypted;
}

std::vector<std::vector<int8_t>> GetTable()
{
	std::vector<std::vector<int8_t>> table;

	for (size_t i = 0; i < alphabet.size(); ++i)
	{
		std::vector<int8_t> alphabetCopy(alphabet.cbegin(), alphabet.cend());
		std::rotate(alphabetCopy.begin(), alphabetCopy.begin() + i, alphabetCopy.end());
		table.push_back(alphabetCopy);
	}
	return table;
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 5)
		{
			throw std::runtime_error("Invalid command line arguments: <EXE> <INPUT> <OUTPUT> <MODE> <KEY>");
		}

		std::ifstream input(argv[1], std::ios::binary);
		if (!input.is_open())
		{
			throw std::runtime_error("<INPUT> is not opened");
		}

		std::vector<int8_t> msg(std::istreambuf_iterator<char>(input), {});
		ValidateMessage(msg);

		const auto mode = ToMode(argv[3]);
		const auto key = ToKey(argv[4], msg.size());
		const auto table = GetTable();

		if (mode == Mode::Encrypt)
		{
			msg = Encrypt(msg, key, table);
		}
		else if (mode == Mode::Decrypt)
		{
			msg = Decrypt(msg, key, table);
		}

		std::ofstream output(argv[2], std::ios::binary);
		output.write((char*)&msg[0], msg.size());
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unkown error" << std::endl;
	}
}
