#include <iostream>

#include "flag.hh"

int main(int p_argc, char **p_argv) {
	Flag flag;

	// Create a flag for showing the usage, and give it an alias so both -h and --help are valid
	flag.Bool("h",  false, "Show the usage");
	flag.Alias("h", "-help");

	// std::int64_t flag
	flag.Int64("num",  32, "A number");
	flag.Alias("num", "int");

	// std::size_t flag
	flag.Size("size",   0,   "A size");

	// float flag
	flag.Float("float", 1.5, "A floating point number");

	// std::string flag
	flag.Str("msg",     "",  "Message to output");

	// If there was an error, output it and exit
	if (not flag.Parse(p_argc, p_argv)) {
		std::cout << flag.Error() << std::endl;

		return 1;
	}

	// Check if the help flag is true
	if (flag.Bool("h")) {
		std::cout << "Usage of " << p_argv[0] << ":\n";
		flag.PrintDefaults(std::cout, 2); // Print default usage to std::cout with indent size 2

		return 0;
	}

	std::cout << "num:   " << flag.Int64("num")   << std::endl;
	std::cout << "size:  " << flag.Size("size")   << std::endl;
	std::cout << "float: " << flag.Float("float") << std::endl;
	std::cout << "msg:   " << flag.Str("msg")     << std::endl;

	// Other command line arguments that are not flags
	std::cout << "\nOther arguments:\n";
	for (const auto &arg : flag.Args())
		std::cout << arg << "\n";

	return 0;
}
