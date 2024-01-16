#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "Core/Colors.h"
#include "Core/Vectors.h"
#include "Rasterizer.h"
#include "Texture.h"

static std::vector<std::string> SplitAll(std::string s, std::string delimeter) {
    std::vector<std::string> splited_string{};

    size_t pos = 0;
    std::string token;

    while ((pos = s.find(delimeter)) != std::string::npos) {
        token = s.substr(0, pos);
        splited_string.push_back(token);
        s.erase(0, pos + delimeter.length());
    }
    splited_string.push_back(s);

    return splited_string;
}

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    bool smooth;
};

struct Material {};

struct Model {
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<std::string> meshes_names;
    std::vector<Texture<Color>> textures;

    void Debug() {
        spdlog::debug("Debugging \"{}\"", name);
        for (size_t i = 0; i < meshes.size(); i++) {
            spdlog::debug("\tMesh: \"{}\"", meshes_names[i]);

            spdlog::debug("\t\tVertices:");
            for (size_t j = 0; j < meshes[i].vertices.size(); j++) {
                spdlog::debug("\t\t\t{} pos: ({:.2f}, {:.2f}, {:.2f}), uv: ({:.2f}, {:.2f}), nrml: ({:.2f}, "
                              "{:.2f}, {:.2f}), color: ({}, {}, {}, {})",
                              j, meshes[i].vertices[j].position.x, meshes[i].vertices[j].position.y,
                              meshes[i].vertices[j].position.z, meshes[i].vertices[j].uv.x, meshes[i].vertices[j].uv.y,
                              meshes[i].vertices[j].normal.x, meshes[i].vertices[j].normal.y,
                              meshes[i].vertices[j].normal.z, meshes[i].vertices[j].color.r,
                              meshes[i].vertices[j].color.g, meshes[i].vertices[j].color.b,
                              meshes[i].vertices[j].color.a);
            }
            spdlog::debug("\t\tIndices:");
            for (size_t j = 0; j < meshes[i].indices.size() / 3; j++) {
                spdlog::debug("\t\t\t{} {} {} {}", j, meshes[i].indices[j * 3 + 0], meshes[i].indices[j * 3 + 1],
                              meshes[i].indices[j * 3 + 2]);
            }
            spdlog::debug("Smooth shading {}", meshes[i].smooth);
        }
    }

    static Model LoadFromObj(const std::string &file) {
        spdlog::stopwatch obj_stopwatch;

        std::vector<Vector2> obj_uvs{};
        std::vector<Vector3> obj_pos{};
        std::vector<Vector3> obj_nrml{};

        Color col;

        Model model{};

        // Load the file
        std::ifstream obj_file(file);

        model.name = file.substr(file.find_last_of("/") + 1, file.find_last_of("."));

        // Parse the file
        if (obj_file.is_open()) {
            spdlog::trace("Opened file \"{}\"", file);
            std::string obj_line{};
            int line = 1;
            while (obj_file) {
                std::getline(obj_file, obj_line);
                const auto tokens = SplitAll(obj_line, " ");

                if (tokens[0] == "mtllib") {
                    size_t pos = file.find_last_of("/");
                    std::string mtl_path = file.substr(0, pos + 1) + tokens[1];
                    // spdlog::trace("Material Path \"{}\"", mtl_path);
                } else if (tokens[0] == "o") {
                    // spdlog::trace("Current Mesh \"{}\"", tokens[1]);
                    model.meshes_names.push_back(tokens[1]);
                    Mesh dummy_mesh{};
                    model.meshes.push_back(dummy_mesh);
                    col = Color(rand() % 255, rand() % 255, rand() % 255);
                } else if (tokens[0] == "v") {
                    Vector3 pos{};
                    pos.x = std::atof(tokens[1].c_str());
                    pos.y = std::atof(tokens[2].c_str());
                    pos.z = std::atof(tokens[3].c_str());
                    obj_pos.push_back(pos);
                } else if (tokens[0] == "vn") {
                    Vector3 nrml{};
                    nrml.x = std::atof(tokens[1].c_str());
                    nrml.y = std::atof(tokens[2].c_str());
                    nrml.z = std::atof(tokens[3].c_str());
                    obj_nrml.push_back(nrml);
                } else if (tokens[0] == "vt") {
                    Vector2 uv{};
                    uv.x = std::atof(tokens[1].c_str());
                    uv.y = std::atof(tokens[2].c_str());
                    obj_uvs.push_back(uv);
                } else if (tokens[0] == "f") {
                    for (size_t i = 1; i < 4; i++) {
                        const auto index = SplitAll(tokens[i], "/");

                        Vertex vertex{};
                        vertex.position = obj_pos[std::atoi(index[0].c_str()) - 1];
                        vertex.uv = obj_uvs[std::atoi(index[1].c_str()) - 1];
                        vertex.normal = obj_nrml[std::atoi(index[2].c_str()) - 1];
                        // vertex.color = Color(vertex.uv.x * 255, vertex.uv.y * 255, 0);
                        // vertex.color = Color(255, 255, 255);
                        vertex.color = Color(rand() % 255, rand() % 255, rand() % 255);
                        vertex.color = col;

                        // TODO: use indices
                        model.meshes[model.meshes.size() - 1].vertices.push_back(vertex);
                    }

                } else if (tokens[0] == "s") {
                    const auto smooth = static_cast<bool>(std::atoi(tokens[1].c_str()));
                    model.meshes[model.meshes.size() - 1].smooth = smooth;
                    spdlog::trace("Smooth shading {}", smooth);
                }
                line++;
            }
        } else {
            spdlog::error("Failed to open file \"{}\"", file);
        }

        spdlog::info("Model took {:.3}s to load \"{}\"", obj_stopwatch, file);

        return model;
    }
};