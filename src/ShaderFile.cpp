//
// Created by skyne on 4/26/2022.
//

#include "ShaderFile.h"

ShaderFile::ShaderFile(const std::string_view& path, unsigned int type) {
	this->path = std::string{ path };
	this->type = type;
}

unsigned int ShaderFile::get_type() const {
	return this->type;
}

std::string ShaderFile::get_path() const {
	return this->path;
}