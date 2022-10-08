#ifndef FLAG_HH__HEADER_GUARD__
#define FLAG_HH__HEADER_GUARD__

#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector
#include <cstdint>       // std::int64_t
#include <cstdlib>       // std::size_t, std::malloc, std::free
#include <string>        // std::string
#include <cstring>       // std::strlen, std::strncpy
#include <stdexcept>     // std::runtime_error
#include <variant>       // std::variant
#include <ostream>       // std::ostream

class Flag {
public:
	void Bool(const std::string &p_name, bool p_default, const char *p_desc) {
		m_flags[p_name] = Value(Type::Bool, p_default, p_desc);
	}

	void Int64(const std::string &p_name, std::int64_t p_default, const char *p_desc) {
		m_flags[p_name] = Value(Type::Int64, p_default, p_desc);
	}

	void Size(const std::string &p_name, std::size_t p_default, const char *p_desc) {
		m_flags[p_name] = Value(Type::Size, p_default, p_desc);
	}

	void Float(const std::string &p_name, float p_default, const char *p_desc) {
		m_flags[p_name] = Value(Type::Float, p_default, p_desc);
	}

	void Str(const std::string &p_name, const std::string &p_default, const char *p_desc) {
		m_flags[p_name] = Value(Type::Str, p_default, p_desc);
	}

	void Alias(const std::string &p_name, const std::string &p_alias) {
		m_aliases[p_alias] = p_name;
	}

	bool Bool(std::string p_name) {
		if (m_aliases.count(p_name) != 0)
			p_name = m_aliases.at(p_name);

		Value &flag = m_flags.at(p_name);
		if (flag.type != Type::Bool)
			throw std::runtime_error("bool Get() requested, but flag '" + p_name + "' is not bool");

		return std::get<bool>(flag.data);
	}

	std::int64_t Int64(std::string p_name) {
		if (m_aliases.count(p_name) != 0)
			p_name = m_aliases.at(p_name);

		Value &flag = m_flags.at(p_name);
		if (flag.type != Type::Int64)
			throw std::runtime_error("int64 Get() requested, but flag '" +
			                         p_name + "' is not int64");

		return std::get<std::int64_t>(flag.data);
	}

	std::size_t Size(std::string p_name) {
		if (m_aliases.count(p_name) != 0)
			p_name = m_aliases.at(p_name);

		Value &flag = m_flags.at(p_name);
		if (flag.type != Type::Size)
			throw std::runtime_error("size Get() requested, but flag '" + p_name + "' is not size");

		return std::get<std::size_t>(flag.data);
	}

	float Float(std::string p_name) {
		if (m_aliases.count(p_name) != 0)
			p_name = m_aliases.at(p_name);

		Value &flag = m_flags.at(p_name);
		if (flag.type != Type::Float)
			throw std::runtime_error("float Get() requested, but flag '" +
			                         p_name + "' is not float");

		return std::get<float>(flag.data);
	}

	std::string Str(std::string p_name) {
		if (m_aliases.count(p_name) != 0)
			p_name = m_aliases.at(p_name);

		Value &flag = m_flags.at(p_name);
		if (flag.type != Type::Str)
			throw std::runtime_error("str Get() requested, but flag '" + p_name + "' is not str");

		return std::get<std::string>(flag.data);
	}

	void PrintDefaults(std::ostream &p_stream, std::size_t p_indent) {
		std::string indent(p_indent, ' ');

		for (const auto &[key, value] : m_flags) {
			p_stream << indent << "-" + key;

			for (const auto &[alias, name] : m_aliases) {
				if (key == name)
					p_stream << ", -" + alias;
			}

			p_stream << "  " << value.TypeToStr() << ": ";

			switch (value.type) {
			case Type::Bool:  p_stream << (std::get<bool>(value.defData)? "true" : "false"); break;
			case Type::Int64: p_stream << std::get<std::int64_t>(value.defData); break;
			case Type::Size:  p_stream << std::get<std::size_t> (value.defData); break;
			case Type::Float: p_stream << std::get<float>       (value.defData); break;
			case Type::Str:   p_stream << std::get<std::string> (value.defData); break;
			}

			p_stream << '\n' << indent << "    " << value.desc << std::endl;
		}
	}

	bool Parse(int p_argc, char **p_argv) {
		m_args.clear();

		++ p_argv;
		-- p_argc;

		for (int i = 0; i < p_argc; ++ i) {
			if (p_argv[i][0] != '-') {
				m_args.push_back(p_argv[i]);

				continue;
			}

			std::string arg = p_argv[i];

			std::size_t pos          = arg.find_first_of('=');
			std::string valStr, name = arg.substr(1, pos - 1);

			if (m_flags.count(name) == 0) {
				if (m_aliases.count(name) == 0) {
					m_err = "Unknown flag '" + name + "'";

					return false;
				}

				name = m_aliases.at(name);
			}

			if (pos != std::string::npos)
				valStr = arg.substr(pos + 1);

			Value &flag = m_flags.at(name.c_str());

			switch (flag.type) {
			case Type::Bool:
				if (valStr == "" or valStr == "true")
					flag.data = true;
				else if (valStr == "false")
					flag.data = false;
				else {
					m_err = "Flag '" + name + "' expected bool (true/false), got '" + valStr + "'";

					return false;
				}

				break;

			case Type::Int64:
				try {
					flag.data = static_cast<std::int64_t>(stoll(valStr));
				} catch (...) {
					m_err = "Flag '" + name + "' expected integer, got '" + valStr + "'";

					return false;
				}

				break;

			case Type::Size:
				try {
					flag.data = static_cast<std::size_t>(stoull(valStr));
				} catch (...) {
					m_err = "Flag '" + name + "' expected unsigned integer, got '" + valStr + "'";

					return false;
				}

				break;

			case Type::Float:
				try {
					flag.data = stof(valStr);
				} catch (...) {
					m_err = "Flag '" + name + "' expected float, got '" + valStr + "'";

					return false;
				}

				break;

			case Type::Str:
				flag.data = valStr;

				break;
			}
		}

		return true;
	}

	const char *Error() {
		return m_err.c_str();
	}

	std::vector<const char*> Args() {
		return m_args;
	}

private:
	enum class Type {
		Bool = 0,
		Int64,
		Size,
		Float,
		Str
	};

	using Data = std::variant<bool, std::int64_t, std::size_t, float, std::string>;

	struct Value {
		Value(): type(Type::Bool) {}
		Value(Type p_type, const Data &p_data, const char *p_desc):
			type(p_type),
			data(p_data),
			defData(p_data),
			desc(p_desc)
		{}

		const char *TypeToStr() const {
			switch (type) {
			case Type::Bool:  return "bool";
			case Type::Int64: return "int64";
			case Type::Size:  return "size";
			case Type::Float: return "float";
			case Type::Str:   return "str";

			default: return "???";
			}
		}

		Type        type;
		Data        data;
		Data        defData;
		const char *desc;
	};

	std::string m_err;

	std::vector<const char*> m_args;
	std::unordered_map<std::string, std::string> m_aliases;
	std::unordered_map<std::string, Value>       m_flags;
};

#endif
