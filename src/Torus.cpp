//
// Created by skyne on 4/25/2022.
//

#include "Torus.h"
#include <cmath>
#include <numbers>

Torus::Torus (float in_radius, float out_radius, int circles, int faces)
    : in_radius{ in_radius },
      out_radius{ out_radius }, circles{ circles }, faces{ faces }
{
  if (out_radius <= in_radius)
    {
      // todo: throw exception
    }

  vertices.resize ((faces + 1) * (circles + 1), glm::vec3 (0.0f));
  indices.resize (2 * faces * (circles + 1) + 2);
  // generate first ring ...
  float r = (out_radius - in_radius) / 2;
  double theta = (2 * std::numbers::pi) / faces;
  std::vector<glm::vec3> circle (faces + 1);
  for (int i = 0; i <= faces; ++i)
    {
      circle[i].x = (float)std::cos (theta * i) * r;
      circle[i].y = (float)0.0f;
      circle[i].z = (float)std::sin (theta * i) * r;
    }

  // generate circles ...
  theta = (2 * std::numbers::pi) / circles;
  int pos = 0;
  float R = in_radius + r;
  for (int i = 0; i <= circles; ++i)
    {
      auto cosT = (float)std::cos (theta * i);
      auto sinT = (float)std::sin (theta * i);
      for (int j = 0; j <= faces; ++j)
        {
          vertices[i * (faces + 1) + j].x = cosT * (R + circle[j].x);
          vertices[i * (faces + 1) + j].y = sinT * (R + circle[j].x);
          vertices[i * (faces + 1) + j].z = circle[j].z;
          if (i < circles)
            {
              indices[pos++] = i * (faces + 1) + j;       // tl
              indices[pos++] = (i + 1) * (faces + 1) + j; // tl
            }
        }
    }
    indices[pos++] = 0;
    indices[pos++] = (faces + 1);
  // calculate normals
  normals.resize (vertices.size ());
  glm::vec3 a, b, c, norm;
  for (int i = 0; i < indices.size () - 2; ++i)
    {

      a = vertices[indices[i]];
      b = vertices[indices[i + 1 + (i % 2)]];
      c = vertices[indices[i + 2 - (i % 2)]];

      norm = glm::normalize (glm::cross (a - b, c - b));

      normals[indices[i]] += norm;
      normals[indices[i + 1]] += norm;
      normals[indices[i + 2]] += norm;
    }
  // calculate texture coords
  // define primitive draw type
}

Torus::Torus (float in_radius, float out_radius, int circles)
    : Torus (in_radius, out_radius, circles, circles)
{
}

std::vector<unsigned int>
Torus::get_indices () const
{
  return indices;
}

std::vector<glm::vec3>
Torus::get_vertices () const
{
  return vertices;
}

std::vector<glm::vec3>
Torus::get_normals () const
{
  return normals;
}

std::vector<glm::vec2>
Torus::get_text_coords () const
{
  return tex_coords;
}
