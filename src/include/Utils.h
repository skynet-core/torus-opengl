#ifndef TORUS_UTILS_H
#define TORUS_UTILS_H

#include <string>
#include <string_view>

namespace Utils {

	std::string resolve_path(const std::string_view& file_path);
	std::string read_file(const std::string_view& path);

}

#endif // !TORUS_UTILS_H
