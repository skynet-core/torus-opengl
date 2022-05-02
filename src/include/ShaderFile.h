//
// Created by skyne on 4/26/2022.
//

#ifndef TORUS_SHADERFILE_H
#define TORUS_SHADERFILE_H

#include <string>

class ShaderFile {
public:

	explicit ShaderFile(const std::string_view& path, unsigned int type);

	[[nodiscard]] unsigned int get_type() const;

	[[nodiscard]] std::string get_path() const;

private:
	std::string path;
	unsigned int type;
};

#endif // TORUS_SHADERFILE_H
